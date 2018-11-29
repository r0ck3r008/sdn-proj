#!/bin/sh
path='/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/bin/site_perl:/usr/bin/vendor_perl:/usr/bin/core_perl:/pox'

docker run -e PATH=$path -v ${PWD}/ext:/pox/ext -v ${HOME}/git/sdn-proj/controllers/librelay:/pox/ext/librelay -it pox
