from MySQLdb import connect
from socket import socket, AF_INET, SOCK_STREAM
from sys import exit, stderr

def init_db_cxn(db_host, uname, passwd, db_name):
    conn=None
    try:
        conn=connect(host=db_host, user=uname, passwd=passwd, db=db_name)
        print('[!]Successfully connected to database {} under username {}'.format(db_name, uname))

        cur=conn.cursor()

        return (conn, cur)
    except Exception as e:
        stderr.write('[-]Error in connecting to db under uname {}: {}'.format(uname, e))
        if conn!=None:
            conn.close()
        exit(-1)

def send_query(t, query):
    try:
        t[1].execute(query)
        print('[!]Executed query {}'.format(query))
        if 'insert' in query.lower() or 'delete' in query.lower() or 'create' in query.lower():
            t[0].commit()
            return

        rows=t[1].fetchall()

        ret=[]
        for r in rows:
            ret.append(r[0])

        return ret
    except Exception as e:
        stderr.write('[-]Error in executing {}: {}'.format(query, e))
        t[0].close()
        exit(-1)

def sock_create(addr, flag):
    sock=None
    try:
        sock=socket(AF_INET, SOCK_STREAM)
        if flag==0:
            sock.bind(addr)
            sock.listen(5)
            print('[!]Socket created successfully and bound to {}...'.format(addr))
        elif flag==1:
            sock.connect(addr)
            print('[!]Socket successfully connected to {}'.format(addr))
        return sock
    except Exception as e:
        stderr.write('[-]Error in creating socket at {}: {}'.format(addr, e))
        if sock!=None:
            sock.close()
        exit(-1)

def get_self_ip():
    sock=sock_create(('1.1.1.1', 80), 1)
    ret=sock.getsockname()[0]
    sock.close()
    return ret

def snd(sock, cmds, addr):
    try:
        sock.send(cmds)
    except Exception as e:
        stderr.write('[-]Error in sending {} to {}: {}'.format(cmds, addr, e))

def rcv(sock, addr):
    try:
        cmdr=sock.recv(2048)
        return cmdr
    except Exception as e:
        stderr.write('[-]Error in receving form {}: {}'.format(addr, e))
