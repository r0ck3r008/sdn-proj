from socket import socket, AF_INET, SOCK_STREAM, AF_UNIX
from sys import exit, stderr
from multiprocessing import Process as process
from importlib import import_module
from os import path

db_workings=import_module('db_workings', '.')
cli_run=import_module('cli_run', '.')

def sock_create(addr, flag):
    sock=None
    try:
        if flag==0:
            sock=socket(AF_INET, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
            print('[!]Server socket bound and listening...')
        elif flag==1:
            sock=socket(AF_INET, SOCK_STREAM)
            sock.connect(addr)
            print('[!]Connection to {} success'.format(addr[0]))
        elif flag==2:
            sock=socket(AF_UNIX, SOCK_STREAM)
            if path.exists(addr):
                sock.connect(addr)
                print('[!]Connected to UDS as client..')
            else:
                sock.bind(addr)
                sock.listen(5)
                print('[!]UDS server listening for connections...')
        return sock
    except Exception as e:
        print('[-]Error in creating socket server: {}'.format(e), file=stderr)
        if sock!=None:
            sock.close()
            exit(-1)

def svr_loop(svr_sock, db_info, uds_addr, pipe_name):
    i=-1
    while True:
        i+=1
        sock=None
        try:
            sock, addr=svr_sock.accept()
            print('[!]Successful connection to {}.'.format(addr[0]))
            proc=process(target=cli_run.handle_ctrlr, args=(sock, addr, db_info, uds_addr, pipe_name))
            proc.start()
            print('[!]{} handle process started successfully'.format(addr[0]))
        except exception as e:
            print('[-]Error in handelling client {}'.format(i), file=stderr)
            if sock!=None:
                sock.close()

def uds_loop(uds_sock, uds_addr, pipe_name):
    i=0
    while True:
        sock=None
        try:
            sock, addr=uds_sock.accept()
            print('[!]New UDS connection!')
            proc=process(target=cli_run.handle_uds, args=(sock, pipe_name))
            proc.start()
            i+=1
        except Exception as e:
            print('[-]Error in starting UDS client handler {}: {}'.format(i, e))
            if sock!=None:
                sock.close()
