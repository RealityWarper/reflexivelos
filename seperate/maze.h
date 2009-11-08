#ifndef MAZE_H
#define MAZE_H

#include <ncurses.h>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <time.h>
#include <string>
#include <cmath>
#include <list>

#define f(x,y) for (int x = 0; x < y; ++x)
#define ROW 1000
#define COL 1000
#define PL 100000
#define MONS 1000
#define COLORS 5

using namespace std;

extern int mov[PL][8], rotor[PL][8], pl;	// rotor is for cycles which leave you rotated, pl is number of places
extern bool mirror[PL][8];	// does the connection reverse chirality?

extern int ground[PL];	// colors, doors, trees

extern int trans[9], adx[8], ady[8];

extern int row, col, centerx, centery, rocks, rspot, colors[MONS], mons;

extern char letters[MONS];

extern int occupant[PL];
extern bool seen[PL];

extern bool drawbuff[100][100];

struct pos {
	int grid, rot, mir;
	
	pos(int g1 = 0, int r1 = 0, int m1 = 1);
	
	void rotate(int r2);
	
	void reflect();
	
	bool move(int dir);
	
	// reverse-engineers move - makes p2 the result of move(dir)
	void patch(int dir, pos p2);
	
	int follow_wall();
	
	void destroy();
};

extern pos player;

pos follow_wall(pos p);

void glue(pos p1, pos p2);

bool inview(int cx, int cy, int dis);

bool opaque(int g);

bool blocked(int g);

void draw(int g, int cx, int cy, bool exists);

void initialize();

void testinit(int size);

bool canglue(pos p1, pos p2);

void randmap(int size, int iter1, int iter2, int iter3);

void randmap2(int iter);

bool getmap(string filepath);

// paranoid sanity test for the map. Anything that passes is usable by the program.
int checkmap();

// symmetric line of sight using the mathematical formalism of digital lines.
void showdir(int dir, int dis);

void clearbuff();
void showbuff();

void mem_draw(int g, int cx, int cy);

void remember(int dis, int MEM);

// generate a random digital line of length n and height h
void genpath(int n, int h);

void save();

#endif
