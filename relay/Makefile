ALL_OBJS= server.o broadcast.o snd_rcv.o link.o allocate.o
LINKER_FLAGS= -shared -g -pthread -lsodium -o
LIB_FLAGS= -fpic -c

all: librelay.so main clean_objs

main: main.py
	chmod 755 main.py

librelay.so: ${ALL_OBJS}
	gcc -I${MAKE_PATH} ${ALL_OBJS} ${LINKER_FLAGS} librelay.so

server.o: server.c
	gcc -I${MAKE_PATH} ${LIB_FLAGS} server.c

broadcast.o: broadcast.c
	gcc -I${MAKE_PATH} ${LIB_FLAGS} broadcast.c

snd_rcv.o: snd_rcv.c
	gcc -I${MAKE_PATH} ${LIB_FLAGS} snd_rcv.c

link.o: link.c
	gcc -I${MAKE_PATH} ${LIB_FLAGS} link.c

allocate.o: allocate.c
	gcc -I${MAKE_PATH} ${LIB_FLAGS} allocate.c

clean_objs: ${ALL_OBJS}
	rm -f ./*.o

clean:
	rm ./librelay.so
	chmod 644 main.py
