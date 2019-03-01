from ryu.base import app_manager
from ryu.controller import ofp_event
from ryu.controller.handler import MAIN_DISPATCHER
from ryu.controller.handler import set_ev_cls
from ryu.ofproto import ofproto_v1_2
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types
from threading import Thread as thread, Lock as lock
#import importlib

#helper=importlib.import_module('helper', '/ryu/apps/rootsw_ctrlr')
#cfg=importlib.import_module('cfg', '/ryu/apps/rootsw_ctrlr')

class SimpleSwitch12(app_manager.RyuApp):
    OFP_VERSIONS = [ofproto_v1_2.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(SimpleSwitch12, self).__init__(*args, **kwargs)
        #global definitions
        self.mac_to_port = {}
        self.blacklist=[]
        rel_addr=(cfg.rel_addr, cfg.rel_port)
        self_port=(12346 if cfg.port==None else cfg.port)
        pipe_name=('pipe' if cfg.pipe_name==None else cfg.pipe_name)
        self.mtx=lock()
        #start server thread
        svr_thr=thread(target=helper.svr_run, args=(self_port, self.blacklist, self.mtx))
        svr_thr.start()

        #start client thread
        cli_thr=thread(target=helper.cli_run, args=(rel_addr, pipe_name))
        cli_thr.start()

    def add_flow(self, datapath, port, dst, src, actions):
        ofproto = datapath.ofproto

        match = datapath.ofproto_parser.OFPMatch(in_port=port,
                                                 eth_dst=dst,
                                                 eth_src=src)
        self.mtx.acquire()
        if dst not in self.blacklist:
            inst = [datapath.ofproto_parser.OFPInstructionActions(
                    ofproto.OFPIT_APPLY_ACTIONS, actions)]
        else:
            inst = [datapath.ofproto_parser.OFPInstructionActions(
                    ofproto.OFPIT_CLEAR_ACTIONS, [])]
        self.mtx.release()

        mod = datapath.ofproto_parser.OFPFlowMod(
            datapath=datapath, cookie=0, cookie_mask=0, table_id=0,
            command=ofproto.OFPFC_ADD, idle_timeout=0, hard_timeout=0,
            priority=0, buffer_id=ofproto.OFP_NO_BUFFER,
            out_port=ofproto.OFPP_ANY,
            out_group=ofproto.OFPG_ANY,
            flags=0, match=match, instructions=inst)
        datapath.send_msg(mod)

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

        self.logger.info("packet in %s %s %s %s", dpid, src, dst, in_port)

        # learn a mac address to avoid FLOOD next time.
        self.mac_to_port[dpid][src] = in_port

        self.mtx.acquire()
        if dst in self.blacklist:
            out_port=ofproto.OFPP_IN_PORT
        elif dst in self.mac_to_port[dpid]:
            out_port = self.mac_to_port[dpid][dst]
            #check here if contents of this match the self.mac, if yes, means arp is done, send to relay
        else:
            out_port = ofproto.OFPP_FLOOD
        self.mtx.release()

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
