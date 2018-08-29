#!/usr/bin/python

"""
This example create a 3-switch topology connected in a loop.
A host is connected to each switch.
"""

from mininet.topo import Topo

class triangleTopo( Topo ):
  "Create a custom network and add nodes to it."

  def __init__( self ):
    #setLogLevel( 'info' )
    # Initialize topology
    Topo.__init__(self)

    #info( '*** Adding hosts\n' )
    h1 = self.addHost( 'h1' )
    h2 = self.addHost( 'h2' )
    h3 = self.addHost( 'h3' )

    #info( '*** Adding switches\n' )
    nodeA = self.addSwitch('s1')
    nodeB = self.addSwitch('s2')
    nodeC = self.addSwitch('s3')

    #info( '*** Creating links\n' )
    self.addLink( nodeA, nodeB )
    self.addLink( nodeB, nodeC )
    self.addLink( nodeC, nodeA )
    self.addLink( h1, nodeA )
    self.addLink( h2, nodeB )
    self.addLink( h3, nodeC )

topos = {'mytopo': (lambda: triangleTopo() ) }
