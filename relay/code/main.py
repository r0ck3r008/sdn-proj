from argparse import ArgumentParser
from importlib import import_module
from multiprocessing import Process as process

utils=import_module('utils', '.')
ids_workings=import_module('ids_workings', '.')
uds_workings=import_module('uds_workings', '.')

def init_glbls(args):
    glbls={}
    glbls['bind_addr']=args.bind_addr
    glbls['bind_port']=args.bind_port
    glbls['uds_sock_name']='./sock' if args.uds_sock_name==None else args.uds_sock_name
    glbls['suprelay_file']=args.suprelay_file

    return glbls

if __name__=='__main__':
    #parse arguments
    parser=ArgumentParser()
    parser.add_argument('-b', '--bind_addr', required=True, metavar='', dest='bind_addr', help='The address to bind for relay')
    parser.add_argument('-p', '--bind_port', required=True, metavar='', type=int, dest='bind_port', help='The port to bind for relay')
    parser.add_argument('-uS', '--uds_sock_name', metavar='', dest='uds_sock_name', help="The URI of the UDS socket (Default is './uds')")
    parser.add_argument('-sR', '--suprelay_file', metavar='', dest='suprelay_file', help="Path of a file containing the address of super-relay(s)")
    args=parser.parse_args()
    glbls=init_glbls(args)

    #form sockets
    uds_sock=utils.sock_create(glbls['uds_sock_name'], 2)
    ids_sock=utils.sock_create((glbls['bind_addr'], glbls['bind_port']), 0)

    #create processes
    uds_svr_proc=process(target=uds_workings.uds_svr_loop, args=[uds_sock, ids_sock])
    uds_svr_proc.start()

    ids_svr_proc=process(target=ids_workings.ids_svr_loop, args=[ids_sock, uds_sock, glbls['uds_sock_name'], glbls['suprelay_file']])
    ids_svr_proc.start()

    #join
    uds_svr_proc.join()
    ids_svr_proc.join()

    #close sockets
    uds_sock.close()
    ids_sock.close()
