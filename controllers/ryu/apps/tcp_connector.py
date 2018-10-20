import multiprocessing as mt
import ctypes as ct
import global_defs as gt

def bcast(conn):#send a query, or a reply from sock->sock
    while True:
        cmd=str(conn.recv())
        flag, cmds=cmd.split(':')
        ret=gt.relay_lib.send_query(ct.c_int(gt.sock), ct.c_int(flag), ct.c_char_p(cmds.encode()))

        if ret:
            print('Error in broadcasting query for {}'.format(cmds))

def server(conn):
    while True:
        cmdr=gt.relay_lib.recv_bcast(ct.c_int(gt.bcast_sock))
        #lock here
        if cmds in hosts:
            #do something with it, ie send reply
            pass

def tcp_handeller():
    gt.p1, gt.c1=mt.Pipe()
    gt.p2, gt.c2=mt.Pipe()

    bcast_process=mt.Process(target=bcast, args=[gt.c1,])
    bacst_process.start()

    server_process=mt.Process(target=server, args=[gt.c2,])
    server_process.start()


def tcp_connector():
    gt.sock=gt.relay_lib.get_connection_back(ct.c_int(gt.server_sock))
    if gt.sock==-1:
        print('Error in getting connection back from relay')

    tcp_handeller()
