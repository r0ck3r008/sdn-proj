#!/bin/sh

docker run -v /lib/modules:/lib/modules -v ${PWD}/topos:/topos --privileged=true -it mininet
