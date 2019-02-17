from MySQLdb import connect
from importlib import import_module
from getpass import getpass
from socket import socket, AF_INET, SOCK_STREAM
from sys import exit

gdefs=import_module('global_defs', '/ryu/apps')

def init_db(host, uname, passwd, db_name):
    try:
        conn=connect(host, user=uname, passwd=passwd, db=db_name)
        print('[!]Successful connection to {}'.format(db_name))

        cur=conn.cursor()

        return (conn, cur)
    except Exception as e:
        print('[-]Error in connecting to db {}: {}'.format(db_name, e))
        exit(-1)

def get_self_ip():
    try:
        #build sock
        sock=socket(AF_INET, SOCK_STREAM)

        #connect
        sock.connect(('1.1.1.1', 80))

        #get name
        ret=sock.getsockname()[0]

        #close and ret
        sock.close()
        return ret
    except Exception as e:
        print('[-]Error in getting self subnet IP address: {}'.format(e))

def send_query(t, ip):
    try:
        query='SELECT macs from `{}`;'.format(ip)

        t[1].execute(query)
        rows=t[1].fetchall()

        macs=[]
        for r in rows:
            macs.append(r[0])

        t[0].close()
        return macs
    except Exception as e:
        print('[-]Error in executing query {}: {}'.format(query, e))

def init():
    #globals
    db_host=gdefs.db_host
    uname=('ctrlr' if gdefs.uname=='' else gdefs.uname)
    db_name=gdefs.db_name

    #get passwd
    passwd=getpass('Enter the password for {}: '.format(uname))

    #initiate db connection
    conn_net, cur_net=init_db(db_host, uname, passwd, db_name)

    #get self ip
    ip=get_self_ip()

    #get data from db
    macs=send_query((conn_net, cur_net), ip)

    #return macs
    return macs
