FROM java

#update packages
RUN apt-get update

#install tmux
RUN apt-get install -y tmux

#copy karaf
COPY karaf-0.7.3.tar.gz /root/

#untar
RUN tar -xvf /root/karaf-0.7.3.tar.gz -C /root

#change /etc/profile
RUN echo 'export TERM=xterm-color' >> /etc/profile
RUN echo 'alias cl=clear' >> /etc/profile

