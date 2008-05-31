#include "maze.h"

void randmap(int size, int iter1, int iter2, int iter3) {
	testinit(size);
	rspot = player.grid = player.rot = 0, player.mir = 1;
	f(i,size) f(j,8) mov[i][j] = -1, rotor[i][j] = 0, mirror[i][j] = false;
	f(i,size) ground[i] = (rand()%5)*(!(rand()%100)) + (rand()%3)*(rand()%2)*(rand()%2)*(rand()%2)*16;
	
	pos edges[8*size+8];
	
	f(i,8*size) {
		edges[i].grid = i/8;
		edges[i].mir = ((i%2)?1:-1);
		edges[i].rot = (i%8)-(i%2);
	}
	
	if (ground[0] >= 32) ground[0] -= 32;
	f(i,size) if (ground[i]/16 == 1 && rand()%5) ground[i] -= 16;
	
	f(i,iter1) {
		pos first = edges[rand()%(size*8)], second = edges[rand()%(8*size)];
		if (canglue(first, second)) glue(first, second);
	}
	
	f(i,iter2) {
		pos first = edges[rand()%(size*8)], second = edges[rand()%(size*8)];
		if (!first.move(0) && !first.move(2) && !first.move(1) && !second.move(4) && !second.move(6) && !second.move(5)) {
			first.patch(1, second), second.patch(5, first);
		}
	}
	
	f(i,iter3) {
		pos bye = edges[rand()%(size*8)];
		if (bye.grid != 0 && (!bye.move(6) || ((ground[bye.grid] == 16 ||  rand()%20 == 0) && bye.move(2))))
			bye.destroy();
	}
}

void randmap2(int iter) {
	int size = 2<<iter;
	testinit(size);
	rspot = player.grid = player.rot = 0, player.mir = 1;
	f(i,size) f(j,8) mov[i][j] = -1, rotor[i][j] = 0, mirror[i][j] = false;
	f(i,size) ground[i] = (rand()%5)*(!(rand()%100)) + (rand()%3)*(rand()%2)*(rand()%2)*(rand()%2)*16;
	
	pos edges[8*size+8];
	
	f(i,8*size) {
		edges[i].grid = i/8;
		edges[i].mir = ((i%2)?1:-1);
		edges[i].rot = (i%8)-(i%2);
	}
	
	if (ground[0] >= 32) ground[0] -= 32;
	f(i,size) if (ground[i]/16 == 1 && rand()%5) ground[i] -= 16;
	
	f(i,iter) {
		int csize = 1<<i;
		f(j,csize) {
			pos tmp(j, 0, 1);
			f(dir,8) {
				pos tmp2 = tmp;
				if (tmp2.move(dir)) {
					tmp.grid += csize, tmp2.grid += csize;
					tmp.patch(dir, tmp2);
					tmp.grid -= csize;
				}
			}
		}
		f(j,csize*8) {
			pos first = edges[rand()%(csize*8)], second = edges[rand()%(csize*8) + csize*8];
			if (canglue(first, second)) {
				glue(first, second);
				if (rand()%3) break;
			}
		}
	}
}
