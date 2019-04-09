from MySQLdb import connect
from sys import exit, stderr

def init_db(db_host, uname, passwd, db_name):
    conn=None
    try:
        conn=connect(db_host, user=uname, passwd=passwd, db=db_name)
        print('[!]Connected under the uname: {}'.format(uname))

        cur=conn.cursor()

        return (conn, cur)
    except Exception as e:
        stderr.write('[-]Error in connecting under uname {}: {}'.format(uname, e))
        if conn!=None:
            conn.close()
        exit(-1)

def send_query(t, query):
    try:
        t[1].execute(query)
        t[0].commit()
        print('[!]Query executed Successfully')
    except Exception as e:
        stderr.write('[-]Error in executing query {}: {}'.format(query, e))


def update_db(t, topo):
    subnets=topo.values()
    ctrlr_ip=topo.keys()
    for i in range(len(ctrlr_ip)):
        subnet=subnets[i]
        ip=ctrlr_ip[i]
        #create table
        query="CREATE TABLE `{}` (macs varchar(50));".format(ip)
        send_query(t, query)
        #inset vals
        query="INSERT INTO `{}` VALUES ('{}'), ".format(ctrlr_ip[i], subnet[0][0].MAC('s{}-eth{}'.format(i+1, len(subnets[1])+1)))
        for h in subnet[1]:
            query="".join([query, "('{}') ".format(h.MAC())])
            if h==subnet[1][-1]:
                query="".join([query, ";"])
            else:
                query="".join([query, " , "])
        send_query(t, query)

