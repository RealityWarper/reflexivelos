#include <ncurses.h>
#include <string.h>
#include <cstdlib>
#include <vector>
#include <time.h>
#include <string>

#define f(x,y) for (int x = 0; x < y; ++x)
#define ROW 1000
#define COL 1000
#define PL 100000
#define MONS 1000
#define COLORS 5

using namespace std;

int mov[PL][8], rotor[PL][8], pl = 0;	// rotor is for cycles which leave you rotated, pl is number of places
bool mirror[PL][8];	// does the connection reverse chirality?

int ground[PL];	// colors, doors, tress

// 0 is down, 1 is down-right, 2 is right, 3 is right-up, etc: rotating is adding and mirroring is subtracting
int trans[9] = {7, 0, 1, 6, -1, 2, 5, 4, 3}, adx[8] = {0, 1, 1, 1, 0, -1, -1, -1}, ady[8] = {1, 1, 0, -1, -1, -1, 0, 1};
int row, col, centerx, centery, rocks, rspot, colors[MONS], mons = 0;

string filepath = "maze.txt";
char letters[MONS];

struct pos {
	int grid, rot, mir;
	
	void rotate(int r2) {
		rot = (rot+r2*mir+8)%8;
	}
	
	void reflect() {
		mir = -mir;
	}
	
	bool move(int dir) {
		int rdir = (dir*mir + rot + 16)%8;
		if (mov[grid][rdir] < 0) return false;
		if (mirror[grid][rdir]) {
			reflect();
			rot = (8+2*rdir-rot)%8;	// conjugation in D_8
		}
		rotate(rotor[grid][rdir]*mir);
		grid = mov[grid][rdir];
		return true;
	}
	
	// reverse-engineers move - makes p2 the result of move(dir)
	void patch(int dir, pos p2) {
		int rdir = (dir*mir + rot + 16)%8, nrot = rot;
		mov[grid][rdir] = p2.grid;
		if (mir != p2.mir) {
			mirror[grid][rdir] = true;
			nrot = (8+2*rdir-nrot)%8;
		} else mirror[grid][rdir] = false;
		rotor[grid][rdir] = (p2.rot-nrot+8)%8;
		if (grid >= pl) pl = grid+1;
	}
	
	int follow_wall() {
		rotate(-2);
		if (move(6)) {
			rotate(2);
			if (move(6)) {
				rotate(2);
				return 1;
			}
			return 0;
		}
		return -1;
	}
} player, monster[MONS];

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

pos follow_wall(pos p) {
	if (p.move(5)) p.rotate(2);
	else if (!p.move(4)) p.rotate(-2);
	return p;
}

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

void glue(pos p1, pos p2) {	// glues left side of p1 to left side of p2, fills in missing connections
	f(iter, 2) {
		pos orig1 = p1, orig2 = p2;
		do {
			pos op1 = p1, rp1 = p1, op2 = p2, rp2 = p2;
			int num1 = p1.follow_wall(), num2 = p2.follow_wall();
			rp1.reflect(), rp2.reflect();
			op1.patch(6, rp2), rp2.patch(2, op1);
			if (op1.move(4)) op1.patch(7, rp2), rp2.patch(3, op1);
			if (op2.move(4)) op2.patch(7, rp1), rp1.patch(3, op2);
			if (num1+num2 != 0) break;
		} while (orig1.grid != p1.grid || orig1.mir != p1.mir || orig1.rot != p1.rot);
		p1 = orig2, p2 = orig1;
		p1.rotate(4), p2.rotate(4);
		p1.reflect(), p2.reflect();
	}
}

pos leftward[PL][4];
int lefttime[PL][4], leftnow = 0, leftmir[PL][4];

bool recent(pos p) {
	int ind = ((p.rot/2)+p.mir+4)%4;
	return (lefttime[p.grid][ind] == leftnow);
}

bool testpatch(pos p1, pos p2) {
	if (p1.move(6)) return false;
	int ind1 = ((p1.rot/2)+p1.mir+4)%4, ind2 = ((p2.rot/2)+p2.mir+4)%4;
	if (recent(p1)) {
		int indl = ((leftward[p1.grid][ind1].rot/2)+leftward[p1.grid][ind1].mir+4)%4;
		if (leftward[p1.grid][ind1].grid != p2.grid || indl != ind2) return false;
		if (leftmir[p1.grid][ind1] != p1.mir*p2.mir) return false;
	} else {
		lefttime[p1.grid][ind1] = leftnow;
		leftward[p1.grid][ind1] = p2;
		leftmir[p1.grid][ind1] = p2.mir*p1.mir;
	}
	return true;
}

bool testleft(pos &p) {
	if (p.move(6)) return true;
	int ind = ((p.rot/2)+p.mir+4)%4;
	if (lefttime[p.grid][ind] != leftnow) return false;
	pos p2 = leftward[p.grid][ind];
	if (p2.mir*p.mir != leftmir[p.grid][ind]) p2.rotate(4), p2.reflect();
	p2.reflect();
	p = p2;
	return true;
}

int testfollow(pos &p) {
	p.rotate(-2);
	if (testleft(p)) {
		p.rotate(2);
		if (testleft(p)) {
			p.rotate(2);
			return 1;
		}
		return 0;
	}
	return -1;
}

bool canglue(pos p1, pos p2) {	// I don't know why, but it seems to work
	++leftnow;
	f(iter, 2) {
		pos orig1 = p1, orig2 = p2;
		do {
			pos op1 = p1, op2 = p2;
			//int num1 = p1.follow_wall(), num2 = p2.follow_wall();
			int num1 = testfollow(p1), num2 = testfollow(p2);
			if (!testpatch(op1, op2)) return false;
			if (!testpatch(op2, op1)) return false;
			if (num1+num2 > 0) {
				if (!testleft(p1)) return false;
				p1.rotate(-2), p1.reflect();
				if (p1.grid != op2.grid || p1.rot != op2.rot || p1.mir != op2.mir) return false;
				break;
			}
			
			/*bool b1 = (orig1.grid != p1.grid || orig1.mir != p1.mir || orig1.rot != p1.rot);
			bool b2 = (orig2.grid != p2.grid || orig2.mir != p2.mir || orig2.rot != p2.rot);
			if (b1 != b2 && num1+num2 == 0) return false;*/
			
			if (num1 == 0 && num2 == -1 && recent(p1)) return false;	// WTF
			if (num1 == -1 && num2 == 0 && recent(p2)) return false;
			
			if (num1+num2 != 0) break;
		} while (orig1.grid != p1.grid || orig1.mir != p1.mir || orig1.rot != p1.rot);
		p1 = orig2, p2 = orig1;
		p1.rotate(4), p2.rotate(4);
		p1.reflect(), p2.reflect();
	}
	return true;
}

void randmap(int size, int iter) {
	f(i,size) f(j,4) lefttime[i][j] = -1;
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
	
	f(i,iter) {
		pos first = edges[rand()%(size*8)], second = edges[rand()%(size*8)];
		if (!first.move(0) && !first.move(2) && !first.move(1) && !second.move(4) && !second.move(6) && !second.move(5)) {
			first.patch(1, second), second.patch(5, first);
		}
	}
}

void getchart(int w, int h, FILE *fin) {
	char c;
	vector<int> offset;
	pos tpos[w+2][h+2];
	for (c = fgetc(fin); (c < '0' || c > '9') && c != '-'; c = fgetc(fin)) if (c == '+') {
		int tmp;
		fscanf(fin, "%d", &tmp);
		offset.push_back(tmp);
	}
	ungetc(c, fin);
	if (offset.empty()) offset.push_back(0);
	f(x,w+2) f(y,h+2) tpos[x][y].grid = -1;
	for (int y = 1; y <= h; ++y) for (int x = 1; x <= w; ++x) {
		fscanf(fin, "%d%c", &tpos[x][y].grid, &c);
		tpos[x][y].rot = (((c-'a'+256)%8)/2)*2, tpos[x][y].mir = ((c-'a'+256)%2?-1:1);
		if (c == ' ' || c == '\t' || c == '\n' || c == '\r') tpos[x][y].rot = 0, tpos[x][y].mir = 1;
	}
	for (vector<int>::iterator i = offset.begin(); i != offset.end(); ++i) {
		for (int y = 1; y <= h; ++y) for (int x = 1; x <= w; ++x) if (tpos[x][y].grid >= 0) tpos[x][y].grid += *i;
		for (int y = 1; y <= h; ++y) for (int x = 1; x <= w; ++x) if (tpos[x][y].grid >= 0) {
			for (int dir = 1; dir < 8; dir += 2) if (tpos[x+adx[dir]][y+ady[dir]].grid != -1)
				tpos[x][y].patch(dir, tpos[x+adx[dir]][y+ady[dir]]);
			for (int dir = 0; dir < 4; dir += 2) if (tpos[x+adx[dir]][y+ady[dir]].grid != -1) {
				tpos[x][y].rotate(dir-6), tpos[x+adx[dir]][y+ady[dir]].rotate(dir-6);
				tpos[x+adx[dir]][y+ady[dir]].reflect();
				//if (canglue(tpos[x][y], tpos[x+adx[dir]][y+ady[dir]]))
					glue(tpos[x][y], tpos[x+adx[dir]][y+ady[dir]]);
				tpos[x+adx[dir]][y+ady[dir]].reflect();
				tpos[x][y].rotate(6-dir), tpos[x+adx[dir]][y+ady[dir]].rotate(6-dir);
			}
		}
		for (int y = 1; y <= h; ++y) for (int x = 1; x <= w; ++x) if (tpos[x][y].grid >= 0) tpos[x][y].grid -= *i;
	}
}

bool getmap() {
	player.grid = player.rot = 0, player.mir = 1;
	f(i,PL) f(j,8) mov[i][j] = -1, rotor[i][j] = 0, mirror[i][j] = false;
	
	FILE *fin = fopen(filepath.c_str(), "r");
	if (fin != NULL) {
		char cmt = '@';
		while (cmt < '0' || cmt > '9') cmt = fgetc(fin);	// psuedo comment hack
		ungetc(cmt, fin);
		fscanf(fin, "%d %d\n", &player.grid, &rspot);	// where the player and rock start out
		while (!feof(fin)) {	// inputs map from local charts
			int width, height;
			if (fscanf(fin, "%d %d\n", &height, &width) != 2 || height <= 0 || width <= 0) break;
			getchart(width, height, fin);
		}
		f(iter2,pl) ground[iter2] = 0;
		while (!feof(fin)) {	// inputs colors, etc.
			int st, en, col;
			if (fscanf(fin, "%d %d %d\n", &st, &en, &col) != 3) break;
			char effect = ' ';
			effect = fgetc(fin);
			if (effect >= '0' && effect <= '9') ungetc(effect, fin);
			if (st < 0) break;
			for (int j = st; j <= en; ++j) if (effect != '?' || rand()%2) ground[j] = col;
		}
	} else {
		mvprintw(centery, centerx, "No map :?");
		getch();
		return false;
	}
	fclose(fin);
	return true;
}

// paranoid sanity test for the map. Anything that passes is usable by the program.
int checkmap() {
	f(i,pl) for (int j = 0; j < 8; j += 2) for (int k = -1; k <= 1; k += 2) {	// all spots and directions
		pos test;
		test.grid = i, test.rot = j, test.mir = k;
		f(dir,8) {	// checks that all movement reverses properly
			if (test.move(dir)) test.move((dir+4)%8);
			if (test.grid != i || test.rot != j || test.mir != k) return i;
		}
		for (int dir = 1; dir < 8; dir += 2) {	// checks that diagonals make sense
			int t1 = -10, t2 = -10, t3 = -10, dirs[4] = {(dir+1)%8, (dir+7)%8, (dir+5)%8, (dir+3)%8};
			if (test.move(dir)) {
				t1 = test.grid, t2 = test.rot, t3 = test.mir;
				test.move((dir+4)%8);
			}
			f(wch,2) if (test.move(dirs[wch])) {
				if (test.move(dirs[wch^1])) {
					if (t1 != test.grid || t2 != test.rot || t3 != test.mir) return i;
					test.move(dirs[wch^3]);
				} else if (t1 != -10) return i;
				test.move(dirs[wch^2]);
			}
		}
	}
	return -1;
}

// O(dis^3) reflexive line of sight using the mathematical formalism of digital lines.
void showdir(int dir, int dis) {
	int changed1[8] = {2, 6, 2, 6, 0, 4, 0, 4}, changed2[8] = {1, 7, 3, 5, 1, 3, 7, 5};	// don't ask
	int dir1 = changed1[dir], dir2 = changed2[dir], ndir;	// dir1 is straight, dir2 is diagonal
	for (int q = 1; q <= dis; ++q) f(p,q+1) {	// generalization of Brun's algorithm, p/q is slope
		int ad1 = 1, ad2[2] = {0, 0}, eps[2] = {0, q-1}, s[2] = {0, q-1};	// s is initial height of the line
		for (pos npos[2] = {player, player}; ad1 <= dis && s[0] <= s[1]; ++ad1) f(i,2) {
			eps[i] += p, ndir = dir1;
			if (eps[i] >= q) eps[i] -= q, ++ad2[i], ndir = dir2;
			int cx = ad1, cy = ad2[i];
			if (dir&1) cx = -cx;	// again, don't ask
			if (dir&2) cy = -cy;
			if (dir&4) cx ^= cy, cy ^= cx, cx ^= cy;
			bool moveable = npos[i].move(ndir);
			if (inview(cx, cy, dis)) draw(npos[i].grid, cx+centerx, cy+centery, moveable);
			if (!moveable || opaque(npos[i].grid)) {	// something's in the way, move the other way
				s[i] -= eps[i]+i*q+i-q, eps[i] = i*q-i, ad2[i] -= 2*i-1, npos[i] = npos[!i];	// change initial height
				if (i == 0) npos[0].move(eps[1]+p >= q?dir2:dir1);	// npos[1] and eps[1] haven't changed yet
			}
		}
	}
}

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
	f(i,20) mons = min(mons, rand()%pl);
	f(i,mons) {
		do monster[i].grid = rand()%pl; while (blocked(monster[i].grid) && monster[i].grid);
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
	else getmap();
	
	int x = checkmap();
	if (x >= 0) {
		mvprintw(centery, centerx, "Bad map: first badness around gridspot %d", x);
		getch();
	} else playgame();
	
	endwin();
	
	return 0;
}
