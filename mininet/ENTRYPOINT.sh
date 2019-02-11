#!/usr/bin/env bash

service openvswitch-switch start
ovs-vsctl set-manager ptcp:6640

cd /topos
tmux

service openvswitch-switch stop
