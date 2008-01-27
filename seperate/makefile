CC = g++
LDFLAGS = -lncurses

OBJS = structure.o primitives.o control.o test.o generate.o atlas.o sanity.o fov.o memory.o arrow.o

maze : ${OBJS}
	${CC} ${CFLAGS} ${OBJS} ${LDFLAGS} -o maze

.cpp.o :
	${CC} ${CFLAGS} -c $<

${OBJS} : maze.h
