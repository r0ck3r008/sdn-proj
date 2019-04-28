from socket import socket, AF_INET, SOCK_STREAM, AF_UNIX
from os import mkfifo, system, popen
from argparse import ArgumentParser
from sys import stderr, exit
from threading import Thread as thread, Lock as lock
from multiprocessing import Process as process
from time import sleep

def sock_create(addr, flag):
    sock=None
    try:
        if flag==0: #ids server
            sock=socket(AF_INET, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
        elif flag==1:   #uds server
            sock=socket(AF_UNIX, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
        else:   #uds client
            sock=socket(AF_UNIX, SOCK_STREAM)
            sock.connect(addr)
        return sock
    except Exception as e:
        stderr.write('[-]Error in creating server socket for addr {}: {}, flag={}'.format(addr, e, flag))
        if sock!=None:
            sock.close()
        exit(-1)

def ids_cli_run(sock, addr):
    #connect to uds
    uds_sock=sock_create('./uds', 2)

    while True:
        cmdr=uds_sock.recv(512)
        if cmdr=='TRIGGER':
            sock.send('TRIGGER')

def svr_functions(ip):
    svr_sock=sock_create((ip, 6666), 0)

    while(1):
        sock=None
        try:
            sock, addr=svr_sock.accept()
            cli_thr=thread(target=ids_cli_run, args=[sock, addr])
            cli_thr.start()
        except Exception as e:
            stderr.write('[-]Error in accepting client: {}'.format(e))
            if sock!=None:
                sock.close()

def pipe_functions():
    try:
        mkfifo('./pipe')
    except Exception as e:
        stderr.write('[-]Error in creating pipe: {}'.format(e))

    #connect to uds
    uds_sock=sock_create('./uds', 2)

    #listen to pipe
    while True:
        cmdr=None
        with open('./pipe', 'r') as pipe:
            while True:
                cmd=pipe.read()
                if len(cmd)==0:
                    break
                else:
                    cmdr=cmd.strip()
        if cmdr=='trigger':
        #send to uds clients
            uds_sock.send(cmdr)

def uds_cli_run(sock, uds_clients, uds_mtx):
    while True:
        cmdr=sock.recv(512)
        if cmdr=='trigger':
            #broadcast
            with uds_mtx:
                for cli in uds_clients:
                    cli.send('TRIGGER')

def uds_functions():
    uds_svr_sock=sock_create('./uds', 1)
    uds_clients=[]
    uds_mtx=lock()

    while(1):
        sock=None
        try:
            sock, _=uds_svr_sock.accept()
            with uds_mtx:
                uds_clients.append(sock)
            uds_cli_thr=thread(target=uds_cli_run, args=[sock, uds_clients, uds_mtx])
            uds_cli_thr.start()
        except Exception as e:
            stderr.write('[-]Error in creating UDS client: {}'.format(e))
            if sock!=None:
                sock.close()

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-i', '--ip', required=True, metavar='', dest='ip', help='The ip of the default interface')
    argument=parser.parse_args()

    uds_proc=process(target=uds_functions)
    uds_proc.start()

    svr_proc=process(target=svr_functions, args=[argument.ip, ])
    svr_proc.start()

    pipe_proc=process(target=pipe_functions)
    pipe_proc.start()

    #join all
    svr_proc.join()
    pipe_proc.join()
    uds_proc.join()
