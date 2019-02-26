from MySQLdb import connect
from sys import exit, stderr

def init_cxn(db_host, uname, passwd, db_name):
    conn=None
    try:
        conn=connect(db_host, user=uname, passwd=passwd, db=db_name)
        cur=conn.get_cursor()
        return conn, cur
    except Exception as e:
        print('[-]Error in getting connection under the name {}: {}\nExiting....'.format(uname, e), file=stderr)
        if conn!=None:
            conn.close()
        exit(-1)

def send_query(t, query):
    try:
        t[1].execute(query)

        if 'update' in query.lower() or 'insert' in query.lower() or 'delete' in query.lower():
            t[0].commit()

        print('[!]Query executed successfully')

        rows=t[1].fetchall()
        ret=[]
        for r in rows:
            ret.append(r[0])

        return ret
    except Exception as e:
        print('[-]Error in executing {}: {}'.format(query, e), file=stderr)
