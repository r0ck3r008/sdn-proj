from libnacl import randombytes_uniform as ru
from urllib import urlretrieve

def send_cmds(sel_hosts):
    bad_s_addr=sel_hosts[0].IP()
    for i in range(len(sel_hosts)):
        host=sel_hosts[i]
        if i==0:
            #bad_s
            print('[!]Starting http server on host {}'.format(host.name))
            host.cmdPrint('python -m SimpleHTTPServer &')
            host.cmdPrint('python utils/master.py -i {} &'.format(host.IP()))
        else:
            #zombies
            host.cmdPrint('python utils/zombie.py -a {} -n {} &'.format(bad_s_addr, host.name))

def choice(t):
    ret=t[ru(len(t))]
    t.remove(ret)
    return ret

def select_hosts(hosts):
    sel_hosts=[]

    #select bad server
    bad_s=choice(hosts)
    sel_hosts.append(bad_s)

    #select zombies
    ten_pcent=len(hosts)/10
    n_zom=ten_pcent if ten_pcent!=0 else 1
    for i in range(n_zom):
        sel_hosts.append(choice(hosts))

    return sel_hosts

def form_hosts(topo):
    subnets=topo.values()
    hosts=[]

    for subnet in subnets:
        for host in subnet[1]:
            hosts.append(host)

    return hosts

def init_viral_works(topo):
    hosts=form_hosts(topo)

    sel_hosts=select_hosts(hosts)

    send_cmds(sel_hosts)
