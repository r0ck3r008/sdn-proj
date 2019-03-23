from argsparse import ArgumentParser
from importlib import import_module
from multiprocessing import Process as process
from os import mkfifo, path
from sys import exit, stderr
from errno import EEXIST

uds_workings=import_module('uds_workings', '.')
svr_workings=import_module('svr_workings', '.')
utils=import_module('utils', '.')

def parse_glbls(args):
    glbls={}
    glbls['bind_addr']=args.bind_addr
    glbls['bind_port']=args.bind_port
    glbls['uds_sock']='./uds' if args.uds_sock==None else args.uds_sock
    glbls['pipe_name']='./pipe' if args.pipe_name==None else args.pipe_name

    return glbls

if __name__=='__main__':
    #arguments
    parser=ArgumentParser()
    parser.add_argument('-b', '--bind_addr', required=True, metavar='', dest='bind_addr', help='The address to bind to')
    parser.add_argument('-p', '--bind_port', required=True, metavar='', dest='bind_port', type=int, help='The port to bind the realy on')
    parser.add_argument('-uS', '--uds_sock', metavar='', dest='uds_sock', help="The URI of the UDS server (Default is './uds')")
    parser.add_argument('-Pn', '--pipe_name', metavar='', dest='pipe_name', help="The pipe name (Default is './pipe')")
    args=parser.parse_args()

    #form globals
    glbls=parse_glbls(args)

    #form pipe
    try:
        mkfifo(glbls['pipe_name'])
    except OSError as e:
        stderr.write('[-]Error in opening pipe: {}'.format(e))
        if e.error==EEXIST:
            raise

    #form UDS socket
    uds_sock=utils.sock_create(glbls['uds_sock'], 2)

    #form server socket
    svr_sock=utils.sock_create((glbls['bind_addr'], glbls['bind_port']), 0)

    #UDS handler
    uds_proc=process(target=uds_workings.uds_loop, args=[uds_sock, glbls['pipe_name']])
    uds_proc.start()

    #svr handler
    svr_proc=process(target=svr_workings.svr_loop, args=[svr_sock, glbls['uds_sock'], glbls['pipe_name']])
    svr_proc.start()

    #wait for finishing
    uds_proc.join()
    svr_proc.join()

    #close sockets
    uds_sock.close()
    svr_sock.close()
