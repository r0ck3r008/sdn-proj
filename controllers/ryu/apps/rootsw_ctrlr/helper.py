from socket import socket, AF_INET, SOCK_STREAM
from sys import exit, stderr
from time import sleep

def sock_create(addr, flag):
    sock=None
    try:
        sock=socket(AF_INET, SOCK_STREAM)
        if flag==0:
            sock.bind(addr)
            sock.listen(5)
            print('[!]Socket bound and listening successfully at {}'.format(addr))
        else:
            sock.connect(addr)
            print('[!]Successfully connected to {}'.format(addr))
        return sock
    except Exception as e:
        print('[-]Error in creating socket for {}: {}'.format(addr, e), file=stderr)
        if sock!=None:
            sock.close()
            exit(-1)

def snd(sock, msg, addr):
    try:
        sock.send(msg.encode())
        print('[!]Sending {} to {} successful!'.format(msg, addr[0]))
    except Exception as e:
        print('[-]Error in sending {} to {}: {}'.format(msg, addr[0], e))

def rcv(sock, addr):
    try:
        cmdr=sock.recv(2048)
        return cmdr
    except Exception as e:
        print('[-]Error in receving from {}: {}'.format(addr[0], e))

def cli_run(rel_addr, pipe_name):
    sleep(1)
    cli_sock=sock_create(rel_addr, 1)
    while True:
        cmdr=''
        with open(pipe_name, 'r') as pipe:
            cmdr=pipe.read()
        snd(cli_sock, cmdr, rel_addr)

def get_self_addr():
    sock=sock_create(('1.1.1.1', 80), 1)
    ret=sock.getsock_name()[0]
    sock.close()
    return ret

def svr_run(self_port, blacklist, mtx):
    #start the server
    svr_sock=sock_create((get_self_addr(), self_port), 0)
    
    #accept connection
    try:
        c_sock, addr=svr_sock.accept()
        print('[!]Accepted connection back from {}'.format(addr[0]))
    except Exception as e:
        print('[-]Error in accepting connectin back frok relay: {}'.format(e))

    while True:
        cmdr=rcv(c_sock, addr)
        #mutex and update blacklist
        if 'blacklist' in cmdr.lower():
            mtx.acquire()
            blacklist.append(cmdr.split(':')[1])
            mtx.release()
        elif 'whitelist' in cmdr.lower():
            mtx.acquire()
            blacklist.remove(cmdr.split(':')[1])
            mtx.release()
