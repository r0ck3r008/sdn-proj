ALL_OBJS= main.o server.o broadcast.o snd_rcv.o link.o allocate.o
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

snd_rcv.o: snd_rcv.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} snd_rcv.c

link.o: link.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} link.c

allocate.o: allocate.c
	gcc -I${MAKE_PATH} ${COMPILER_FLAGS} allocate.c

clean_objs: ${ALL_OBJS}
	rm -f ./*.o

clean:
	rm ./relay.bin
