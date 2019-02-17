#!/bin/sh

service mysql start
#necessary shanigans in lieu of mysql_secure_installation
#new root passwd
mysqladmin password `echo $MYSQL_ROOT_PASSWORD`
#disable remote root login
mysql -u root --password=`echo $MYSQL_ROOT_PASSWORD` --execute="DELETE FROM mysql.user WHERE User='root' AND Host NOT IN ('localhost', '127.0.0.1', '::1');"
#flush privilages
mysql -u root --password=`echo $MYSQL_ROOT_PASSWORD` --execute="FLUSH PRIVILEGES"

#create databases
mysql -u root --password=`echo $MYSQL_ROOT_PASSWORD` --execute="CREATE DATABASE network"

#copy databases
mysql -u root --password=`echo $MYSQL_ROOT_PASSWORD` mysql < /dumps/mysql.sql
mysql -u root --password=`echo $MYSQL_ROOT_PASSWORD` --execute="FLUSH PRIVILEGES"

tmux

service mysql stop
