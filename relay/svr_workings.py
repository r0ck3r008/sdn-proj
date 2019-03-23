from importlib import import_module
from sys import stderr
from threading import Thread as thread
from multiprocessing import Process as process

utils=import_module('utils', '.')

def svr_loop(svr_sock, uds_sock, pipe_name):
    i=0
    while True:
        sock=None
        try:
            sock, addr=svr_sock.accept()
            print('[!]Accepted connection from: {}'.format(addr))
            proc=Process(target=handle_client, args=[sock, addr, pipe_name])
            proc.start()
            i+=1
        except Exception as e:
            stderr.write('[-]Error in acepting {} connection: {}'.format(i, e))
            if sock!=None:
                sock.close()

def handle_client(sock, addr, pipe_name):
    #spawn threads
    uplnk_thr=thread(target=handle_uplnk, args=[pipe_name, addr])
    uplnk_thr.start()

    dwnlnk_thr=thread(target=handle_dwnlnk, args=[uds_sock, sock, addr])
    dwnlnk_thr.start()

    #join threads
    uplink_thr.join()
    dwnlnk_thr.join()

def handle_uplnk(pipe_name, addr):
    #connect back
    sock=utils.sock_create(addr, 1)

    #loop
    while True:
        with open(pipe_name, 'r') as p:
            utils.snd(sock, p.read(), addr)

def handle_dwnlnk(uds_sock, sock, addr):
    #form connection to UDS
    uds_cli=utils.sock_create(uds_sock, 3)

    while True:
        cmdr=utils.rcv(sock, addr)
        #send to UDS
        utils.snd(uds_cli, cmdr, None)


