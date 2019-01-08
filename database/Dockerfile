FROM ubuntu

#update
RUN apt-get update -y

#upgrade
RUN apt-get upgrade -y

#timezone setup
RUN apt-get install -y tzdata; ln -sf /usr/share/zoneinfo/Asia/Kolkata /etc/localtime

#install apps
RUN apt-get install tmux vim mariadb-server -y

#copy entrypoint
COPY entrypoint.sh /entrypoint.sh

#executable
RUN chmod 755 /entrypoint.sh

#entrypoint
ENTRYPOINT /entrypoint.sh
