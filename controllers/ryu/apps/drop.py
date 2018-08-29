from ryu.base import app_manager
from ryu.ofproto import ofproto_v1_2
from ryu.controller import ofp_event
from ryu.controller.handler import set_ev_cls
from ryu.controller.handler import MAIN_DISPATCHER
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types
import sys

def read_file():
    global macs_to_drop
    
    with open('blocked', 'r') as f:
        macs_to_drop=f.read().strip().split('\n')

class learn_switch(app_manager.RyuApp):
    mac_to_port={}
    #inform the version of openflow
    OFP_VERSIONS=[ofproto_v1_2.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(learn_switch, self).__init__(*args, **kwargs)
        read_file()

    #called after a packet_in_handler is called
    def add_flow(self, datapath, port, dst, src, actions):
        #extract ofproto ie openflow protocol version
        ofproto=datapath.ofproto

        #create a flow table match entry
        match=datapath.ofproto_parser.OFPMatch(in_port=port,
                                                eth_dst=dst,
                                                eth_src=src)

        #define the action to take give 'match' is a confirm
        if dst in macs_to_drop:
            inst=[datapath.ofproto_parser.OFPInstructionActions(
                    ofproto.OFPIT_CLEAR_ACTIONS, [])]
            print("Dropping pkt for %s" %(dst))
        else:
            inst=[datapath.ofproto_parser.OFPInstructionActions(
                    ofproto.OFPIT_APPLY_ACTIONS, actions)]

        #build the final mod
        mod=datapath.ofproto_parser.OFPFlowMod(
                datapath=datapath, cookie=0, cookie_mask=0, table_id=0,
                command=ofproto.OFPFC_ADD, idle_timeout=0, hard_timeout=0,
                priority=0, buffer_id=ofproto.OFP_NO_BUFFER,
                out_port=ofproto.OFPP_ANY,
                out_group=ofproto.OFPG_ANY,
                flags=0, match=match, instructions=inst)
        #install mod
        datapath.send_msg(mod)


    #decorator for pkt_in_handler, decorated by set_ev_cls
    #the ofp_event.EventOFPPacket_in defines which event is being called upon here
    #MAIN_DISPATCHER is a constant saying the final dispatch
    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    def pkt_in_handler(self, ev):
        #extract msg from ev
        msg=ev.msg
        #extract datapath from msg
        datapath=msg.datapath
        #extract ofproto, ie version of OFP from datapath
        ofproto=datapath.ofproto
        #in_port is extracted from msg
        in_port=msg.match['in_port']

        #pkt is parsed from raw on the wire data from msg via packet.Packet
        pkt=packet.Packet(msg.data)
        #get_protocols returns a list of all the protocols present in packet, iterable, here we use only the 0th one
        #we need only ethernet protocol to get dst and src
        eth=pkt.get_protocols(ethernet.ethernet)[0]

        #ignore LLDP as it is only one L1 proto header that doesnt give dst and src
        if eth.ethertype==ether_types.ETH_TYPE_LLDP:
            return
        #now the proto is definately ethernet protocol header
        #now extract dst and src from the ethernet protocol header
        dst=eth.dst
        src=eth.src

        #extracting the id given by openflow to the datapath
        dpid=datapath.id
        #??
        self.mac_to_port.setdefault(dpid, {})

        #learn mac addr to avoid flood next time
        #mac_to_port is a dictionary defined in set_ev_cls decorator function where first key is 'dpid' and the value assigned to it is again a dict whose key we set here as 'src' and value in_port
        #this ensures that if this 'src' is ever a 'dst' we know which port to send
        self.mac_to_port[dpid][src]=in_port
    
        #find the out_port in mac_to_port dict if available ie if ever set above
        if dst in self.mac_to_port[dpid]:
            out_port=self.mac_to_port[dpid][dst]
        else:
            #if not found flood every where
            out_port=ofproto.OFPP_FLOOD

        #set the output action to out_port
        actions=[datapath.ofproto_parser.OFPActionOutput(out_port)]

        #install a dataflow to avoid packet_in_handler next time
        if out_port!=ofproto.OFPP_FLOOD:
            self.add_flow(datapath, in_port, dst, src, actions)
        elif dst in macs_to_drop:
            self.add_flow(datapath, in_port, dst, src, [])

        data=None
        #assign msg raw data to 'data' provided theres no buffer
        if msg.buffer_id==ofproto.OFP_NO_BUFFER:
            data=msg.data

        #build a ofproto_parser.OFPPacketOut
        out=datapath.ofproto_parser.OFPPacketOut(
                datapath=datapath, buffer_id=msg.buffer_id, in_port=in_port,
                actions=actions, data=data)

        print('From %s->%s' %(src, dst))
        #sned_msg
        datapath.send_msg(out)

