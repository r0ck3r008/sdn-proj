from ryu.base import app_manager
from ryu.controller import ofp_event
from ryu.controller.handler import MAIN_DISPATCHER
from ryu.controller.handler import set_ev_cls
from ryu.ofproto import ofproto_v1_2
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types
from getpass import getpass
from importlib import import_module
from threading import Lock as lock, Thread as thread
from sys import stderr, exit

cfg=import_module('cfg', '.')
utils=import_module('utils', '.')

mtx=lock()
blackhosts=[]
dwnlnk_mtx=lock()

class SimpleSwitch12(app_manager.RyuApp):
    OFP_VERSIONS = [ofproto_v1_2.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(SimpleSwitch12, self).__init__(*args, **kwargs)
        #global definitions
        self.mac_to_port = {}
        self.count=0
        self.blacklist=[]
        self.rel_addr=(cfg.rel_addr, cfg.rel_port)

        #start processes
        self.dwnlnk_svr_sock=utils.sock_create((utils.get_self_ip(), 12346), 0)
        self.dwnlnk_svr_proc=thread(target=self.dwnlnk_svr_loop)
        self.dwnlnk_svr_proc.start()

        self.uplnk_sock=utils.sock_create(self.rel_addr, 1)

        #connect to db
        db_host=cfg.db_host
        uname='ctrlr' if cfg.uname==None else cfg.uname
        passwd=getpass('[>]Enter passwd for uname {}: '.format(uname))
        db_name='network' if cfg.db_name==None else cfg.db_name
        self.conn, self.cur=utils.init_db_cxn(db_host, uname, passwd, db_name)

        #get hosts (all)
        tables=utils.send_query((self.conn, self.cur), "SHOW TABLES;")
        self.hosts=[]
        for t in tables:
            ret=utils.send_query((self.conn, self.cur), "SELECT macs FROM `{}`;".format(t))
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
            idle_timeout=2
            hard_timeout=2

        mod = datapath.ofproto_parser.OFPFlowMod(
            datapath=datapath, cookie=0, cookie_mask=0, table_id=0,
            command=ofproto.OFPFC_ADD, idle_timeout=idle_timeout, hard_timeout=hard_timeout,
            priority=priority, buffer_id=ofproto.OFP_NO_BUFFER,
            out_port=ofproto.OFPP_ANY,
            out_group=ofproto.OFPG_ANY,
            flags=0, match=match, instructions=inst)
        datapath.send_msg(mod)

    def find_bad_mac(self, in_port):
        vals=self.mac_to_port.values()[0]
        ports=vals.values()
        macs=vals.keys()

        return macs[ports.index(in_port)]

    def dwnlnk_svr_loop(self):
        print('[!]Started downlink server loop')
        sock=None
        try:
            sock, addr=self.dwnlnk_svr_sock.accept()
            print('[!]Got connection back from {}'.format(addr))

            i=0
            while True:
                with dwnlnk_mtx:
                    cmdr=utils.rcv(sock, addr)
                print('[!]Received {} from relay'.format(cmdr))
                cmd, app=cmdr.split('=')
                if cmd=='BLACKLIST':
                    with mtx:
                        if app not in blackhosts:
                            blackhosts.append(app)
                            print('[!]Appended {} to blackhosts'.format(app))
                else:
                    with mtx:
                        if app in blackhosts:
                            blackhosts.remove(app)
                            print('[!]Removed {} from blackhosts'.format(app))
        except Exception as e:
            stderr.write('[-]Error in dwnlnk_svr_loop: {}'.format(e))
            self.dwnlnk_svr_sock.close()
            if sock!=None:
                sock.close()
            exit(-1)

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
        self.logger.info("packet in %s %s %s %s number %s", dpid, src, dst, in_port, self.count)

        if dst not in self.hosts and dst!='ff:ff:ff:ff:ff:ff' and '33:33' not in dst.lower():
            self.add_flow(datapath, in_port, dst, src, [])
            bad_mac=self.find_bad_mac(in_port)
            self.logger.info('[!]Blacklisting {} port for MAC {}'.format(in_port, bad_mac))
            utils.snd(self.uplnk_sock, 'BLACKLIST={}'.format(bad_mac), self.rel_addr)
            return
        elif dst in blackhosts:
            self.logger.info('[-]Forbidden destination {}!!'.format(dst))
            self.add_flow(datapath, in_port, dst, src, [])
            return
        elif dst in self.mac_to_port[dpid]:
            out_port = self.mac_to_port[dpid][dst]
        else:
            out_port = ofproto.OFPP_FLOOD

        if src in blackhosts:
            self.logger.info('[!]Whitelisting {} port for MAC {}'.format(in_port, src))
            utils.snd(self.uplnk_sock, 'WHITELIST={}'.format(src), self.rel_addr)

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
