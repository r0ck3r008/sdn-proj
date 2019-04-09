from importlib import import_module
from argparse import ArgumentParser

mn_utils=import_module('mn_utils', '.')

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-c', '--ctrlr_file', required=True, dest='ctrlr_file', metavar='', help='The file with IP addresses of all the controllers')
    parser.add_argument('-d', '--db_host', required=True, dest='db_host', metavar='', help='The database host name')
    parser.add_argument('-u', '--uname', dest='uname', metavar='', help="The username under which database connection is made (Default 'topology')")
    parser.add_argument('-s', '--subnets', required=True, type=int, metavar='', dest='subnets', help='The number of subnets in the network')
    parser.add_argument('-r', '--rel_sw', required=True, type=int, metavar='', dest='rel_sw', help='The number of relay switches')
    parser.add_argument('-H', '--hosts', required=True, type=int, metavar='', dest='hosts', help='The number of hosts per subnet')
    args=parser.parse_args()

    #start forming
    mn_utils.mn_utils(args)

