#!/bin/sh

docker run -v ${PWD}/ext:/pox/ext -v ${HOME}/git/sdn-proj/controllers/librelay:/pox/ext/librelay -it pox
