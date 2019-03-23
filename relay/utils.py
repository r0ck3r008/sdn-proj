from socket import socket, AF_INET, SOCK_STREAM, AF_UNIX
from sys import exit, stderr
from os import path

def sock_create(addr, flag):
    sock=None
    try:
        if flag==0: #normal server
            sock=socket(AF_INET, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
            print('[!]Successfully listening on {}...'.format(addr))
        elif flag==1: #normal client
            sock=socket(AF_INET, SOCK_STREAM)
            sock.connect(addr)
            print('[!]Connected to {}...'.format(addr))
        elif flag==2: #UDS server
            if path.exists(addr):
                stderr.write('[-]Error in creating UDS socket, path not vacant!')
                exit(-1)
            sock=socket(AF_UNIX, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
            print('[!]UDS socket bound and listening successfully at {}...'.format(addr))
        elif flag==3: #UDS client
            sock=socket(AF_UNIX, SOCK_STREAM)
            sock.connect(addr)
            print('[!]UD client successfully connected to socket at {}'.format(addr))
        return sock
    except Exception as e:
        stderr.write('[-]Error in creating socket at {}: {}'.format(addr, e))
        if sock!=None:
            sock.close()
        exit(-1)

def snd(sock, cmds, addr):
    try:
        sock.send(cmds.encode())
    except Exception as e:
        stderr.write('[-]Error in sending {} to {}: {}'.format(cmds, ('UDS client' if addr==None else addr), e))

def rcv(sock, addr):
    try:
        cmdr=sock.recv(2048).decode()
    except Exception as e:
        stderr.write('[-]Error in receving from {}: {}'.format(('UDS client' if addr==None else addr), e))
