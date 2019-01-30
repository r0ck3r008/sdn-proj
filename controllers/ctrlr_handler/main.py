from getpass import getpass
from MySQLdb import connect
from argparse import ArgumentParser
import importlib as ilib
import sys

directory='/home/naman/git/sdn-proj/controllers/ctrlr_handler'
s_workings=ilib.import_module('server_workings', directory)

def init_db_cxn(host, uname, passwd, db_name):
    try:
        conn=connect(host, uname, passwd, db_name)
        print('[!]Connected to db {} under uname {}'.format(db_name, uname))

        cur=conn.cursor()

        return (conn, cur)
    except Exception as e:
        print('[-]Error in init_db_cxn for uname {} and db {}: {}'.format(uname, db_name, e))
        sys.exit(-1)

if __name__=='__main__':
    #arguments
    parser=ArgumentParser()
    parser.add_argument('-H', '--Host', required=True, dest='host', metavar='', help='The host of the database server')
    parser.add_argument('-u', '--Username', required=True, dest='uname', metavar='', help='The username under which to connect')
    parser.add_argument('-d', '--database', required=True, dest='db_name', metavar='', help='The database name to connect to')
    args=parser.parse_args()

    #get passwd
    passwd=getpass('Enter password of DB {} '.format(args.db_name))

    #init_cxn
    conn, cur=init_db_cxn(args.host, args.uname, passwd, args.db_name)

    #server workings
    s_workings.start(conn, cur)

