#include "maze.h"

int ground[PL];	// colors, doors, trees

int row, col, centerx, centery, rocks, rspot, colors[MONS], mons = 0;

char letters[MONS];

int occupant[PL];
bool seen[PL];

int buff[100][100];
int gridbuff[100][100];
bool drawbuff[100][100];

bool inview(int cx, int cy, int dis) {
	return (cx*cx + cy*cy <= dis*dis+2*dis);
}

bool opaque(int g) {
	return (ground[g] >= 16 && ground[g] < 32 && g != player.grid && g != rspot && occupant[g] == -1);
}

bool blocked(int g) {
	return (ground[g] >= 32);
}

void buffaddch(int cx, int cy, int ch) {
	drawbuff[cx][cy] = true;
	buff[cx][cy] = ch;
}

void clearbuff() {
	f(i,21) f(j,21) drawbuff[i][j] = false;
}

void showbuff() {
	f(i,21) f(j,21) if(drawbuff[i][j]) {
		mvaddch(i, j, buff[i][j]);
		seen[gridbuff[i][j]] = true;
	}
}

void draw(int g, int cx, int cy, bool exists) {
	if (g != player.grid) {
		if (g != rspot) buffaddch(cy, cx, '.' | COLOR_PAIR(ground[g]%16));
		else buffaddch(cy, cx, '*');	// rock
		if (occupant[g] != -1) buffaddch(cy, cx, letters[occupant[g]] | COLOR_PAIR(colors[occupant[g]]));
	} else buffaddch(cy, cx, '@');	// you see yourself in the distance
	if (opaque(g)) buffaddch(cy, cx, '+' | COLOR_PAIR(ground[g]%16));
	if (blocked(g)) buffaddch(cy, cx, '*' | A_BOLD | COLOR_PAIR(ground[g]%16));
	if (!exists) buffaddch(cy, cx, '#' | A_BOLD);
	
	gridbuff[cy][cx] = g;
}

void mem_draw(int g, int cx, int cy) {
	attron(COLOR_PAIR(5));
	attron(A_BOLD);
	mvaddch(cy, cx, '.');
	if (ground[g] >= 16 && ground[g] < 32 && g != player.grid && g != rspot) mvaddch(cy, cx, '+');
	if (blocked(g)) mvaddch(cy, cx, '*');
	if (g == -1) mvaddch(cy, cx, '#');
	attroff(COLOR_PAIR(5));
	attroff(A_BOLD);
}
