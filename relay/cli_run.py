from sys import exit, stderr
from importlib import import_module
from threading import Thread as thread

db_workings=import_module('db_workings', '.')
svr_workings=import_module('svr_workings', '.')

def snd(sock, msg, addr):
    try:
        sock.send(msg.encode())
    except Exception as e:
        print('[-]Error in sending {} to {}: {}'.format(msg, addr[0], e), file=stderr)

def rcv(sock, addr):
    try:
        msg=sock.recv(2048).decode()
        return msg
    except Exception as e:
        print('[!]Error in receving from {}: {}'.format((addr[0] if addr!=None else 'UDS client'), e), file=stderr)

def handle_ctrlr_p1(c_sock, addr, db_info, uds_addr):
    #get db connection
    conn, cur=db_workings.init_cxn(db_info[0], db_info[1], db_info[2], db_info[3])

    #send initial host list
    tables=db_workings.send_query((conn, cur), 'SELECT macs FROM `{}`'.format(addr[0]))
    msg=''
    for t in tables:
        msg="".join([msg, t])
    snd(c_sock, msg, addr)

    #create UDS client socket
    uds_sock=svr_workings.sock_create(uds_addr, 2)

    while True:
        cmdr=rcv(c_sock, addr)
        if 'exit' in cmd.lower():
            break
        snd(uds_sock, cmdr)

        #TODO
        #update db        

    #end stuff
    print('[!]Closing connection to {}'.format(addr[0]))
    conn.close()
    cur.close()
    c_sock.close()
    uds_close()

def handle_ctrlr_p2(addr, pipe_name):
    #get a connection back to controller
    sock=svr_workings.sock_create(addr, 1)

    #open the fifo
    while True:
        with open(pipe_name, 'r') as pipe:
        #listen for fifo updates and keep updating the controller
            cmdr=pipe.read()
            snd(sock, cmdr)

    print('[!]Closing downlink connection with {}'.format(addr[0]))
    sock.close()

#There are two threads for each client
#the first process handles the inbound blacklist and whitelist requests from the controller and sends the same to the UDS server within relay and updates in the db
#the second process listens for the intercontroller blacklist and whitelist requests sent by the UDS server on the fifo 

def handle_ctrlr(c_sock, addr, db_info, uds_addr, pipe_name):
    print('[!]Handelling client {}'.format(addr[0]))

    #create first thread
    thr_p1=thread(target=handle_ctrlr_p1, args=(c_sock, addr, db_info))
    thr_p1.start()

    #create second thread
    thr_p2=thread(target=handle_ctrlr_p2, args=((addr[0], 12346), pipe_name))
    thr_p2.start()

    #join them
    thr_p1.join()
    thr_p2.join()

def handle_uds(c_sock, pipe_name):
    while True:
        cmdr=rcv(c_sock, None)
        with open(pipe_name, 'w') as pipe:
            pipe.write(cmdr)
