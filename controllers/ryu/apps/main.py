import ctypes as ct
import global_defs as gl
import blacklist as bl

gt.relay_lib=ct.CDLL(gt.relay_lib_path)

def init_all_functions():
    #sock_create
    gt.relay_lib.sock_create.restype=ct.c_int
    gt.relay_lib.sock_create.argtypes=[ct.c_char_p, ct.c_int]

    #client_run
    gt.relay_lib.client_run.argtypes=[ct.c_int]


def sock_create():

    #udp sock
    gt.udp_sock=int(gt.relay_lib.sock_create(ct.c_char_p('UDP'.encode(), ct.c_int(0))))
    if gt.udp_sock==-1:
        print('Error in creating udp socket for sending msg to {}'.format(gt.relay_addr))

    #bcast_sock
    gt.bcast_sock=int(gt.relay_lib.sock_create(ct.c_char_p(gt.relay_addr.encode()), ct.c_int(0)))
    if gt.bcast_sock==-1:
        print('Error in connecting to the relay at {}'.format(gt.relay_addr))

    #server_sock
    gt.server_sock=int(gt.relay_lib.sock_create(gt.server_addr.encode(), ct.c_int(1)))
    if gt.server_sock==-1:
        print('Error in creating and binding sock at {}'.format(gt.server_addr))

def init_script():
    bl.read_blacklist_file()
    init_all_functions()
    sock_create()

