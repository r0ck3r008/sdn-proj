FROM archlinux/base

#namespace
RUN echo 'namespace 1.1.1.1' > /etc/resolv.conf

#update
RUN pacman --noconfirm -Syyu

#install
RUN pacman --noconfirm -S tmux vim python iproute2

#copy entrypoint
COPY entrypoint.sh /entrypoint.sh
RUN chmod 755 /entrypoint.sh

#entrypont
ENTRYPOINT ["/entrypoint.sh"]
