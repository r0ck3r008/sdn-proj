#!/usr/bin/python
from argparse import ArgumentParser
import ctypes as ct
import sys

def init_functions():
    #create sock
    lib.create_sock.restype=ct.c_int
    lib.create_sock.argtypes=[ct.c_char_p]

    #server_workings
    lib.server_workings.restype=ct.c_int
    lib.server_workings.argtypes=[ct.c_char_p]

def create_sock():
    global server_sock
    server_sock=lib.create_sock(server_ip.encode())
    if server_sock==-1:
        print('[-]Error in creating socket sock')
        sys.exit(-1)

def server_workings():
    res=lib.server_workings(publish_path.encode())
    if res==-1:
        print('[-]Error in running relay server')
        sys.exit(-1)

def init_script():
    create_sock()
    server_workings()

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('server_ip_port', type=str, help='an ip and port pair to bind the relay server')
    parser.add_argument('librelay_path', type=str, help='a path to librelay.so path')
    parser.add_argument('publish_path', type=str, help='a path to publish the connected controllers')
    args=parser.parse_args()

    server_ip=args.server_ip_port
    lib_path=args.librelay_path
    publish_path=args.publish_path

    lib=ct.CDLL(lib_path)
    init_functions()
    init_script()

