#include "maze.h"

void initialize() {
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	cbreak();
	start_color();
	f(i,8) init_pair(i, i, COLOR_BLACK);
	curs_set(FALSE);
	
	row = 20, col = 20;
	centerx = col/2, centery = row/2;
	
	srand((unsigned) time(NULL));
}

pos monster[MONS], marked;
bool mem_toggle = false;
bool edit = false;

void showgame() {
	erase();
	
	if (mem_toggle) remember(10, 41);
	
	clearbuff();
	f(dir, 8) showdir(dir, 10);
	showbuff();
	
	mvaddch(centery, centerx, '@' | A_BOLD);
	
	mvprintw(row+1, 0, "Numpad moves, >/< rotates, | mirrors, g gets, d drops, q quits.");
	mvaddch(5, col+10, ACS_DEGREE); printw(" = Goal");
	mvaddch(6, col+10, '*' | A_BOLD); printw(" = Tree");
	mvprintw(7, col+10, "+ = Door");
	mvaddch(8, col+10, '#' | A_BOLD); printw(" = Wall");
	mvprintw(9, col+10, ". = Floor");
	if (rocks) mvprintw(row+2, 0, "You have a rock!");
	f(i,mons) if (player.grid == monster[i].grid) {
		mvprintw(row+3, 0, "You are stepping on something squishy.");
		occupant[player.grid] = -1;
		monster[i].grid = -1;
		ground[player.grid] = colors[i] + (ground[player.grid]/16)*16;
	}
	refresh();
}

void editglue() {
	if (canglue(player, marked)) glue(player, marked);
	
	/*pos tmp = player;
	int dir = 0;
	char ch = getch();
	f(i,9) if (ch == moves[i] || ch == vikeys[i]) ch = i+'1';
	if (ch >= '1' && ch <= '9' && ch != '5') dir = trans[ch-'1'];
	else continue;
	if (dir%2 == 0) {
		tmp.rotate(dir+2);
		f(i,100) {
			pos other(rand()%pl, ((rand()%8)/2)*2, (rand()%2)*2-1);
			if (canglue(tmp, other)) {
				glue(tmp, other);
				break;
			}
		}
	} else if (!tmp.move(dir+7) && !tmp.move(dir+1) && !tmp.move(dir)) {
		f(i,100) {
			pos other(rand()%pl, ((rand()%8)/2)*2, (rand()%2)*2-1);
			if (!other.move(dir+3) && !other.move(dir+5) && !other.move(dir+4)) {
				tmp.patch(dir, other), other.patch(dir+4, tmp);
				break;
			}
		}
	}
	ch = ' ';*/
}

void editnew() {
	f(i,PL) {
		marked.grid = i;
		bool empty = true;
		f(dir,8) if (marked.move(dir)) empty = false;
		if (empty) {
			ground[marked.grid] = ground[player.grid];
			break;
		}
	}
}

void playgame() {
	int moves[9] = {KEY_END, KEY_DOWN, KEY_NPAGE, KEY_LEFT, -1, KEY_RIGHT, KEY_HOME, KEY_UP, KEY_PPAGE};
	char vikeys[9] = {'b', 'j', 'n', 'h', -1, 'l', 'y', 'k', 'u'};
	
	mons = MONS;
	f(i,300) mons = min(mons, rand()%pl);
	f(i,mons) {
		do monster[i].grid = rand()%pl; while (blocked(monster[i].grid) && monster[i].grid == 0);
		monster[i].rot = 0, monster[i].mir = 1;
		letters[i] = rand()%26 + (rand()%3?'a':'A');
		colors[i] = rand()%COLORS;
	}
	f(i,PL) occupant[i] = -1;
	marked = player;
	while (1) {
		f(i,mons) if (monster[i].grid > -1) occupant[monster[i].grid] = i;
		showgame();
		int ch = getch();
		if (ch == 'q') break;
		if (ch == '>') player.rotate(-2);
		if (ch == '<') player.rotate(2);
		if (ch == '|') player.reflect();
		if ((ch == ',' || ch == 'g') && player.grid == rspot) rspot = -1, ++rocks;
		if (ch == 'c' && edit) {
			++ground[player.grid];
			if (ground[player.grid]%16 == 9) ground[player.grid] += 7;
			ground[player.grid] %= 48;
		}
		if (ch == 'C' && edit) ground[player.grid] = (ground[player.grid]+16)%48;
		if (ch == 'W' && edit) ground[player.grid] = 0;
		if (ch == 'V' && edit) ground[player.grid] = ground[marked.grid];
		if (ch == 'd' && rocks) rspot = player.grid, --rocks;
		if (ch == 'E') edit = !edit, ch = 'K';
		if (ch == 'm' && edit) mem_toggle = !mem_toggle;
		if (ch == 'M' && edit) marked = player;
		if (ch == 'N' && edit) editnew();
		if (ch == 'f') if (!player.move(6) || blocked(player.grid)) {
			if (blocked(player.grid)) player.move(2);
			player = follow_wall(player);
			while (blocked(player.grid)) {
				if (player.move(0)) player.rotate(-2);
				else player.move(7), player.rotate(4);
			}
		} else if (!blocked(player.grid)) player.move(2);
		if (ch == 'K') f(i,mons) occupant[monster[i].grid] = -1, monster[i].grid = -1;
		/*if (ch == 'L') {
			curs_set(TRUE);
			int xpos = 0, ypos = 0;
			move(ypos+centery, xpos+centerx);
			while (1) {
				ch = getch();
				f(i,9) if (ch == moves[i] || ch == vikeys[i]) ch = i+'1';
				if (ch >= '1' && ch <= '9' && ch != '5') {
					xpos += adx[trans[ch-'1']], ypos += ady[trans[ch-'1']];
					if (xpos > centerx) xpos = centerx;
					if (ypos > centery) ypos = centery;
					if (xpos < -centerx) xpos = -centerx;
					if (ypos < -centery) ypos = -centery;
				} else break;
				move(ypos+centery, xpos+centerx);
			}
			curs_set(FALSE);
			if (ch != 'q') genpath(xpos, ypos);
		}*/
		if (ch == 'B' && edit) {
			pos tmp = player;
			ch = getch();
			f(i,9) if (ch == moves[i] || ch == vikeys[i]) ch = i+'1';
			if (ch >= '1' && ch <= '9') {
				if (ch == '5' || tmp.move(trans[ch-'1'])) {
					tmp.destroy();
					ground[tmp.grid] = (rand()%5)*(!(rand()%100)) + (rand()%3)*(!(rand()%8))*16;
				}
			}
			ch = ' ';
		}
		// if (ch == 'p' && edit);
		if (ch == 'P' && edit) editglue();
		if (ch == 'H' && edit) {
			editnew();
			editglue();
			ch = 'h';
		}
		// if (ch == 'V' && edit) f(i,pl) seen[i] = true;
		if (ch == 'S') save();
		if (ch == 't' && edit) player.grid = rand()%pl, player.rot = ((rand()%8)/2)*2, player.mir = (rand()%2)*2-1;
		if (ch == 'T' && edit) player = marked;
		
		f(i,9) if (ch == moves[i] || ch == vikeys[i]) ch = i+'1';
		if (ch >= '1' && ch <= '9' && ch != '5') {
			bool goanyways = false;
			if (rspot == player.grid || edit) goanyways = true;
			player.move(trans[ch-'1']);
			if (blocked(player.grid) && !goanyways) player.move(trans[ch-'1']+4);
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
	
	if (filepath[0] == '-' && filepath[1] == 'r') {
		int size = 1000;
		if (argc > 2) size = atoi(argv[2]);
		int iter1 = 4*size;
		if (argc > 3) iter1 = atoi(argv[3]);
		int iter2 = iter1/2;
		if (argc > 4) iter2 = atoi(argv[4]);
		int iter3 = 0;
		if (argc > 5) iter3 = atoi(argv[5]);
		//do {
			randmap(size, iter1, iter2, iter3);
		//} while (checkmap() < 0);
	} else if (filepath[0] == '-' && filepath[1] == 's') {
		int iter = 9;
		if (argc > 2) iter = atoi(argv[2]);
		randmap2(iter);
	} else if (!getmap(filepath)) {
		endwin();
		return 0;
	}
	
	int x = checkmap();
	if (x >= 0) {
		mvprintw(centery, centerx, "Bad map: first badness around gridspot %d", x);
		getch();
	} else playgame();
	
	endwin();
	
	return 0;
}
