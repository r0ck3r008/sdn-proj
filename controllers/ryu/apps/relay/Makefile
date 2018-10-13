ALL_OBJS=udp_connector.o sock_create.o snd_rcv.o allocate.o
LINKER_FLAGS=-Wall -Werror -fPIC -shared -g -c

all: librelay.so clean_objs

librelay.so: ${ALL_OBJS}
	gcc -I${MAKE_PATH} -shared -o librelay.so ${ALL_OBJS}

udp_connector.o: udp_connector.c
	gcc -I${MAKE_PATH} ${LINKER_FLAGS} udp_connector.c

sock_create.o: sock_create.c
	gcc -I${MAKE_PATH} ${LINKER_FLAGS} sock_create.c

snd_rcv.o: snd_rcv.c
	gcc -I${MAKE_PATH} ${LINKER_FLAGS} snd_rcv.c

allocate.o: allocate.c
	gcc -I${MAKE_PATH} ${LINKER_FLAGS} allocate.c

clean_objs:
	rm -f ./*.o

clean:
	rm -f librelay.so
