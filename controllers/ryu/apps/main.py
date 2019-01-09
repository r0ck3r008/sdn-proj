from socket import socket, AF_INET, SOCK_STREAM
import importlib as ilib

gdef=ilib.import_module('global_defs', '/ryu/apps')

def sock_create(addr, flag):
    try:
        s=socket(AF_INET, SOCK_STREAM)
        print('[!]Socket created')

        if flag==1:#this is a client or a db_interface or to get self ip
            s.connect(addr)
            print('[!]Successfully connected to {}:{}'.format(addr[0], addr[1]))
        elif flag==0:#this is a server
            s.bind(addr)
            s.listen(5)
            print('[!]Successfully bound and listning on {}:{}'.format(addr[0], addr[1]))

        return s
    except Exception as e:
        print('[-]Error in sock_create: {}'.format(e))


def get_self_addr():
    #connect(fake)
    s=sock_create(('1.1.1.1', 80))
    #get name and disconnect
    ret=s.getsockname()[0]
    s.close()

    return ret

def update_db(self_ip):
    #connect
    db_sock=sock_create(gdef.db_interface_addr)
    
    query=("INSERT INTO controllers VALUES ('{}')".format(self_ip))
    db_sock.send(query.encode())

    print('[!]Received from db_interface: {}'.format(db_sock.recv().decode()))

    db_sock.close()

def init_main():
    self_ip=get_self_ip()
    #database handller
    update_db(self_ip)
        
    #connect to relay
#    gdef.bcast_sock=sock_create((self_ip, 6666), flag=1)

    #get a connection back from relay
#    sock=sock_create(gdef.self_addr, flag=0)
#    gdef.sock=sock.accept()
#    print('[!]Got relay connection back')

