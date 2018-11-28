FROM ubuntu

#nameserver
RUN echo 'nameserver 1.1.1.1' > /etc/resolv.conf
#update
RUN apt-get update

#install utils
RUN apt-get -y install git tmux python vim python-setuptools python-pip net-tools lsof tcpdump 

#pip installs
RUN pip install tinyrpc webob routes ovs oslo.config msgpack eventlet lxml paramiko sqlalchemy ncclient

#clone ryu
RUN git clone git://github.com/osrg/ryu.git 

#install
RUN cd ryu; python ./setup.py install

#expose ports
EXPOSE 6653

#copy entrypoint
COPY ENTRYPOINT.sh /

#change permissions
RUN chmod +x ENTRYPOINT.sh

#Enter
ENTRYPOINT ["/ENTRYPOINT.sh"]
