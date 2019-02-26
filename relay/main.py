from importlib import import_module
from getpass import getpass
from sys import exit
from os import mkfifo
from errno import EEXIST

db_workings=import_module('db_workings', '.')
cfg=import_module('cfg', '.')
svr_workings=import_module('svr_workings', '.')

def process_glbls():
    glbls={}
    glbls['bind_addr']=(cfg.bind_addr, (12345 if cfg.bind_port==None else cfg.bind_port))
    glbls['uds_addr']=('./uds' if cfg.uds_addr==None else cfg.uds_addr)
    glbls['pipe_name']=('./pipe' if cfg.pipe_name==None else cfg.pipe_name)
    glbls['db_host']=db_host
    glbls['uname']=('relay' if cfg.uname==None else cfg.uname)
    glbls['db_name']=('topology' if cfg.db_name==None else cfg.db_name)
    glbls['passwd']=getpass('[>]Enter password for username {}: '.format(uname))
    return glbls

if __name__=='__main__':
    #globals
    glbls=process_glbls()

    #open fifo
    try:
        mkfifo(glbls['pipe_name'])
    except OSError as e:
        print('[-]Error in opening pipe: {}'.format(e))
        if e.errno==EEXIST:
            raise 

    #uds sock
    uds_svr=svr_workings.sock_create(glbls['uds_addr'], 2)

    #uds_loop
    svr_workings.uds_loop(uds_svr, glbls['uds_addr'], glbls['pipe_name'])

    #server sock
    svr_sock=svr_workings.sock_create(glbls['bind_addr'], 0)

    #svr_loop
    svr_workings.svr_loop(svr_sock, (glbls['db_host'], glbls['uname'], glbls['passwd'], glbls['db_name']), glbls['uds_addr'], glbls['pipe_name'])
