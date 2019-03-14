from mininet.net import Mininet
from mininet.node import RemoteController as rc
from mininet.cli import CLI as cli
from mininet.log import setLogLevel
from getpass import getpass
from MySQLdb import connect
from argparse import ArgumentParser
from libnacl import randombytes_uniform as ru, sodium_init
from sys import exit

#database
def init_db(db_host, uname, passwd, db_name):
    conn=None
    try:
        conn=connect(db_host, user=uname, passwd=passwd, db=db_name)
        print('[!]Connected under the uname: {}'.format(uname))

        cur=conn.cursor()

        return (conn, cur)
    except Exception as e:
        print('[-]Error in connecting under uname {}: {}'.format(uname, e))
        if conn!=None:
            conn.close()

def send_query(t, query):
    try:
        t[1].execute(query)
        t[0].commit()
        print('[!]Query executed Successfully')
    except Exception as e:
        print('[-]Error in executing query {}: {}'.format(query, e))


def update_db(t, topo):
    subnets=topo.values()
    ctrlr_ip=topo.keys()
    for i in range(len(ctrlr_ip)):
        subnet=subnets[i]
        ip=ctrlr_ip[i]
        #create table
        query="CREATE TABLE `{}` (macs varchar(50));".format(ip)
        send_query(t, query)
        #inset vals
        query="INSERT INTO `{}` VALUES ('{}'), ".format(ctrlr_ip[i], subnet[0][0].MAC('s{}-eth{}'.format(i+1, len(subnets[1])+1)))
        for h in subnet[1]:
            query="".join([query, "('{}') ".format(h.MAC())])
            if h==subnet[1][-1]:
                query="".join([query, ";"])
            else:
                query="".join([query, " , "])
        send_query(t, query)

#databse

#mininet
def parse_ctrlr_file(fname):
    ctrlr_ip=[]
    with open(fname, 'r') as f:
        ctrlr_ip=f.read().strip().split('\n')

    print('[!]Registered controller IPs are: {}'.format(ctrlr_ip))

    return ctrlr_ip

#topo is {"ctrlr ip": [["root_sw"], ["hosts"]]}
def init_subnets(net, ctrlr_ip, n_subnets, n_hosts):
    topo={}
    
    for i in range(n_subnets):
        topo[ctrlr_ip[i]]=[[], []]
        sw=net.addSwitch('s{}'.format(i+1))
        topo[ctrlr_ip[i]][0].append(sw)

        for j in range(1, n_hosts+1):
            h=net.addHost('h{}s{}'.format(j, i+1))
            net.addLink(sw, h)
            topo[ctrlr_ip[i]][1].append(h)

    return topo

def init_rel_sw(net, n_rel_sw, n_subnets):
    rel_sw={}
    k=0
    for i in range(n_subnets+1, (n_subnets+n_rel_sw+1)):
        sw=net.addSwitch('s{}'.format(i))
        rel_sw[sw]=2
        #the unlucky ones given stright line connectivity(terminal rel_sw)
        if k!=2:
            rel_sw[sw]=rel_sw[sw]-1
            k+=1

    rels=rel_sw.keys()
    n=len(rels)
    for i in range(n):
        sw=rels[i]
        rels.remove(sw)
        links=[]
        k=rel_sw[sw]
        if len(rels)==0:
            break
        for j in range(k):
            s=rels[ru(len(rels))]
            if rel_sw[s]!=0 and s not in links:
                net.addLink(sw, s)
                links.append(s)
                rel_sw[s]=rel_sw[s]-1
                rel_sw[sw]=rel_sw[sw]-1

    return rel_sw.keys()

def choice(t):
    ret=t[ru(len(t))]
    t.remove(ret)
    return ret

def assign_rel_sw(net, rel_sw, topo):
    subnets=topo.values()
    n=len(rel_sw)
    for i in range(len(rel_sw)):
        rel=choice(rel_sw)
        num=1
        if len(subnets)>len(rel_sw):
            if len(rel_sw)==0:
                #go all out
                num=len(subnets)
            else:
                #give random number of subnets
                num=num+ru(len(subnets)-len(rels))

        for j in range(num):
            subnet=choice(subnets)
            net.addLink(rel, subnet[0][0])


def init_ctrlrs(net, ctrlr_ip):
    ctrlrs=[]
    for i in range(1, len(ctrlr_ip)+1):
        ctrlrs.append(net.addController('c{}'.format(i), controller=rc, ip=ctrlr_ip[i-1], port=6633))

    return ctrlrs

def init_switches(ctrlrs, topo, rel_sw):
    ctrlr_ip=topo.keys()
    j=0
    for i in range(len(ctrlrs)):
        ctrlr=ctrlrs[i]
        if ctrlr.IP() in ctrlr_ip:
            topo[ctrlr.IP()][0][0].start([ctrlr])
        else:
            rel_sw[j].start([ctrlr])
            j+=1
#Mininet

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-c', '--ctrlr_file', required=True, dest='ctrlr_file', metavar='', help='The file with IP addresses of all the controllers')
    parser.add_argument('-d', '--db_host', required=True, dest='db_host', metavar='', help='The database host name')
    parser.add_argument('-u', '--uname', dest='uname', metavar='', help="The username under which database connection is made (Default 'topology')")
    parser.add_argument('-s', '--subnets', required=True, type=int, metavar='', dest='subnets', help='The number of subnets in the network')
    parser.add_argument('-r', '--rel_sw', required=True, type=int, metavar='', dest='rel_sw', help='The number of relay switches')
    parser.add_argument('-H', '--hosts', required=True, type=int, metavar='', dest='hosts', help='The number of hosts per subnet')
    args=parser.parse_args()

    #log
    setLogLevel('info')

    #ctrlr_ip file
    ctrlr_ip=parse_ctrlr_file(args.ctrlr_file)

    #sanity ch
    if len(ctrlr_ip)!=(args.subnets+args.rel_sw):
        print('[-]Not enough controllers available. Exiting...')
        exit(-1)

    #init libnacl
    sodium_init()

    #mininet
    net=Mininet(topo=None, autoSetMacs=True)

    #db init
    passwd=getpass('Enter the password for username {}: '.format(('topology' if args.uname==None else args.uname)))
    conn_net, cur_net=init_db(args.db_host, ('topology' if args.uname==None else args.uname), passwd, 'network')

    #form subnets
    topo=init_subnets(net, ctrlr_ip, args.subnets, args.hosts)

    #form relay switches
    rel_sw=init_rel_sw(net, args.rel_sw, args.subnets)

    #assign subnets to relay switches
    assign_rel_sw(net, [sw for sw in rel_sw], topo)

    #build topology
    net.build()

    #update db
    update_db((conn_net, cur_net), topo)

    #wait for controllers to come online
    print("[!]Startup controllers and then type 'BUILD' here to initiate further topology build...")
    ip=str(raw_input('[>] '))

    #init controllers
    ctrlrs=init_ctrlrs(net, ctrlr_ip)

    #init switches
    init_switches(ctrlrs, topo, rel_sw)

    #init cli
    cli(net)

    #stop
    net.stop()

    #end connection with db
    cur_net.close()
    conn_net.close()
