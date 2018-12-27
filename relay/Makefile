ALL_OBJS= main.o server.o tcp_child.o broadcast.o lock_and_exec.o list.o sock_create.o snd_rcv.o allocate.o
LINKER_FLAGS= -g -pthread -lsodium -o
COMPILER_FLAGS=-g -c

all:relay.bin clean_objs

relay.bin: ${ALL_OBJS}
	gcc -I${MAKE_PATH} ${ALL_OBJS} ${LINKER_FLAGS} ./relay.bin

main.o: main.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} main.c

server.o: server.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} server.c

tcp_child.o: tcp_child.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} tcp_child.c

broadcast.o: broadcast.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} broadcast.c

lock_and_exec.o: lock_and_exec.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} server.c

list.o: list.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} list.c

sock_create.o: sock_create.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} sock_create.c

snd_rcv.o: snd_rcv.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} snd_rcv.c

allocate.o: allocate.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} allocate.c

clean_objs: ${ALL_OBJS}
	rm -f ./*.o

clean:
	rm ./relay.bin
