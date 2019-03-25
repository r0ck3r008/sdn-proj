from socket import AF_INET, SOCK_STREAM, AF_UNIX, socket
from sys import stderr, exit

def sock_create(addr, flag):
    sock=None
    try:
        if flag==0:
            sock=socket(AF_INET, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
            print('[!]IDS server bound and listening on {}...'.format(addr))
        elif flag==1:
            sock=socket(AF_INET, SOCK_STREAM)
            sock.connect(addr)
            print('[!]IDS connected to {}...'.format(addr))
        elif flag==2:
            sock=socket(AF_UNIX, SOCK_STREAM)
            sock.bind(addr)
            sock.listen(5)
            print('[!]UDS server bound and listening successfully on {}...'.format(addr))
        elif flag==3:
            sock=socket(AF_UNIX, SOCK_STREAM)
            sock.connect(addr)
            print('[!]UDS connected to {}'.format(addr))
        return sock
    except Exception as e:
        stderr.write('[-]Error in creating sock for addr {}: {}'.format(addr, e))
        if sock!=None:
            sock.close()
        exit(-1)
