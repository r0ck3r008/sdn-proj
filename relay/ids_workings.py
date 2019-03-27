from multiprocessing import Process as process
from threading import Thread as thread
from importlib import import_module
from sys import stderr, exit
from time import sleep

utils=import_module('utils', '.')

def ids_svr_loop(ids_sock, uds_sock, uds_sock_name, suprelay_file):
    #close uds sock copy
    uds_sock.close()

    #handle super relays
    if suprelay_file!=None:
        suprelay_proc=process(target=init_suprelay, args=[suprelay_file, ids_sock, uds_sock_name]).start()

    i=0
    while True:
        sock=None
        try:
            sock, addr=ids_sock.accept()
            print('[!]Accepted client {}'.format(addr))
            proc=process(target=handle_ctrlr, args=[sock, addr, ids_sock, uds_sock_name])
            proc.start()
            #close copy with it
            sleep(0.1)
            sock.close()
            i+=1
        except Exception as e:
            stderr.write('[-]Error in accepting client number {}'.format(i))
            if sock!=None:
                sock.close()

def init_suprelay(suprelay_file, ids_sock, uds_sock_name):
    #self ip
    self_ip=ids_sock.getsockname()[0]
    ids_sock.close()

    #suprelay addresses
    suprelay_addrs=utils.parse_suprelay_file(suprelay_file)

    #server
    suprelay_svr_sock=utils.sock_create((self_ip, 12346), 0)

    #connect to each and expect connection back
    for addr in suprelay_addrs:
        uplnk_sock=None
        dwnlnk_sock=None
        try:
            uplnk_sock_sock=utils.sock_create(addr, 1)
            dwnlnk_lnk_sock, _=suprelay_svr_sock.accept()
            s_proc=process(target=handle_suprelay, args=[uplnk_sock, dwnlnk_sock, addr, suprelay_svr_sock, uds_sock_name]).start()
            uplnk_sock.close()
            dwnlnk_sock.close()
        except Exception as e:
            stderr.write('[-]Error in connecting to super relay at {}: {}'.format(addr, e))
            if uplnk_sock!=None:
                uplnk_sock.close()
            if dwnlnk_sock!=None:
                dwnlnk_sock.close()

def handle_suprelay(uplnk_sock, dwnlnk_sock, addr, suprelay_svr_sock, uds_sock_name):
    print('[!]Handelling suprelay at {}'.format(addr))
    #close server sock copy
    suprelay_svr_sock.close()

    #create threads
    dwnlnk_thr=thread(target=handle_dwnlnk, args=[dwnlnk_lnk_sock, addr, uds_sock_name])
    dwnlnk_thr.start()

    uplnk_thr=thread(target=handle_uplnk, args=[addr, uds_sock_name, uplnk_sock])
    uplnk_thr.start()

    #join all
    dwnlnk_thr.join()
    uplnk_thr.join()

def handle_ctrlr(sock, addr, ids_sock, uds_sock_name):
    print('[!]Handeling ctrlr at {}'.format(addr))
    #close duplicate sock
    ids_sock.close()

    #form threads
    dwnlnk_thr=thread(target=handle_dwnlnk, args=[sock, addr, uds_sock_name])
    dwnlnk_thr.start()

    uplnk_thr=thread(target=handle_uplnk, args=[addr, uds_sock_name, None])
    uplnk_thr.start()

    #join all
    dwnlnk_thr.join()
    uplnk_thr.join()

def handle_dwnlnk(sock, addr, uds_sock_name):
    print('[!]Handeling downlink for client at {}'.format(addr))
    #create uds link
    uds_sock=utils.sock_create(uds_sock_name, 3)

    #send first msg
    uds_sock.send('DWNLNK'.encode())

    #loop
    while True:
        try:
            cmdr=sock.recv(2048).decode()
            #send to uds
            uds_sock.send(cmdr.encode())
            print('[!]Received {} from ctrlr at {} and sent to uds!'.format(cmdr, addr))
        except Exception as e:
            stderr.write('[-]Error in dwnlnk handler of {}: {}'.format(addr, e))

def handle_uplnk(addr, uds_sock_name, sock=None):
    print('[!]Handeling uplink for client at {}'.format(addr))
    #connect back or not
    if sock==None:
        sock=utils.sock_create((addr[0], 12346), 1)

    #create uds link
    uds_sock=utils.sock_create(uds_sock_name, 3)

    #send first msg
    uds_sock.send('UPLNK'.encode())

    #loop
    while True:
        try:
            cmdr=uds_sock.recv(2048).decode()
            #send to ctrlr
            sock.send(cmdr.encode())
            print('[!]Reeceived {} from uds and sent to client at {}!'.format(cmdr, addr[0]))
        except Exception as e:
            stderr.write('[-]Error in uplnk handler of {} client: {}'.format(addr[0], e))
