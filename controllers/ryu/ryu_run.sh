#!/bin/sh

docker run -v ${PWD}/apps:/ryu/apps -v ${HOME}/git/sdn-proj/controllers/librelay:/ryu/apps/librelay -it ryu 
