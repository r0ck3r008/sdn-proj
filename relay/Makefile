ALL_OBJS= main.o server.o broadcast.o sock_create.o udp_child.o tcp_child.o snd_rcv.o list.o allocate.o alarm.o
LINKER_FLAGS= -g -pthread -lsodium -o
COMPILER_FLAGS=-g -c

all:relay.bin clean_objs

relay.bin: ${ALL_OBJS}
	gcc -I${MAKE_PATH} ${ALL_OBJS} ${LINKER_FLAGS} ./relay.bin

main.o: main.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} main.c

server.o: server.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} server.c

broadcast.o: broadcast.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} broadcast.c

sock_create.o: sock_create.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} sock_create.c

udp_child.o: udp_child.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} udp_child.c

tcp_child.o: tcp_child.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} tcp_child.c

snd_rcv.o: snd_rcv.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} snd_rcv.c

list.o: list.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} list.c

allocate.o: allocate.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} allocate.c

alarm.o: alarm.c
	gcc -I${MAKE_OPTS} ${COMPILER_FLAGS} alarm.c

clean_objs: ${ALL_OBJS}
	rm -f ./*.o

clean:
	rm ./relay.bin
