FROM base/archlinux

#nameserver
RUN echo 'nameserver 1.1.1.1' > /etc/resolv.conf

#update
RUN pacman --noconfirm -Syyu

#install
RUN pacman --noconfirm -S tmux vim git lsof iproute2 python2

#locale
RUN echo 'en_US.UTF-8 UTF-8' >> /etc/locale.gen; locale-gen

#link
RUN ln -s /usr/bin/python2 /usr/bin/python

#clone
RUN git clone http://github.com/noxrepo/pox; cd pox; git checkout eel

#copy entrypoint
COPY entrypoint.sh /entrypoint.sh
RUN chmod 755 /entrypoint.sh

#entrypoint
ENTRYPOINT ["/entrypoint.sh"]
