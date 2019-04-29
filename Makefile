#check required executables
docker := `which docker`
mysql := `which mysql`
python3 := `which python3`
python2 := `which python2`
#path
export MAKE_PATH=${PWD}

all: docker mysql python3 python2 ryu_docker mininet_docker database

#depends
ifndef docker:
	$(error "Docker is unavailable, please install latest version of docker before making")
ifndef mysql:
	$(error "Mysql is unavailable, please install the latest version before making")
ifndef python3:
	$(error "Python3 is unavailable, please install the latest version before making")
ifndef python2:
	$(error "Python2 is unavailable, please install the latest version before making")
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
