from importlib import import_module
from sys import stderr

utils=import_module('utils', '.')
cfg=import_module('cfg', '.')

def dwnlnk_svr_loop(dwnlnk_svr_sock, db_host, uname, passwd, db_name):
    print('[!]Started downlink server loop')
    sock=None
    try:
        conn, cur=utils.init_db_cxn(db_host, uname, passwd, db_name)

        #create table
        utils.send_query((conn, cur), "CREATE TABLE `{}` (mac varchar(50));".format(self_ip))

        sock, addr=dwnlnk_svr_sock.accept()
        print('[!]Got connection back from {}'.format(addr))
        #get connection
        blacklist=[]

        while True:
            cmdr=utils.rcv(sock, addr)
            if len(cmdr)>30:
                continue

            print('[!]Received {} from relay'.format(cmdr))
            cmd, app=cmdr.split('=')
            if cmd=='BLACKLIST':
                #query
                if app not in blacklist:
                    utils.send_query((conn, cur), "INSERT INTO `{}` VALUES ('{}');".format(self_ip, app))
                    blacklist.append(app)
                    print('[!]Appended {} to blackhosts'.format(app))
            else:
                #query
                if app in blacklist:
                    utils.send_query((conn, cur), "DELETE FROM `{}` WHERE mac='{}';".format(self_ip, app))
                    blacklist.remove(app)
                print('[!]Removed {} from blackhosts'.format(app))
    except Exception as e:
        stderr.write('[-]Error in dwnlnk_svr_loop: {}'.format(e))
        dwnlnk_svr_sock.close()
        if sock!=None:
            sock.close()
        exit(-1)

def init_dwnlnk_svr(passwd):
    global self_ip
    self_ip=utils.get_self_ip()
    db_host=cfg.db_host
    db_name="ctrlrs"
    uname=('ctrlr' if cfg.uname==None else cfg.uname)

    dwnlnk_svr_sock=utils.sock_create((self_ip, 12346), 0)
    dwnlnk_svr_loop(dwnlnk_svr_sock, db_host, uname, passwd, db_name)
