export MAKE_PATH=${PWD}
export COMPILER=gcc

all: ryu_docker mininet_docker relay.bin

ryu_docker: ${MAKE_PATH}/controllers/ryu/Makefile
	make -C ${MAKE_PATH}/controllers/ryu 

mininet_docker: ${MAKE_PATH}/mininet/Makefile
	make -C ${MAKE_PATH}/mininet

relay.bin: ${MAKE_PATH}/relay/Makefile
	make -C ${MAKE_PATH}/relay
