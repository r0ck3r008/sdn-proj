from mininet.topo import Topo

class dcenterTopo(Topo):
    def build(self, n_racs, n_hosts):
        switches=self.init_switches(n_racs)
        hosts=self.init_hosts(n_racs, n_hosts)
        topo=self.link_up(switches, hosts)

    def init_switches(self, n_racs):
        switches=[]
        root=self.addSwitch('s1')
        switches.append(root)

        for i in range(1, n_racs+1):
            dpid=(i*16)+1
            switches.append(self.addSwitch('s1r{}'.format(i), dpid='{}'.format(dpid)))

        return switches

    def init_hosts(self, n_racs, n_hosts):
        hosts=[]
        for i in range(1, n_racs+1):
            hosts.append([])
            for j in range(1, n_hosts+1):
                hosts[i-1].append(self.addHost('h{}r{}'.format(j, i)))

        return hosts

    def link_up(self, switches, hosts):
        topo={}

        #extract root switch
        root_sw=switches[0]
        switches.remove(root_sw)

        #link up all hosts
        i=0
        for s in switches:
            topo[s]=[]
            for h in hosts[i]:
                self.addLink(s, h)
                topo[s].append(h)
            i+=1
            self.addLink(root_sw, s)

        return topo

topos={
        'dcenter':dcenterTopo
        }
