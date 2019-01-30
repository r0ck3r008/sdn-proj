from getpass import getpass
from importlib import import_module
from argparse import ArgumentParser
from mininet.net import Mininet
from mininet.node import RemoteController
from mininet.cli import CLI as cli
from libnacl import randombytes_uniform

db_handler=import_module('db_handler', '/ryu/apps')

def init_controllers(ctrlr_ip):
    t=[]
    i=0
    for ctrlr in ctrlr_ip:
       t.append(net.addController('c{}'.format(i), controller=RemoteController, ip=ctrlr, port=6633))
       i=i+1

    return t

def init_switches(num):
    t=[]
    for i in range(num):
        t.append(net.addSwitch('s{}'.format(i), listenPort=6634))

    return t

def init_hosts(num):
    t=[]
    for i in range(num):
        t.append(net.addHost('h{}'.format(i)))

    return t

def choice(t):
    ret=t[randombytes_uniform(len(t))]
    t.remove(ret)
    return ret

def make_topo(controllers, switches, hosts):
    topo={}
    #pair switch and controllers
    for i in range(len(controllers)):
        ctrlr=choice(controllers)
        switch=choice(switches)
        switch.start([ctrlr])
        topo[ctrlr]=[switch]

    print("****Creating Links****")
    #add switch host links
    ctrlrs=topo.keys()
    swi_num=len(ctrlrs)
    for ctrlr in ctrlrs:
        host_num=1
        if len(hosts)>swi_num:
            host_num=host_num+randombytes_uniform((len(hosts)-swi_num))
        for j in range(host_num):
            host=choice(hosts)
            net.addLink(topo[ctrlr][0], host, )
            topo[ctrlr].append(host)

    net.build()
    return topo


if __name__=='__main__':
    #args
    parser=ArgumentParser()
    parser.add_argumen('-D', '--db_host', required=True, metavar='', dest='db_host', help='Enter the database host name')
    parser.add_argumen('-u', '--db_uname', required=True, metavar='', dest='db_uname', help='Enter the database user name')
    parser.add_argumen('-H', '--hosts', required=True, metavar='', dest='hosts', type=int, help='Enter the number of hosts')
    parser.add_argumen('-s', '--switches', required=True, metavar='', dest='swi', type=int, help='Enter the database host name')
    args=parser.parse_args()

    #get passwd
    passwd=getpasswd('Enter Password for username {}: '.format(args.db_uname))

    #get connection
    global conn_ctrlr, conn_net, cur_ctrlr, cur_net
    conn_ctrlr, cur_ctrlr=db_handler.init_db(args.db_host, args.db_uname, passwd, 'controllers')
    conn_net, cur_net=db_handler.init_db(args.db_host, args.db_uname, passwd, 'network')

    #get controllers
    ctrlr_ip=db_handler.send_query("Select * from controllers;", (conn_ctrlr, cur_ctrlr))

    #check switch and controller num
    if args.swi!=ctrlr_ip.len():
        print('[!]Multiple switches under controllers unsupported!!!\nExiting now!')
        sys.exit(-1)

    #init topology
    global net
    net=Mininet(topo=None, build=False)

    #form controllers
    controllers=init_controllers(ctrlr_ip)

    #form switches
    switches=init_switches(args.swi)

    #form hosts
    hosts=init_hosts(args.hosts)

    #form topology
    topo=make_topo(controllers, switches, hosts)

    #update database
    ctrlrs=topo.keys()
    for ctrlr in ctrlrs:
        db_handler.update_network(ctrlr, topo[ctrlr], (conn_net, cur_net))

    #necessary shananigans
    cli(net)
    net.stop()
    conn_ctrlr.close()
    conn_net.close()

