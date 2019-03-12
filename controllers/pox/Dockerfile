FROM archlinux/base

#nameserver
RUN echo 'nameserver 1.1.1.1' > /etc/resolv.conf

#update
RUN pacman --noconfirm -Syyu

#install
RUN pacman --noconfirm -S tmux vim git python2 iproute2 lsof

#timezone
RUN ln -sf /usr/share/zoneinfo/Asia/Kolkata /etc/localtime

#locale-gen
RUN echo 'en_US.UTF-8 UTF-8' > /etc/locale.gen; locale-gen

#clone
RUN git clone http://github.com/noxrepo/pox -b eel

#PATH
RUN export PATH=/pox:$PATH

#entrypoint
COPY entrypoint.sh /entrypoint.sh

#chmod 755
RUN chmod 755 /entrypoint.sh

#entrypoint
ENTRYPOINT ["/entrypoint.sh"]
