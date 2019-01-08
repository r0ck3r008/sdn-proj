#!/bin/sh

if [ -z "$1" ]; then
	echo 'Enter Password as first argument'
	exit -1
fi

docker run -e MYSQL_ROOT_PASSWORD="$1" -v ${PWD}/dumps:/dumps -v /etc/mysql/my.cnf:/etc/mysql/my.cnf:ro -it mariadb_custom
