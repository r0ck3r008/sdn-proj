export MAKE_PATH=${PWD}

all: ryu_docker mininet_docker help

ryu_docker: ${MAKE_PATH}/controllers/ryu/Makefile
	make -C ${MAKE_PATH}/controllers/ryu 

mininet_docker: ${MAKE_PATH}/mininet/Makefile
	make -C ${MAKE_PATH}/mininet
