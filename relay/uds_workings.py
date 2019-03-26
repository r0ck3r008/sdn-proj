from threading import Thread as thread, Lock as lock
from importlib import import_module
from sys import stderr, exit
from time import sleep

utils=import_module('utils', '.')
clients={}
msgs=[]

def uds_svr_loop(uds_sock, ids_sock):
    #close ids sock copy
    ids_sock.close()
    mtx=(lock(), lock())
    bcast_thr=thread(target=bcast_func, args=[mtx])
    bcast_thr.start()
    i=0
    while True:
        sock=None
        try:
            sock, _=uds_sock.accept()
            flag_cmd=sock.recv(2048).decode()
            flag=(0 if flag_cmd=='DWNLNK' else 1)
            print('[!]Accepted new UDS connection with flag {}'.format(flag))
            if flag==0: #downlink
                dwnlnk_handler_thr=thread(target=dwnlnk_handler, args=[sock, mtx[1]])
                dwnlnk_handler_thr.start()
            else:
                with mtx[0]:
                    clients[i]=sock
            i+=1
        except Exception as e:
            stderr.write('[-]Error in accepting new uds connection for client {}: {}'.format(i, e))
            if sock!=None:
                sock.close()
                
def bcast_func(mtx):
    print('[!]Bcast thread started!!!')
    while True:
        with mtx[1]:
            if msgs:
                cmds=msgs.pop()
                with mtx[0]:
                    tags=clients.keys()
                    for tag in tags:
                        try:
                            clients[tag].send(cmds.encode())
                        except Exception as e:
                            stderr.write('[-]Error in sending via bcast function to tag {}: {}'.format(tag, e))
                print('[!]Broadcasted {}'.format(cmds))
        sleep(0.01)

def dwnlnk_handler(sock, mtx):
    print('[!]Dwnlnk handler in UDS started!!!')
    while True:
        try:
            cmdr=sock.recv(2048).decode()
            print('[!]Received {} from downlink.'.format(cmdr))
            with mtx:
                msgs.append('{}'.format(cmdr))
            print('[!]Received {} from uds client and appended to msgs!!'.format(cmdr))
        except Exception as e:
            stderr.write('[-]Error in downlink handler: {}'.format(e))
