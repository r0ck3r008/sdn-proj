from getpass import getpass
import importlib as ilib
import multiprocessing as mt
import sys
from argparse import ArgumentParser
from mininet.net import Mininet
from mininet.cli import CLI as cli
from mininet.log import setLogLevel
from mininet.node import RemoteController

init_nodes=ilib.import_module('init_nodes', '/topos')
db_handler=ilib.import_module('db_handler', '/topos')

def add_links(ctrlr_list, switch_list, host_list):
    final_dict={}

    return final_dict


def topo(controllers, switches, hosts):
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
    final_dict=add_links(ctrlr_list, switch_list, host_list)

    #fork and update

    #necessary shananigans

    return True

if __name__=='__main__':
    #cmdline arguments
    parser=ArgumentParser()
    parser.add_argument('-s', '--switches', type=int, required=True, dest='switches', metavar='', help='The number of switches in topology')
    parser.add_argument('-H', '--hosts', type=int, required=True, dest='hosts', metavar='', help='The number of hosts in topology')
    parser.add_argument('-d', '--db_ip:port', required=True, dest='host', metavar='', help='Input the IP, Port of the database server')
    parser.add_argument('-u', '--uname', required=True, dest='uname', metavar='', help='Input the username for the database server')
    args=parser.parse_args()

    #controllers db setup
    passwd=getpass('Enter database passwd for user {}: '.format(args.uname))

    #fetch conttrollers
    controllers=db_handler.fetch_controllers(args.host, args.uname, passwd)

    #form topology
    setLogLevel('info')
    hosts=topo(controllers, args.switches, args.hosts)

    #update hosts database for each controller
    db_handler.update_controller_db(hosts)
    #end
