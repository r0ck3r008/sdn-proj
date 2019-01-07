def form_mac_list(num, flag): #1 for hosts else 0
    mac_list=[]

    for i in range(num):
        nhex=hex(i)[2:]
        mac=['0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0']
        j=0
        k=0
        while j<len(num):
            if k not in [2, 5, 8, 11, 14]:
                mac[k]=num[j]
                j=j+1
            k=k+1
        mac=''.join(mac)
        if flag:
            mac=mac[::-1]
        mac_list.append(mac)

    return mac_list

def form_ip_list(num):
    ip_list=[]

    for i in range(hosts):
        ip='192.168.'
        j=i
        k=0
        while j>=255:#max hosts possible are 65025
            k=k+1
            j=j-255
        l=str(k)+'.'+str(j)
        ip=ip.join(l)
        ip_list.append(ip)

    return ip_list

def init_switches(net, switches):
    switch_list=[]
    mac_list=form_mac_list(switches, 0)

    for i in range(switches):
        name='s'+str(i)
        swi=new.addSwitch(name, listenPort=6634, mac=mac_list[i])
        switch_list.append(swi)

    return switch_list

def init_controllers(net, controllers):
    i=0
    ctrlr_list=[]
    for ctrlr in controllers:
        name='c'+str(i)
        c=net.addController(name, controller=RemoteController, ip=ctrlr, port=6633)
        ctrlr_list.append(c)

    i=i+1

    return ctrlr_list

def init_hosts(net, hosts):
    host_list=[]
    ip_list=form_ip_list(hosts)
    mac_list=form_mac_list(hosts, 1)

    for i in range(hosts):
        name='h'+str(i)
        h=net.addHost(name, mac=mac_list[i], ip=ip_list[i])
        host_list.append(h)

    return host_list
