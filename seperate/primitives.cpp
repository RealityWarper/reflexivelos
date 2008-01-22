#include "maze.h"

int ground[PL];	// colors, doors, trees

int row, col, centerx, centery, rocks, rspot, colors[MONS], mons = 0;

char letters[MONS];

int occupant[PL];

bool inview(int cx, int cy, int dis) {
	return (cx*cx + cy*cy <= dis*dis+2*dis);
}

bool opaque(int g) {
	return (ground[g] >= 16 && ground[g] < 32 && g != player.grid && g != rspot && occupant[g] == -1);
}

bool blocked(int g) {
	return (ground[g] >= 32);
}

void draw(int g, int cx, int cy, bool exists) {
	/*attron(COLOR_PAIR(5));
	attron(A_BOLD);//*/
	if (g != player.grid) {
		if (g != rspot) mvaddch(cy, cx, '.' | COLOR_PAIR(ground[g]%16));
		else mvaddch(cy, cx, '*');	// rock
		if (occupant[g] != -1) mvaddch(cy, cx, letters[occupant[g]] | COLOR_PAIR(colors[occupant[g]]));
	} else mvaddch(cy, cx, '@');	// you see yourself in the distance
	if (opaque(g)) mvaddch(cy, cx, '+' | COLOR_PAIR(ground[g]%16));
	if (blocked(g)) mvaddch(cy, cx, '*' | A_BOLD | COLOR_PAIR(ground[g]%16));
	if (!exists) mvaddch(cy, cx, '#' | A_BOLD);
	/*attroff(COLOR_PAIR(5));
	attroff(A_BOLD);//*/
}
