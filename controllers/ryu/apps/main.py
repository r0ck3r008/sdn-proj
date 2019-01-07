from socket import socket, AF_INET, SOCK_STREAM
import importlib as ilib

gdef=ilib.import_module('global_defs', '/ryu/apps')

def sock_create(addr, flag):
    try:
        s=socket(AF_INET, SOCK_STREAM)
        print('[!]Socket created')

        if(flag):#this is a client
            s.connect(addr)
            print('[!]Successfully connected to {}:{}'.format(addr[0], addr[1]))
        else:#this is a server
            s.bind(addr)
            s.listen(5)
            print('[!]Successfully bound and listning on {}:{}'.format(addr[0], addr[1]))
    except Exception as e:
        print('[-]Error in sock_create: {}'.format(e))


def init_main():
    #connect to relay
    gdef.bcast_sock=sock_create(gdef.relay_addr, flag=1)

    #get a connection back from relay
    sock=sock_create(gdef.self_addr, flag=0)
    gdef.sock=sock.accept()
    print('[!]Got relay connection back')

