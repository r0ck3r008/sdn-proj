from argparse import ArgumentParser
from mininet.net import Mininet
from mininet.node import RemoteController as rc
from mininet.cli import CLI as cli
from libnacl import randombytes_uniform

def choice(t):
    ret=t[randombytes_uniform(len(t))]
    t.remove(ret)
    return ret

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-c', '--controller', required=True, metavar='', dest='ctrlr_ip', help='The IP of the controller')
    parser.add_argument('-H', '--hosts', required=True, type=int, metavar='', dest='hosts', help='The number of hosts to be added')
    parser.add_argument('-s', '--switches', required=True, type=int, metavar='', dest='swi', help='The number of switches to be added')
    args=parser.parse_args()

    print('Connecting to controller at: {}'.format(args.ctrlr_ip))

    net=Mininet(topo=None, build=False)

    h=[]
    for i in range(args.hosts):
        h.append(net.addHost('h{}'.format(str(i))))

    c0=net.addController('c0', controller=rc, ip=args.ctrlr_ip, port=6633)

    s=[]
    for i in range(args.swi):
        s.append(net.addSwitch('s{}'.format(str(i))))

    print('****adding links****')

    for swi in s:
        num=1
        if len(s)<len(h):
            if swi==s[-1]:
                #last switch, go all out
                num=len(h)
            else:
                num=num+randombytes_uniform(len(h)-len(s))

        for i in range(num):
            host=choice(h)
            net.addLink(swi, host, )
        swi.start([c0])

    net.build()
    cli(net)
    net.stop()
