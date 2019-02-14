from mininet.net import Mininet
from mininet.log import setLogLevel
from mininet.cli import CLI as cli
from mininet.node import RemoteController as rc
from argparse import ArgumentParser
from MySQLdb import connect
from getpass import getpass
from sys import exit

####DATABASE FUNCTIONS####
def init_db(host, uname, passwd, db_name):
    try:
        conn=connect(host, user=uname, passwd=passwd, db=db_name)
        print('[!]Connected to database {}'.format(db_name))

        cur=conn.cursor()
        return (conn, cur)
    except Exception as e:
        print('[-]Error in connecting to db {}: {}'.format(db_name, e))
        exit(-1)

def send_query(t, query):
    try:
        t[1].execute(query)

        if 'update' in query.lower() or 'insert' in query.lower():
            t[0].commit()

        print('[!]Query {} executed successfully'.format(query))

        rows=t[1].fetchall()
        ret=[]
        for r in rows:
            ret.append(i[0])

        return ret
    except Exception as e:
        print('[-]Error in executing query {}: {}'.format(query, e))
        return False

def update_db(t, topo, dump_mac):
    ctrlr_ips=topo.keys()
    for ip in ctrlr_ips:
        #create table query
        query="CREATE TABLE `{}` (macs varchar(50));".format(ip)
        if send_query(t, query)==False:
            print('[-]Error in executing {}'.format(query))
        
        #insert query
        query="INSERT INTO `{}` VALUES ('{}'), ".format(ip, dump_mac)
        for h in topo[ip][1]:
            query="".join([query, "('{}')".format(h.MAC())])
            if h==topo[ip][1][-1]:
                query="".join([query, ";"])
            else:
                query="".join([query, ", "])
        if send_query(t, query)==False:
            print('[-]Error in executing query {}'.format(query))

####DATABASE DUNCTIONS####

####TOPOLOGY FUNCTIONS####
def parse_ctrlr_file(fname):
    ctrlr_ips=[]
    print('{}'.format(fname))
    with open(fname, 'r') as f:
        ctrlr_ips=f.read().strip().split('\n')

    print('[!]The controllers are detected at: {}'.format(ctrlr_ips))
    return ctrlr_ips

def init_switches(net, subnets):
    print('***Initiating switches***')
    rel_sw=net.addSwitch('s{}'.format(subnets+1))
    switches=[[rel_sw], []]

    for i in range(1, subnets+1):
        switches[1].append(net.addSwitch('s{}'.format(i)))

    return switches

def init_hosts(net, subnets, hosts_num):
    print('***Initiating hosts***')
    hosts=[]
    k=1
    for i in range(1, subnets+1):
        hosts.append([])
        for j in range(1, hosts_num+1):
            hosts[i-1].append(net.addHost('h{}s{}'.format(j, i), ip='192.168.1.{}'.format(k)))
            k+=1

    return hosts

def form_topo(net, ctrlr_ips, switches, hosts):
    print('***Forming Links***')
    topo={}

    for i in range(len(switches[1])):
        sw=switches[1][i]
        net.addLink(switches[0][0], sw)
        topo[ctrlr_ips[i]]=[[sw], []]
        for h in hosts[i]:
            net.addLink(sw, h)
            topo[ctrlr_ips[i]][1].append(h)

    return topo

def init_ctrlrs(net, ctrlr_ips):
    ctrlrs=[]
    for i in range(1, len(ctrlr_ips)+1):
        ctrlrs.append(net.addController('c{}'.format(i), controller=rc, ip=ctrlr_ips[i-1], port=6633))

    return ctrlrs

def form_ctrlr_links(ctrlrs, switches):
    print('***Starting switches with ctrlrs***')
    for i in range(len(switches[1])):
        switches[1][i].start([ctrlrs[i]])

    switches[0][0].start([ctrlrs[-1]])
####TOPOLOGY FUNCTIONS####

####MAIN CALLEE####
if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-c', '--ctrlr_file', required=True, dest='ctrlr_file', metavar='', help="The file containing the controllers' IP")
    parser.add_argument('-d' '--db_host', required=True, dest='db_host', metavar='', help="The db image host IP")
    parser.add_argument('-u', '--uname', dest='uname', metavar='', help="The user name to login to the database")
    parser.add_argument('-s', '--subnets', dest='subnets', required=True, type=int, metavar='', help="The number of subnets")
    parser.add_argument('-H', '--hosts', dest='hosts', required=True, type=int, metavar='', help="The number of hosts per subnet")
    args=parser.parse_args()

    #parse ctrlr file
    ctrlr_ips=parse_ctrlr_file(args.ctrlr_file)

    #check sanity
    if len(ctrlr_ips)!=(args.subnets+1):
        print('[-]Error subnets number and controllers dont match.')
        exit(-1)

    #mininet
    setLogLevel('info')
    net=Mininet(topo=None, autoSetMacs=True)

    #form switches
    switches=init_switches(net, args.subnets)

    #form hosts
    hosts=init_hosts(net, args.subnets, args.hosts)

    #form topology dict
    topo=form_topo(net, ctrlr_ips, switches, hosts)

    #calc dump mac
    dump_mac=switches[0][0].MAC('s{}-eth1'.format(args.subnets+1))

    #update db
    passwd=getpass('Enter the password for username {}: '.format('topology' if args.uname==None else args.uname))
    conn_net, cur_net=init_db(args.db_host, ('topology' if args.uname==None else args.uname), passwd, 'network')
    update_db((conn_net, cur_net), topo, dump_mac)

    #instructional wait
    print("[!]Networks Databse has been updated, please throughly check for each controller, start every controller and THEN type 'BUILD' below to build the mininet topology...")
    while True:
        inst=str(raw_input('[>] '))
        if inst=='BUILD' or inst=='build':
            break
        else:
            print('[-]Invalid instruction!!')

    #init controllers
    ctrlrs=init_ctrlrs(net, ctrlr_ips)

    #form links
    form_ctrlr_links(ctrlrs, switches)

    #necessary shananigans
    net.build()
    cli(net)
    net.stop()

####MAIN CALEE####
