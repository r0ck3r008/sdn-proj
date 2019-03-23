from importlib import import_module
from multiprocessing import Lock as lock, Process as process
from sys import stderr

utils=import_module('utils', '.')
mtx=lock()

def uds_loop(uds_sock, pipe_name):
    i=0
    while True:
        sock=None
        try:
            sock, addr=uds_sock.accept()
            print('[!]New UDS connection!!')
            proc=process(target=uds_cli_run, args=[sock, pipe_name])
            pro.start()
            i+=1
        except Exception as e:
            stderr.write('[-]Error in accepting UDS client {}: {}'.format(i, e))
            if sock!=None:
                sock.close()

def uds_cli_run(sock, pipe_name):
    while True:
        cmdr=utils.rcv(sock, None)
        write_to_pipe(pipe_name, cmdr)

def write_to_pipe(pipe_name, cmds):
    with open(pipe_name, 'w') as p:
        mtx.acquire()
        p.write(msg)
        mtx.release()
