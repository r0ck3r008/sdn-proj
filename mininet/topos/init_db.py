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

        rows=t[0].fetchall()
        return rows
    except Exception as e:
        print('[-]Error in executing query {}: {}'.format(query, e))

def update_network(ctrlr, swi_host, cxn):
    queries=[]

    queries.append("CREATE TABLE {} (nodes varchar(50));".format(ctrlr.IP()))

    query="INSERT INTO {} VALUES (".format(ctrlr.IP())
    i=0
    for s_h in swi_host:
        if i==0:
            query2=query2.join("'{}'".format(s_h.MAC()))
            i=i+1
        else:
            query2=query2.join("'{}'".format(s_h.IP()))

    query2=query2.join(");")
    queries.append(query2)

    try:
        for query in queries:
            rows=send_query(query, cxn)
            print('[!]received from DB: {}'.format(rows))

    except Exception as e:
        print('[-]Error in updating {} table: {}'.format(ctrlr.IP(), e))

