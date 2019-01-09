from socket import AF_INET, SOCK_STREAM, socket
import importlib as ilib
from threading import Thread, Lock

directory='/home/naman/sdn-proj/controllers/ctrlr_handler'
gdef=ilib.import_module('global_defs', directory)

def sock_create(addr, flag):
    try:
        s=socket(AF_INET, SOCK_STREAM)
        print('[!]Socket created successfully for {}:{}'.format(addr[0], addr[1]))

        if flag==1:
            s.connect(addr)
            print('[!]Successfully connected to {}:{}'.format(addr[0], addr[1]))
        else:
            s.bind(addr)
            s.listen(5)
            print('[!]Socket on {}:{} bound and listning successfully'.format(addr[0], addr[1]))

        return s
    except Exception as e:
        print('[-]Error in initiating server/connection at {}:{} : {}'.format(addr[0], addr[1], e))


def get_self_ip():
    s=sock_create(('1.1.1.1', 80), 1)
    ret= s.getsockname()[0]
    s.close()

    return ret

def send_query(query):
    try:
        #lock it
        gdef.mutex.acquire()
        #exec and commit
        gdef.cur.execute(query)
        gdef.conn.commit()
        rows=gdef.cur.fetchall()
        #unlock
        gdef.mutex.release()
        print('[!]Query {} executed and commited successfully'.format(query))
        return rows
    except Exception as e:
        print('[-]Error in executing query {} sent by the client: {}'.format(query, e))
        if gdef.mutex.locked():
            gdef.mutex.release()

def cli_run(sock):
    try:
        cmdr=sock.recv(512)
        rows=send_query(cmds)
        sock.send('{}'.format(rows))
        print('[!]Sent {} back to client {}'.format(rows, sock.getsockname()))
        sock.close()
    except Exception as e:
        print('[-]Error in handelling client {}: {}'.format(sock.getsockname(), e))

def server_run():
    try:
        #initiate the mutex
        gdef.mutex=Lock()
        while True:
            #accept new cxn
            client_sock=gdef.sock.accept()
            print('[1]Accepted client at {}'.format(client_sock.getsockname()))
            #init process
            thr=Thread(target=cli_run, args=[client_sock,])
            thr.start()
            print('[!]Started client handler thread for {}'.format(client_sock.getsockname()))
    except Exception as e:
        print('[-]Error in accept function {}'.format(e))

def start():
    gdef.sock=sock_create((get_self_ip(), 12346), 0)
    server_run()
