FROM base/archlinux

#update packages
RUN pacman -Syy

#download some important packages packages
RUN pacman --noconfirm -S git sudo binutils fakeroot tmux

#add user
RUN useradd -m -s /bin/bash -G wheel user

#clone git repo of onos
RUN cd /home/user; git clone https://aur.archlinux.org/onos.git; chown -R user:user /home/user

#append /etc/sudoers
RUN chmod 640 /etc/sudoers; echo 'user ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers; chmod 440 /etc/sudoers

#copy .tar.gz
COPY onos-1.12.0.tar.gz /home/user/onos

#makepkg -s
RUN cd /home/user/onos; sudo -u user makepkg --noconfirm -s

#install pkg.tar.gz
RUN pacman --noconfirm -U /home/user/onos/onos-1.12.0-2-x86_64.pkg.tar.xz

#append in /etc/profile
RUN echo 'export JAVA_HOME=/usr/lib/jvm/java-8-openjdk/jre' >> /etc/profile
RUN echo 'export TERM=xterm-color' >> /etc/profile
RUN source /etc/profile

#mkdir /home/sdn
RUN mkdir /home/sdn

#clean /var/cache
RUN pacman --noconfirm -Sc
