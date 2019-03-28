from ryu.base import app_manager
from ryu.controller import ofp_event
from ryu.controller.handler import MAIN_DISPATCHER
from ryu.controller.handler import set_ev_cls
from ryu.ofproto import ofproto_v1_2
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types
from MySQLdb import connect
from getpass import getpass
from socket import socket, AF_INET, SOCK_STREAM
from sys import stderr, exit
from time import time
from importlib import import_module

cfg=import_module('cfg', '.')

def init_cxn(db_host, uname, passwd, db_name):
    conn=None
    try:
        conn=connect(db_host, user=uname, passwd=passwd, db=db_name)
        print('[!]Connection successful to {} database...'.format(db_name))
        cur=conn.cursor()

        return (conn, cur)
    except Exception as e:
        stderr.write('[-]Error in getting connection to db {} under name {}: {}'.format(db_name, uname, e))
        if conn!=None:
            conn.close()
        exit(-1)

def send_query(t, query):
    try:
        t[1].execute(query)

        if 'insert' in query.lower() or 'insert' in query.lower() or 'delete' in query.lower():
            t[0].commit()

        rows=t[1].fetchall()
        ret=[]
        for r in rows:
            ret.append(r[0])

        return ret
    except Exception as e:
        stderr.write('[-]Error in executing query {}: {}'.format(query, e))

class SimpleSwitch12(app_manager.RyuApp):
    OFP_VERSIONS = [ofproto_v1_2.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(SimpleSwitch12, self).__init__(*args, **kwargs)
        #global definitions
        self.mac_to_port = {}
        self.blacklist=[]
        self.count=0

        #connect to db
        db_host=cfg.db_host
        uname='ctrlr' if cfg.uname==None else cfg.uname
        passwd=getpass('[>]Enter passwd for uname {}: '.format(uname))
        db_name='network' if cfg.db_name==None else cfg.db_name
        self.conn, self.cur=init_cxn(db_host, uname, passwd, db_name)

        #get hosts (all)
        tables=send_query((self.conn, self.cur), "SHOW TABLES;")
        self.hosts=[]
        for t in tables:
            ret=send_query((self.conn, self.cur), "SELECT macs FROM `{}`;".format(t))
            for r in ret:
                self.hosts.append(r)

        print('[!]Self hosts are {}'.format(self.hosts))

    def add_flow(self, datapath, port, dst, src, actions):
        ofproto = datapath.ofproto

        idle_timeout=1
        hard_timeout=5
        priority=0
        if actions!=[]:
            inst = [datapath.ofproto_parser.OFPInstructionActions(
                    ofproto.OFPIT_APPLY_ACTIONS, actions)]
            match = datapath.ofproto_parser.OFPMatch(in_port=port, eth_dst=dst, eth_src=src)
        else:
            inst = [datapath.ofproto_parser.OFPInstructionActions(
                    ofproto.OFPIT_CLEAR_ACTIONS, [])]
            match = datapath.ofproto_parser.OFPMatch(in_port=port)
            idle_timeout=0
            hard_timeout=0
            priority=1

        mod = datapath.ofproto_parser.OFPFlowMod(
            datapath=datapath, cookie=0, cookie_mask=0, table_id=0,
            command=ofproto.OFPFC_ADD, idle_timeout=idle_timeout, hard_timeout=hard_timeout,
            priority=priority, buffer_id=ofproto.OFP_NO_BUFFER,
            out_port=ofproto.OFPP_ANY,
            out_group=ofproto.OFPG_ANY,
            flags=0, match=match, instructions=inst)
        datapath.send_msg(mod)

    #mac_to_port is [dpid][mac][port]
    def find_bad_mac(self, in_port, copy):
        vals=copy.values()[0]
        macs=vals.keys()
        ports=vals.values()

        return macs[ports.index(in_port)]


    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def _packet_in_handler(self, ev):
        msg = ev.msg
        datapath = msg.datapath
        ofproto = datapath.ofproto
        in_port = msg.match['in_port']

        pkt = packet.Packet(msg.data)
        eth = pkt.get_protocols(ethernet.ethernet)[0]

        if eth.ethertype == ether_types.ETH_TYPE_LLDP:
            # ignore lldp packet
            return
        dst = eth.dst
        src = eth.src

        dpid = datapath.id
        self.mac_to_port.setdefault(dpid, {})

        self.count+=1
        self.logger.info("packet in %s %s %s %s %s", dpid, src, dst, in_port, self.count)

        if dst not in self.hosts and dst!='ff:ff:ff:ff:ff:ff' and '33:33' not in dst.lower():
            #blacklisting action
            self.add_flow(datapath, in_port, dst, src, [])
            copy=self.mac_to_port
            print('[!]Blacklisting {} port for MAC addr: {}'.format(in_port, self.find_bad_mac(in_port, copy)))
            if in_port not in self.blacklist:
                self.blacklist.append(in_port)
            return
        elif dst in self.mac_to_port[dpid]:
            out_port = self.mac_to_port[dpid][dst]
            #check here if contents of this match the self.mac, if yes, means arp is done, send to relay
        else:
            out_port = ofproto.OFPP_FLOOD

        # learn a mac address to avoid FLOOD next time.
        self.mac_to_port[dpid][src] = in_port

        actions = [datapath.ofproto_parser.OFPActionOutput(out_port)]

        # install a flow to avoid packet_in next time
        if out_port != ofproto.OFPP_FLOOD:
            self.add_flow(datapath, in_port, dst, src, actions)

        data = None
        if msg.buffer_id == ofproto.OFP_NO_BUFFER:
            data = msg.data

        out = datapath.ofproto_parser.OFPPacketOut(
            datapath=datapath, buffer_id=msg.buffer_id, in_port=in_port,
            actions=actions, data=data)
        datapath.send_msg(out)
