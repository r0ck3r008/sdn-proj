ALL_OBJS= main.o server.o broadcast.o snd_rcv.o link.o allocate.o
LINKER_FLAGS= -g -pthread -lsodium -o

all: relay.bin clean_objs

relay.bin: ${ALL_OBJS}
	gcc -I${MAKE_PATH} ${ALL_OBJS} ${LINKER_FLAGS} relay.bin

main.o: main.c
	gcc -I${MAKE_PATH} -c main.c

server.o: server.c
	gcc -I${MAKE_PATH} -c server.c

broadcast.o: broadcast.c
	gcc -I${MAKE_PATH} -c broadcast.c

snd_rcv.o: snd_rcv.c
	gcc -I${MAKE_PATH} -c snd_rcv.c

link.o: link.c
	gcc -I${MAKE_PATH} -c link.c

allocate.o: allocate.c
	gcc -I${MAKE_PATH} -c allocate.c

clean_objs: ${ALL_OBJS}
	rm -f ./*.o

clean:
	rm ./relay.bin
