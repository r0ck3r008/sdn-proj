export MAKE_PATH=${PWD}
export COMPILER=gcc

all: ryu_docker mininet_docker dbinterface.bin backend.bin

ryu_docker: ${MAKE_PATH}/controllers/ryu/Makefile
	make -C ${MAKE_PATH}/controllers/ryu 

mininet_docker: ${MAKE_PATH}/mininet/Makefile
	make -C ${MAKE_PATH}/mininet

dbinterface.bin: ${MAKE_PATH}/dbinterface.d/Makefile
	make -C ${MAKE_PATH}/dbinterface.d

backend.bin: ${MAKE_PATH}/controllers/ryu/apps/backend/Makefile
	make -C ${MAKE_PATH}/controllers/ryu/apps/backend
