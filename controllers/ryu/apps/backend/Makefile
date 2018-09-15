ALL_OBJS= main.o dbconnect.o sock_create.o snd_rcv.o allocate.c
LINKER_FLAGS= -g -o

all: backend.bin clean_objects

backend.bin: ${ALL_OBJS}
	${COMPILER} -I${MAKE_PATH} ${ALL_OBJS} ${LINKER_FLAGS} backend.bin

main.o: main.c
	${COMPILER} -I${MAKE_PATH} -c main.c

dbconnect.o: dbconnect.c
	${COMPILER} -I${MAKE_PATH} -c dbconnect.c

sock_create.o: sock_create.c
	${COMPILER} -I${MAKE_PATH} -c sock_create.c

snd_rcv.o: snd_rcv.c
	${COMPILER} -I${MAKE_PATH} -c snd_rcv.c

allocate.o: allocate.c
	${COMPILER} -I${MAKE_PATH} -c allocate.c

clean_objects:
	rm -f ./*.o

clean:
	rm -f ./backend.bin
