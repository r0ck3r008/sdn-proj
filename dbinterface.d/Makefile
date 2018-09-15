ALL_OBJS= main.o server.o dbconnect.o allocate.o
LINK_FLAGS=-pthread `pkg-config --cflags mariadb --libs mariadb` -g -o

all: dbinterface.bin clean_objects

dbinterface.bin: ${ALL_OBJS}
	${COMPILER} -I${MAKE_PATH} ${ALL_OBJS} ${LINK_FLAGS} dbinterface.bin 

main.o: main.c
	${COMPILER} -I${MAKE_PATH} -c main.c

server.o: server.c
	${COMPILER} -I${MAKE_PATH} -c server.c

dbconnect.o: dbconnect.c
	${COMPILER} -I${MAKE_PATH} -c dbconnect.c	

allocate.o: allocate.c
	${COMPILER} -I${MAKE_PATH} -c allocate.c

clean_objects: 
	rm -f ./*.o

clean:
	rm -f ./dbinterface.bin
