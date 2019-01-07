from MySQLdb import connect

def init_db(host, uname, passwd, db_name):
    try:
        conn=connect(host, uname, passwd, db_name)
        cur=conn.cursor()
        print('[!]Got cursor for connection to {} under uname {}'.format(db_name, uname))

    except Exception as e:
        print('[-]Error in connecting to {} with uname {}: {}'.format(db_name, uname, e))

    return (conn, cur)

def send_query(t, query):
    try:
        t[1].execute(query)

        if 'update' in strtolower(query) or 'insert' in strtolower(query):
            t[0].commit()

        print('[!]Successfully executed query {}'.format(query))
    except Exception as e:
        print('[-]Error in executing query {}: e'.format(query, e))

def fetch_controllers(host, uname, passwd):
    #init connection to controller db
    conn, cur=init_db(host, uname, passwd, 'controllers')
    
    #form query
    query='SELECT ip FROM controllers'

    #exec query
    rows=send_query((conn, cur), query)

    controllers=[]
    for eachRow in rows:
        controllers.append(eachRow[0])

    return controllers

def update_controller_db(hosts):
    pass

