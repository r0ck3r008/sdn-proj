from multiprocessing import Process as process
from threading import Thread as thread
from importlib import import_module
from sys import stderr, exit
from time import sleep

utils=import_module('utils', '.')

def ids_svr_loop(ids_sock, uds_sock, uds_sock_name):
    #close uds sock copy
    uds_sock.close()

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

def handle_ctrlr(sock, addr, ids_sock, uds_sock_name):
    print('[!]Handeling ctrlr at {}'.format(addr))
    #close duplicate sock
    ids_sock.close()

    #form threads
    dwnlnk_thr=thread(target=handle_dwnlnk, args=[sock, addr, uds_sock_name])
    dwnlnk_thr.start()

    uplnk_thr=thread(target=handle_uplnk, args=[addr, uds_sock_name])
    uplnk_thr.start()

    #join all
    dwnlnk_thr.join()
    uplnk_thr.join()

def handle_dwnlnk(sock, addr, uds_sock_name):
    print('[!]Handeling downlink for ctrlr at {}'.format(addr))
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

def handle_uplnk(addr, uds_sock_name):
    print('[!]Handeling uplink for ctrlr at {}'.format(addr))
    #connect back
    sock=utils.sock_create((addr[0], 12346), 1)

    #create uds link
    uds_sock=utils.sock_create(uds_sock_name, 3)

    #send first msg
    uds_sock.send('UPLNK'.encode())

    if uds_sock.getblocking()==True:
        print('[!]Yes the UDS is blocking with timeout {}'.format(uds_sock.gettimeout()))

    #loop
    while True:
        try:
            cmdr=uds_sock.recv(2048).decode()
            #send to ctrlr
            sock.send(cmdr.encode())
            print('[!]Reeceived {} from uds and sent to ctrlr at {}!'.format(cmdr, addr[0]))
        except Exception as e:
            stderr.write('[-]Error in uplnk handler of {} ctrlr: {}'.format(addr[0], e))
