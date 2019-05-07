#path
export MAKE_PATH=${PWD}

all: ryu_docker mininet_docker database

#targets
ryu_docker: ${MAKE_PATH}/controllers/ryu/Makefile
	make -C ${MAKE_PATH}/controllers/ryu
mininet_docker: ${MAKE_PATH}/mininet/Makefile
	make -C ${MAKE_PATH}/mininet
database: ${MAKE_PATH}/database/Makefile
	make -C ${MAKE_FILE}/database
clean:
	make -C ${MAKE_PATH}/controllers/ryu clean
	make -C ${MAKE_PATH}/mininet clean
