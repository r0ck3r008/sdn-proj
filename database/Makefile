all: mariadb_custom

mariadb_custom: Dockerfile entrypoint.sh
	docker build -t mariadb_custom .

clean:
	docker rmi mariadb_custom --force
