CC = g++
LDFLAGS = -lncurses

maze : structure.o primitives.o control.o test.o generate.o atlas.o sanity.o fov.o
	${CC} structure.o primitives.o control.o test.o generate.o atlas.o sanity.o fov.o ${LDFLAGS} -o maze

structure.o : structure.cpp maze.h
	${CC} -c structure.cpp

primitives.o : primitives.cpp maze.h
	${CC} -c primitives.cpp

control.o : control.cpp maze.h
	${CC} -c control.cpp

test.o : test.cpp maze.h
	${CC} -c test.cpp

generate.o : generate.cpp maze.h
	${CC} -c generate.cpp

atlas.o : atlas.cpp maze.h
	${CC} -c atlas.cpp

sanity.o : sanity.cpp maze.h
	${CC} -c sanity.cpp

fov.o : fov.cpp maze.h
	${CC} -c fov.cpp
