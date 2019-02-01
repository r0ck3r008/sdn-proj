from MySQLdb import connect

def init_db(host, uname, passwd, db_name):
    try:
        conn=connect(host, uname, passwd, db_name)
        cur=conn.cursor()
        print('[!]Db connection success')

        return (conn, cur)
    except Exception as e:
        print('[-]Error in conncetion {}'.format(e))

def send_query(query, t):
    try:
        t[1].execute(query)
        
        if 'update' in query.lower() or 'insert' in query.lower():
            t[0].commit()

        print('[1]Query executed successfully')

        rows=t[1].fetchall()
        ret=[]
        for i in rows:
            ret.append(i[0])

        return ret
    except Exception as e:
        print('[-]Error in executing query {}: {}'.format(query, e))

def update_network(ctrlr, swi_host, cxn):
    print('[!]Ctrlr_ip: {}'.format(ctrlr.IP()))
    for s_h in  swi_host:
        if s_h==swi_host[0]:
            print('[!]Switch mac is: {}'.format(s_h.MAC()))
        else:
            print('[!]Host {} IP is: {}'.format(swi_host.index(s_h), s_h.IP()))
