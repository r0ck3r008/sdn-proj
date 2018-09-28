#!/usr/bin/python
import ctypes as ct
import sys

def init_functions():
    #create sock
    lib.create_sock.restype=c_int
    lib.create_sock.argtypes=[c_char_p]

    #server_workings
    lib.server_workings.restype=c_int
    lib.server_workings.argtypes=[c_char_p]

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

if __name__=='__main__':
    if len(sys.argv)!=4:
        print('[!]Usage: ./main.py [ip_to_bind:port_to_bind] [library_path] [publish_path]')

    server_ip=sys.argv[1]
    lib_path=sys.argv[2]
    publish_path=sys.argv[3]

    lib=ct.CDLL(lib_path)
    init_functions()

