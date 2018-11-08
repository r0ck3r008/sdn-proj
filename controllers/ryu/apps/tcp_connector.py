import importlib as ilib

import multiprocessing as mt
import ctypes as ct
gt=ilib.import_module('global_defs', '/ryu/apps')

def bcast(conn):#send a query, or a reply from sock->sock
    print('Calling bcast in tcp_connector')
    while True:
        cmd=str(conn.recv())
        flag, cmds=cmd.split(':')
        ret=gt.relay_lib.send_query(ct.c_int(gt.sock), ct.c_int(flag), ct.c_char_p(cmds.encode()))

        if ret:
            print('Error in broadcasting query for {}'.format(cmds))

def server(conn):
    print('Calling server in main')
    while True:
        cmdr=gt.relay_lib.recv_bcast(ct.c_int(gt.bcast_sock))
        #this qualifies as reader
        gt.reader.aquire()
        if gt.readers==0:
            gt.writer.aquire()
        gt.reader_count+=1
        gt.reaader.release()

        if cmds in hosts:
            #lock here
            #send reply
            gt.p1.send('0:{}'.format(hosts[cmds]))

        gt.reader.aquire()

def tcp_handeller():
    print('Calling tcp_handeller in tcp_connector')
    gt.p1, gt.c1=mt.Pipe()
    gt.p2, gt.c2=mt.Pipe()

    bcast_process=mt.Process(target=bcast, args=[gt.c1,])
    bacst_process.start()

    server_process=mt.Process(target=server, args=[gt.c2,])
    server_process.start()


def get_connection_back():
    print('Calling get_connection_bak in tcp_connector')
    gt.sock=gt.relay_lib.get_connection_back(ct.c_int(gt.server_sock))
    if gt.sock==-1:
        print('Error in getting connection back from relay')

    tcp_handeller()
