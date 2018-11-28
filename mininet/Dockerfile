FROM ubuntu

RUN echo 'nameserver 1.1.1.1' > /etc/resolv.conf

#update
RUN apt-get update

#install mininet and required networking tools
RUN apt-get -y install mininet iproute2 ethtool tcpdump net-tools iputils-ping lsof vim tmux

#expose ports
EXPOSE 6633 6653 6640

#copy entrypoint scripy
COPY ENTRYPOINT.sh /

#make entrypoint script executable
RUN chmod 755 ENTRYPOINT.sh

#exec entrypoint
ENTRYPOINT ["/ENTRYPOINT.sh"]
