all: mininet_docker

mininet_docker: Dockerfile ENTRYPOINT.sh
	docker build -t mininet .
clean:
	docker rmi mininet --force
