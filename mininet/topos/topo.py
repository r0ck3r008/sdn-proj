from getpass import getpass
import importlib as ilib
import multiprocessing as mt
import sys
from secrets import randbelow, choice
from argparse import ArgumentParser
from mininet.net import Mininet
from mininet.cli import CLI as cli
from mininet.log import setLogLevel
from mininet.node import RemoteController

init_nodes=ilib.import_module('init_nodes', '/topos')
db_handler=ilib.import_module('db_handler', '/topos')

def gen_final_dict(ctrlr_list, switch_list, host_list):
    final_dict={}
    num_sw=len(switch_list)

    for i in range(num_sw):
        #choose ctrlr switch pair
        c=choice(ctrlr_list)
        s=choice(switch_list)
        t_c_sw=(c, s)
        ctrlr_list.remove(c)
        switch_list.remove(s)

        #choose hosts
        hosts=[]
        can_have=1
        if len(host_list)>num_sw:
            can_have=randbelow(len(host_list)-num_sw+1)
        for i in range(can_have):
            h=choice(host_list)
            host_list.remove(h)
            hosts.append(h)
        
        #append to dictionary
        final_dict[t_c_sw]=hosts

    return final_dict

def add_links(net, ctrlr_list, switch_list, host_list):
    n_sw=len(switch_list)
    n_ho=len(host_list)
    final_dict=gen_final_dict(ctrlr_list, switch_list, host_list)

    #add host links
    for t in final_dict:
        hosts=final_dict[t]
        for j in range(len(hosts)):
            net.addLink(t[1], hosts[j],)

    #add inter switch links
    tup=list(final_dict.keys())
    for i in range(n_sw):
        #take ith switch and link to all switches after that
        for j in range(i+1, n_sw):
            net.addLink(tup[i][1], tup[j][1],)

    net.build()

    #make switch-controller links
    for t in tup:
        t[1].start([t[0]])

    return final_dict


def topo(controllers, switches, hosts, host, uname, passwd):
    if len(controllers)>switches:
        print('[-]More switches are required!!')
        return False
    elif len(controllers)<switches:
        print('[-]Too many switches!!')
        return False

    #init new topo
    net=Mininet(topo=None, build=False)
    
    #add switches
    switch_list=init_nodes.init_switches(net, switches)

    #add controllers
    ctrlr_list=init_nodes.init_controllers(net, controllers)

    #add hosts
    host_list=init_nodes.init_hosts(net, hosts)

    #add links
    final_dict=add_links(net, ctrlr_list, switch_list, host_list)

    #fork and update
    for t in final_dict:
        h=final_dict[t]
        proc=mt.Process(target=db_handler.update_controller_db, args=[t, h, host, uname, passwd])
        proc.start()

    #necessary shananigans
    a=final_dict
    a[1].cmdPrint('ovs-vsctl show')
    cli(net)
    net.stop()

    return True

if __name__=='__main__':
    #cmdline arguments
    parser=ArgumentParser()
    parser.add_argument('-s', '--switches', type=int, required=True, dest='switches', metavar='', help='The number of switches in topology')
    parser.add_argument('-H', '--hosts', type=int, required=True, dest='hosts', metavar='', help='The number of hosts in topology')
    parser.add_argument('-d', '--db_ip:port', required=True, dest='host', metavar='', help='Input the IP, Port of the database server')
    parser.add_argument('-u', '--uname', required=True, dest='uname', metavar='', help='Input the username for the database server')
    args=parser.parse_args()

    if args.hosts < args.switches:
        print('[-]Error, need more hosts!!')
        sys.exit(-1)

    #controllers db setup
    passwd=getpass('Enter database passwd for user {}: '.format(args.uname))

    #fetch conttrollers
    controllers=db_handler.fetch_controllers(args.host, args.uname, passwd)

    #form topology
    setLogLevel('info')
    hosts=topo(controllers, args.switches, args.hosts, host, uname, passwd)

    #update hosts database for each controller
    db_handler.update_controller_db(hosts)
    #end
