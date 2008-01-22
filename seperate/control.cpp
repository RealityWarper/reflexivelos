#include "maze.h"

void initialize() {
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	cbreak();
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	init_pair(4, COLOR_GREEN, COLOR_BLACK);
	init_pair(5, COLOR_BLACK, COLOR_BLACK);
	curs_set(FALSE);
	
	row = 20, col = 40;
	centerx = col/2, centery = row/2;
	
	srand((unsigned) time(NULL));
}

pos monster[MONS];

void showgame() {
	erase();
	
	f(dir, 8) showdir(dir, 10);
	
	mvaddch(centery, centerx, '@' | A_BOLD);
	
	mvprintw(row+1, 0, "Numpad moves, >/< rotates, | mirrors, g gets, d drops, q quits.");
	if (rocks) mvprintw(row+2, 0, "You have a rock!");
	f(i,mons) if (player.grid == monster[i].grid) {
		mvprintw(row+3, 0, "You are stepping on something squishy.");
		occupant[player.grid] = -1;
		monster[i].grid = -1;
		ground[player.grid] = colors[i] + (ground[player.grid]/16)*16;
	}
	refresh();
}

void playgame() {
	mons = MONS;
	f(i,30) mons = min(mons, rand()%pl);
	f(i,mons) {
		do monster[i].grid = rand()%pl; while (blocked(monster[i].grid) && monster[i].grid == 0);
		monster[i].rot = 0, monster[i].mir = 1;
		letters[i] = rand()%26 + (rand()%3?'a':'A');
		colors[i] = rand()%COLORS;
	}
	f(i,pl) occupant[i] = -1;
	while (1) {
		f(i,mons) if (monster[i].grid > -1) occupant[monster[i].grid] = i;
		showgame();
		int ch = getch();
		if (ch == 'q') break;
		if (ch == '>') player.rotate(-2);
		if (ch == '<') player.rotate(2);
		if (ch == '|') player.reflect();
		if ((ch == ',' || ch == 'p' || ch == 'g') && player.grid == rspot) rspot = -1, ++rocks;
		if (ch == 'd' && rocks) rspot = player.grid, --rocks;
		if (ch == 'f') if (!player.move(6) || blocked(player.grid)) {
			if (blocked(player.grid)) player.move(2);
			player = follow_wall(player);
			while (blocked(player.grid)) {
				if (player.move(0)) player.rotate(-2);
				else player.move(7), player.rotate(4);
			}
		} else if (!blocked(player.grid)) player.move(2);
		
		int moves[9] = {KEY_END, KEY_DOWN, KEY_NPAGE, KEY_LEFT, -1, KEY_RIGHT, KEY_HOME, KEY_UP, KEY_PPAGE};
		f(i,9) if (ch == moves[i]) ch = i+'1';
		if (ch >= '1' && ch <= '9' && ch != '5') {
			player.move(trans[ch-'1']);
			if (blocked(player.grid)) player.move(trans[ch-'1']+4);
		}
		f(i,mons) if (monster[i].grid > -1 && rand()%9) {
			occupant[monster[i].grid] = -1;
			int dir = rand()%8;
			monster[i].move(dir);
			if (blocked(monster[i].grid) || monster[i].grid == rspot) monster[i].move(dir+4);
		}
	}
}

int main(int argc, char **argv) {
	initialize();
	
	string filepath = "maze.txt";
	if (argc > 1) filepath = argv[1];
	
	if (filepath[0] == '-') {
		int size = 1000;
		if (argc > 2) size = atoi(argv[2]);
		int iter = 10*size;
		if (argc > 3) iter = atoi(argv[3]);
		//do {
			randmap(size, iter);
		//} while (checkmap() < 0);
	}
	else getmap(filepath);
	
	int x = checkmap();
	if (x >= 0) {
		mvprintw(centery, centerx, "Bad map: first badness around gridspot %d", x);
		getch();
	} else playgame();
	
	endwin();
	
	return 0;
}
