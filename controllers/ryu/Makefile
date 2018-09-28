all: ryu_docker librelay.so

ryu_docker: Dockerfile ENTRYPOINT.sh
	docker build -t ryu .

librelay.so: ${MAKE_PATH}/controllers/ryu/apps/relay/Makefile
	make -C ${MAKE_PATH}/controllers/ryu/apps/relay

clean:
	docker rmi ryu --force
