#check required executables
docker := `which docker`
#path
export MAKE_PATH=${PWD}
export COMPILER=gcc

#targets
all: docker ryu_docker mininet_docker mariadb_custom

ifndef docker:
	$(error "Docker is unavailable, please install latest version of docker before making")

ryu_docker: ${MAKE_PATH}/controllers/ryu/Makefile
	make -C ${MAKE_PATH}/controllers/ryu

mininet_docker: ${MAKE_PATH}/mininet/Makefile
	make -C ${MAKE_PATH}/mininet

mariadb_custom: ${MAKE_PATH}/database/Makefile
	make -C ${MAKE_PATH}/database
