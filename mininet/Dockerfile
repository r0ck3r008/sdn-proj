FROM ubuntu

#update
RUN apt-get update

#install vim tmux
RUN apt-get -y install vim tmux

#install mininet and required networking tools
RUN apt-get -y install mininet iproute2 ethtool tcpdump net-tools iputils-ping lsof

#expose ports
EXPOSE 6633 6653 6640

#copy entrypoint scripy
COPY ENTRYPOINT.sh /

#make entrypoint script executable
RUN chmod 755 ENTRYPOINT.sh

#exec entrypoint
ENTRYPOINT ["/ENTRYPOINT.sh"]
