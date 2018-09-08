from ryu.base import app_manager
from ryu.ofproto import ofproto_v1_2
from ryu.controller import ofp_event
from ryu.controller.handler import set_ev_cls
from ryu.controller.handler import MAIN_DISPATCHER
from ryu.lib.packet import packet
from ryu.lib.packet import ethernet
from ryu.lib.packet import ether_types

def read_file():
    global blacklist
    
    with open('blacklist', 'r') as f:
        blacklist=f.read().strip().split('\n')    
        
#every class needs to inherit app_manager RyuApp class
class learn_sw(app_manager.RyuApp):
    
    #define ofp version
    OFP_VERSIONS=[ofproto_v1_2.OFP_VERSION]

    def __init__(self, *args, **kwargs):
        super(learn_sw, self).__init__(*args, **kwargs)
        self.mac_to_port={}
        read_file()

    def add_flow(self, datapath, port, dst, src, actions):
        #extrace openflow protocol
        ofproto=datapath.ofproto

        #create a match entry for the new flow
        match=datapath.ofproto_parser.OFPMatch(
                in_port=port,
                eth_dst=dst,
                eth_src=src)

        #make the instruction list that must be followed if match takes place
        if dst not in blacklist:
            inst=[datapath.ofproto_parser.OFPInstructionActions(
                ofproto.OFPIT_APPLY_ACTIONS, actions)]
        else:
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
    @set_ev_cls(ofp_event.EventOFPPacketIn, MAIN_DISPATCHER)
    #event as argument
    def pkt_fwd(self, ev):
        #extract msg from the event
        msg=ev.msg
        #extract datapath, which entails basic info about the path of data flow
        #like ports macs etc
        datapath=msg.datapath
        #extract openflow protocol version
        ofproto=datapath.ofproto
        #extract in port
        in_port=msg.match['in_port']

        #msg.data has the raw data the packet has to offer
        #packet.Packet function extracts the data to pkt datatype
        pkt=packet.Packet(msg.data)
        #get the very first protocol or the physical layer header
        eth=pkt.get_protocols(ethernet.ethernet)[0]

        #ignore lldp packet as it doesnt have dst and source
        if eth.ethertype==ether_types.ETH_TYPE_LLDP:
            return
        
        #extract source and destination
        src=eth.src
        dst=eth.dst

        #extract datapath id
        dpid=datapath.id
        #set default value of dpid in mat_to_port dict equal to null dict
        self.mac_to_port.setdefault(dpid, {})

        #learn thr current src and dpid to avoid flood next time
        #this dictionary maps datapathid to perticular src mac and in_port
        self.mac_to_port[dpid][src]=in_port

        #assign outport equal to mac_to_port value corresponding same dpid and dst
        #if previously assigned, here if current src was to become a dst another
        #time, current in_port will be used as out_port that time, making them 
        #cling to each forever, other wise flood every possible port
        if dst in self.mac_to_port[dpid]:
            out_port=self.mac_to_port[dpid][dst]
        elif dst in blacklist:
            out_port=self.OFPP_IN_PORT
        else:
            out_port=ofproto.OFPP_FLOOD

        #assign actions list using OFPActionOutput
        actions=[datapath.ofproto_parser.OFPActionOutput(out_port)]

        #install a new flow to avoid packet_in function next time, provided pkt
        #is not flooded
        if out_port!=ofproto.OFPP_FLOOD:
            self.add_flow(datapath, in_port, dst, src, actions)

        data=None
        #check if the buffer is null, if yes assign msg.data to data
        if msg.buffer_id==ofproto.OFP_NO_BUFFER:
            data=msg.data

        out=datapath.ofproto_parser.OFPPacketOut(
                datapath=datapath, buffer_id=msg.buffer_id, in_port=in_port,
                actions=actions, data=data)
        datapath.send_msg(out)

