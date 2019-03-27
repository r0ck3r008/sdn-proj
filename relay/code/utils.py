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

def parse_suprelay_file(fname):
    addrs=[]
    try:
        with open(fname, 'r') as f:
            for s in f.readlines():
                ip, port=s.strip().split(':')
                addrs.append((ip, int(port, 10)))
        return addrs
    except Exception as e:
        stderr.write('[-]Error in parsing super-relay address file {}: {}'.format(fname, e))
        exit(-1)

