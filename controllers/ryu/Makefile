all: ryu_docker

ryu_docker: Dockerfile ENTRYPOINT.sh
	docker build -t ryu .

clean:
	docker rmi ryu --force
