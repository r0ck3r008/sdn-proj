import ctypes as ct
import global_defs as gl
import blacklist as bl
import tcp_connector as tcp_c

gt.relay_lib=ct.CDLL(gt.relay_lib_path)

def read_blacklist_file():
    
    with open(gt.blacklist_path, 'r') as f:
        gt.blacklist=f.read().strip().split('\n')


def init_all_functions():
    #sock_create
    gt.relay_lib.sock_create.restype=ct.c_int
    gt.relay_lib.sock_create.argtypes=[ct.c_char_p, ct.c_int]

    #udp_connector
    gt.relay_lib.udp_connector.restype=ct.c_int
    gt.relay_lib.udp_connector.argtypes=[ct.c_int, ct.c_char_p]

    #get_connection_back
    gt.relay_lib.get_connection_back.restype=ct.c_int
    gt.relay_lib.get_connection_back.argtypes=[ct.c_int]

    #send_query
    gt.relay_lib.send_query.restype=ct.c_int
    gt.relay_lib.send_query.argtypes=[ct.c_int, ct.c_char_p]

def connect_udp():
    argv=gt.relay_addr+'12345'
    if gt.relay_lib.udp_connector(ct.c_int(gt.udp_sock), ct.c_char_p(addr.encode()))==-1:
        print('[-]Error in sending udp rsvp to {}'.format(addr))

def sock_create():

    #udp sock
    gt.udp_sock=int(gt.relay_lib.sock_create(ct.c_char_p('UDP'.encode(), ct.c_int(0))))
    if gt.udp_sock==-1:
        print('Error in creating udp socket for sending msg to {}'.format(gt.relay_addr))
    connect_udp()

    #server_sock
    gt.server_sock=int(gt.relay_lib.sock_create(gt.server_addr.encode(), ct.c_int(1)))
    if gt.server_sock==-1:
        print('Error in creating and binding sock at {}'.format(gt.server_addr))

    #bcast_sock
    gt.bcast_sock=int(gt.relay_lib.sock_create(ct.c_char_p(gt.relay_addr.encode()), ct.c_int(0)))
    if gt.bcast_sock==-1:
        print('Error in connecting to the relay at {}'.format(gt.relay_addr))

def init_script():
    bl.read_blacklist_file()
    init_all_functions()
    sock_create()
    tcp_c.get_connection_back()

