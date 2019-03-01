from argparse import ArgumentParser
#from subprocess import Popen as popen
from multiprocessing import Process as process
from os import mkfifo, path, system
from sys import path

def ctrlr_handler(cls):
    ctrlr_proc=system("/usr/local/bin/ryu-manager fwd.py".format(cls))

def intf_handler(pipe_name):
    #start interface loop
    while True:
        ip=str(raw_input('[>]'))
        if 'blacklist' in ip.lower() or 'whitelist' in ip.lower():
            with open(pipe_name, 'w') as pipe:
                pipe.write(ip)
        elif '?' in ip or 'help' in ip.lower():
            print('Commands:\n1. BLACKLIST [mac address]\n2. WHITELIST [mac address]\n3. ?/help')
        else:
            print('[-]Invalid command! Retry...')

if __name__=='__main__':
    parser=ArgumentParser()
    parser.add_argument('-r', '--relay_addr', required=True, metavar='', dest='relay_addr', help='The address of the relay server')
    parser.add_argument('-rP', '--relay_port', required=True, metavar='', dest='relay_port', type=int, help='The port of the relay server')
    parser.add_argument('-c', '--ctrlr_cls', metavar='', dest='ctrlr_class', help='The ryu class that needs execution (default is ./fwd.py)')
    parser.add_argument('-p', '--pipe_name', metavar='', dest='pipe_name', help='The name of the pipe to be used for subprocess communication (default is ./pipe)')
    args=parser.parse_args()

    path.append('/ryu/apps')
    #open fifo
    pipe_name=('./pipe' if args.pipe_name==None else args.pipe_name)
    try:
        mkfifo(pipe_name)
        print('[!]Pipe successfully opened!')
    except OSError as e:
        if e.errno==EEXIST:
            print('[!]The existance of pipe predates the program call')

    #break into controller and interface
    ctrlr_proc=process(target=ctrlr_handler, args=(('./fwd' if args.ctrlr_class==None else args.ctrlr_class),))
    ctrlr_proc.start()

#    intf_proc=process(target=intf_handler, args=(('./pipe' if args.pipe_name==None else args.pipe_name), ))
#    intf_proc.start()

    #wait for them to finish
#    ctrlr_proc.join()
#    intf_proc.join()
#    ctrlr_proc=ctrlr_handler(args.ctrlr_class)
    intf_handler(pipe_name)
#    ctrlr_proc.wait()
