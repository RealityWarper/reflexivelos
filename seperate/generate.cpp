#include "maze.h"

void randmap(int size, int iter) {
	testinit(size);
	rspot = player.grid = player.rot = 0, player.mir = 1;
	f(i,size) f(j,8) mov[i][j] = -1, rotor[i][j] = 0, mirror[i][j] = false;
	f(i,size) ground[i] = (rand()%5)*(rand()%2)*(rand()%2)*(rand()%2) + (rand()%3)*(rand()%2)*(rand()%2)*(rand()%2)*16;
	
	pos edges[8*size+8];
	
	f(i,8*size) {
		edges[i].grid = i/8;
		edges[i].mir = ((i%2)?1:-1);
		edges[i].rot = (i%8)-(i%2);
	}
	
	if (ground[0] >= 32) ground[0] -= 32;
	f(i,size) if (ground[i]/16 == 1 && rand()%5) ground[i] -= 16;
	
	f(i,iter) {
		pos first = edges[rand()%(size*8)], second = edges[rand()%(8*size)];
		if (canglue(first, second)) glue(first, second);
	}
	
	f(i,iter/2) {
		pos first = edges[rand()%(size*8)], second = edges[rand()%(size*8)];
		if (!first.move(0) && !first.move(2) && !first.move(1) && !second.move(4) && !second.move(6) && !second.move(5)) {
			first.patch(1, second), second.patch(5, first);
		}
	}
}
