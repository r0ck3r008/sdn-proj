import importlib as ilib
import multiprocessing as mt
import threading as thr
import time
from ryu.base import app_manager
from ryu.ofproto import ofproto_v1_2
from ryu.controller import ofp_event
from ryu.controller.handler import set_ev_cls
from ryu.controller.handler import MAIN_DISPATCHER
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types

main=ilib.import_module('main', '/ryu/apps')
gt=ilib.import_module('global_defs', '/ryu/apps')


#every class needs to inherit app_manager RyuApp class
class learn_sw(app_manager.RyuApp):
    #define ofp version
    OFP_VERSIONS=[ofproto_v1_2.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(learn_sw, self).__init__(*args, **kwargs)
        self.mac_to_port={}
        init_process=mt.Process(target=main.init_script, args=[])
        init_process.start()
        print('Main init process started')

    def add_flow(self, datapath, port, dst, src, actions):
        #print('Calling add_flow in learning_switch')
        #extrace openflow protocol
        ofproto=datapath.ofproto

        #create a match entry for the new flow
        match=datapath.ofproto_parser.OFPMatch(
                in_port=port,
                eth_dst=dst,
                eth_src=src)

        #make the instruction list that must be followed if match takes place
        #if dst not in gt.blacklist:
        #    inst=[datapath.ofproto_parser.OFPInstructionActions(
        #        ofproto.OFPIT_APPLY_ACTIONS, actions)]
        
        inst=[datapath.ofproto_parser.OFPInstructionActions(
            ofproto.OFPIT_CLEAR_ACTIONS, [])]


        mod=datapath.ofproto_parser.OFPFlowMod(
            datapath=datapath, cookie=0, cookie_mask=0, table_id=0,
            command=ofproto.OFPFC_ADD, idle_timeout=0, hard_timeout=0,
            priority=0, buffer_id=ofproto.OFP_NO_BUFFER, out_port=ofproto.OFPP_ANY,
            out_group=ofproto.OFPG_ANY,
            flags=0, match=match, instructions=inst)
        datapath.send_msg(mod)


    #decorator function decorated pkt_fwd function
    #has EventOFPPacketIn as first argument 
    #has second argument main_dispatcher as main handeller
    #event as argument
    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def pkt_fwd(self, ev):
        msg=ev.msg
        datapath=msg.datapath
        ofproto=datapath.ofproto
        in_port=msg.match['in_port']

        pkt=packet.Packet(msg.data)
        eth=pkt.get_protocols(ethernet.ethernet)[0]

        if eth.ethertype==ether_types.ETH_TYPE_LLDP:
            return
        
        src=eth.src
        dst=eth.dst

        dpid=datapath.id
        self.mac_to_port.setdefault(dpid, {})

        self.mac_to_port[dpid][src]=in_port

        if dst in self.mac_to_port[dpid]:
            out_port=self.mac_to_port[dpid][dst]
        elif dst in  gt.blacklist:
            pass
            #need bcast function here
        else:
            out_port=ofproto.OFPP_FLOOD

        actions=[datapath.ofproto_parser.OFPActionOutput(out_port)]

        if out_port!=ofproto.OFPP_FLOOD:
            self.add_flow(datapath, in_port, dst, src, actions)

        data=None
        if msg.buffer_id==ofproto.OFP_NO_BUFFER:
            data=msg.data

        out=datapath.ofproto_parser.OFPPacketOut(
                datapath=datapath, buffer_id=msg.buffer_id, in_port=in_port,
                actions=actions, data=data)
        datapath.send_msg(out)

