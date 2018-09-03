#!/bin/sh

docker run -v /lib/modules:/lib/modules -v /topos:${PWD}/topos --privileged -it mininet
