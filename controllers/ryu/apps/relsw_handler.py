from MySQLdb import connect
from getpass import getpass
from importlib import import_module

gdefs=import_module('global_defs', '/ryu/apps')

def init_db(db_host, uname, passwd, db_name):
    try:
        conn=connect(db_host, user=uname, passwd=passwd, db=db_name)
        print('[!]Successful connection by user {}'.format(uname))

        cur=conn.cursor()
        
        return (conn, cur)
    except Exception as e:
        print('[-]Error in connecting with username {}: {}'.format(uname, e))

def send_query(t):
    try:
        root=[]
        hosts=[]

        #get tables names
        tables=[]
        t[1].execute('SHOW TABLES')
        rows=t[1].fetchall()
        for r in rows:
            tables.append(r[0])
        
        #fetch data
        rows=None
        i=0
        for table in tables:
            t[1].execute("SELECT macs FROM `{}`;".format(table))
            rows=t[1].fetchall()
            ret=[]
            for r in rows:
                ret.append(r[0])

            root.append(ret[0])
            hosts.append([])
            for host in ret[1:]:
                hosts[i].append(host)
            i+=1

        t[0].close()
        print('[!!!]Fetched: ')
        for i in range(len(root)):
            print("######For root {}:".format(root[i]))
            for host in hosts[i]:
                print('host->{}'.format(h))

        return (root, hosts)
    except Exception as e:
        print('[-]Error in fetching data: {}'.format(e))
        t[0].close()
        
def init():
    #globals
    db_host=gdefs.db_host
    uname=('ctrlr' if gdefs.uname=='' else gdefs.uname)
    db_name=gdefs.db_name

    #get passwd
    passwd=getpas('Enter the password for username {}: '.format(uname))

    #init db
    (conn_net, cur_net)=init_db(db_host, uname, passwd, db_name)

    #fetch data
    root, hosts=send_query((conn_net, cur_net))

    return (root, hosts)
