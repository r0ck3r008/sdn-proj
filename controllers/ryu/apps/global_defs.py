#absolute path of some important files
blacklist_path='/ryu/apps/blacklist'
relay_lib_path='/ryu/apps/librelay/librelay.so'

#ip addresses of the realy and to be bound
relay_addr='192.168.1.61'
bind_addr='127.0.0.1'

#main host and blacklist database
hosts={}
blacklist=[]

#all the sockets
udp_sock=0
tcp_sock=0
server_sock=0
sock=0

#c library import wrapper
relay_lib=None

#pipes for the multiprocesses
p1=None
p2=None
c1=None
c2=None

#bounded semaphores
reader=None
writer=None
