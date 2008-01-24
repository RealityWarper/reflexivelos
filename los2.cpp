#include <ncurses.h>
#include <string.h>
#include <cstdlib>
#include <time.h>

#define f(x,y) for (int x = 0; x < y; ++x)
#define ROW 1000
#define COL 1000

int adx[9] = {-1, 0, 1, -1, 0, 1, -1, 0, 1}, ady[9] = {1, 1, 1, 0, 0, 0, -1, -1, -1};
int row, col, px, py, ch;
bool rock[ROW][COL];
bool bold[ROW][COL];
char filechar[ROW][COL];
bool from_file;

void initialize() {
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	cbreak();
	start_color();
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	curs_set(FALSE);
	srand((unsigned) time(NULL));
	
	row = 20, col = 60;
	px = col/2, py = row/2;
	f(i,row) f(j,col) if (i == 0 || i == row-1 || j == 0 || j == col-1 || rand()%13 < 4) rock[i][j] = true;
	f(i,row) f(j,col) if (rand()%3 == 0) bold[i][j] = true;
}

void moveplayer(int dir) {
	if ((px+adx[dir] >= 0) && (py+ady[dir] >= 0) && (((px+adx[dir] < col) && (py+ady[dir] < row)) || from_file)) {
		if (!rock[py+ady[dir]][px+adx[dir]]) {
			px += adx[dir];
			py += ady[dir];
		}
	}
}

void draw (int cx, int cy, int dis) {
	if ((cx-px)*(cx-px) + (cy-py)*(cy-py) <= dis*dis + 1) {	// circular view - can be changed if you like
		if (!from_file) {
			if (rock[cy][cx]) {
				if (bold[cy][cx]) mvaddch(cy, cx, '*' | A_BOLD | COLOR_PAIR(2));
				else mvaddch(cy, cx, '*' | COLOR_PAIR(2));
			} else mvaddch(cy, cx, '.' | COLOR_PAIR(1));
		} else if (cx-px+col/2 >= 0 && cx-px+col/2 <= col && cy-py+row/2 >= 0 && cy-py+row/2 <= row) {
			mvaddch(cy-py+row/2, cx-px+col/2, filechar[cy][cx]);
		}
	}
}

int ccw(int x1, int y1, int x2, int y2, int x3, int y3) {	// positive iff they are counterclockwise
	return (x1*y2 + x2*y3 + x3*y1 - x1*y3 - x2*y1 - x3*y2);
}

// runs in O(N), "point" (read: unit length segment) to "point" line of sight that also checks intermediate "point"s.
// Gives identical results to the other algorithm, amazingly. Both are equivalent to checking for digital lines.
// you see those inner loops? Amortized time. Each while loop is entered at most N times, total.
void trace(int dir, int n, int h) {
	int topx[n+2], topy[n+2], botx[n+2], boty[n+2];	// convex hull of obstructions
	int curt = 0, curb = 0;	// size of top and bottom convex hulls
	int s[2][2] = {{0, 0}, {0, 0}};	// too lazy to think of real variable names, four critical points on the convex hulls - these four points determine what is visible
	topx[0] = botx[0] = boty[0] = 0, topy[0] = 1;
	for (int ad1 = 1, ad2[2] = {0, 0}, eps[2] = {0, n-1}; ad1 <= n; ++ad1) {
		f(i,2) {
			eps[i] += h;	// good old Bresenham
			if (eps[i] >= n) {
				eps[i] -= n;
				++ad2[i];
			}
		}
		f(i,2) if (ccw(topx[s[!i][1]], topy[s[!i][1]], botx[s[i][0]], boty[s[i][0]], ad1, ad2[i]+i) <= 0) return;	// the relevant region is no longer visible. If we don't exit the loop now, strange things happen.
		int cx[2] = {ad1, ad1}, cy[2] = {ad2[0], ad2[1]};
		f(i,2) {
			if (dir&1) cx[i] = -cx[i];
			if (dir&2) cy[i] = -cy[i];
			if (dir&4) cx[i] ^= cy[i], cy[i] ^= cx[i], cx[i] ^= cy[i];
			cx[i] += px, cy[i] += py;
			
			if (ccw(topx[s[i][1]], topy[s[i][1]], botx[s[!i][0]], boty[s[!i][0]], ad1, ad2[i]+1-i) > 0) {
				draw(cx[i], cy[i], n);
			}
		}
		
		if (rock[cy[0]][cx[0]]) {	// new obstacle, update convex hull
			++curb;
			botx[curb] = ad1, boty[curb] = ad2[0]+1;
			if (ccw(botx[s[0][0]], boty[s[0][0]], topx[s[1][1]], topy[s[1][1]], ad1, ad2[0]+1) >= 0) return;	// the obstacle obscures everything
			if (ccw(topx[s[0][1]], topy[s[0][1]], botx[s[1][0]], boty[s[1][0]], ad1, ad2[0]+1) >= 0) {
				s[1][0] = curb;	// updating visible region
				while (s[0][1] < curt && ccw(topx[s[0][1]], topy[s[0][1]], topx[s[0][1]+1], topy[s[0][1]+1], ad1, ad2[0]+1) >= 0) ++s[0][1];
			}
			while (curb > 1 && ccw(botx[curb-2], boty[curb-2], botx[curb-1], boty[curb-1], ad1, ad2[0]+1) >= 0) {	// not convex anymore, delete a point
				if (s[1][0] == curb) --s[1][0];	// s[0][0] won't be a problem
				--curb;
				botx[curb] = botx[curb+1], boty[curb] = boty[curb+1];
			}
		}
		
		if (rock[cy[1]][cx[1]]) {	// same as above
			++curt;
			topx[curt] = ad1, topy[curt] = ad2[1];
			if (ccw(botx[s[1][0]], boty[s[1][0]], topx[s[0][1]], topy[s[0][1]], ad1, ad2[1]) >= 0) return;
			if (ccw(topx[s[1][1]], topy[s[1][1]], botx[s[0][0]], boty[s[0][0]], ad1, ad2[1]) >= 0) {
				s[1][1] = curt;
				while (s[0][0] < curb && ccw(botx[s[0][0]], boty[s[0][0]], botx[s[0][0]+1], boty[s[0][0]+1], ad1, ad2[1]) <= 0) ++s[0][0];
			}
			while (curt > 1 && ccw(topx[curt-2], topy[curt-2], topx[curt-1], topy[curt-1], ad1, ad2[1]) <= 0) {
				if (s[1][1] == curt) --s[1][1];
				--curt;
				topx[curt] = topx[curt+1], topy[curt] = topy[curt+1];
			}
		}
	}
}

// The "other algorithm" - based on generalization of Bresenham, runs in O(N^3), and thus less than optimal. An earlier version of this ran in O(N^4), because it checked each value of s independently.
// Easier to understand, easier to remember, easier to code, easier to debug. Never uses multiplictaion. Slower!
void showdir(int dir, int dis) {
	int cx, cy;
	for (int q = 1; q <= dis; ++q) f(p,q+1) {
		for (int ad1 = 1, ad2[2] = {0, 0}, s[2] = {0, q-1}, eps[2] = {0, q-1}; ad1 <= dis && s[0] <= s[1]; ++ad1) f(i,2) {
			eps[i] += p;
			if (eps[i] >= q) {	// this should look familiar
				eps[i] -= q;
				++ad2[i];
			}
			cx = ad1, cy = ad2[i];
			if (dir&1) cx = -cx;
			if (dir&2) cy = -cy;
			if (dir&4) cx ^= cy ^= cx ^= cy;
			cx += px, cy += py;
			draw(cx, cy, dis);
			if (rock[cy][cx]) {	// update range of possibilities for s
				if (i == 0) s[i] += q-eps[i], eps[i] = 0, ++ad2[0];
				if (i == 1) s[i] -= eps[i]+1, eps[i] = q-1, --ad2[1];
			}
		}
	}
}

int distance;

bool toggle = false;

int call[2] = {1, 1};
clock_t timer[2];

void showgame() {
	erase();
	
	++call[toggle];
	clock_t start = clock();
	f(dir, 8) {
		if (toggle) showdir(dir, distance);	// old algorithm
		else f(i,distance+1) trace(dir, distance, i);	// new one!
	}
	timer[toggle] += clock()-start;
	
	if (!from_file) mvaddch(py, px, '@' | A_BOLD);
	else mvaddch(row/2, col/2, '@' | A_BOLD);
	
	mvprintw(row+1, 0, "Numpad moves, 'q' quits, space toggles between algorithms, 'r' is random.");
	mvprintw(row+2, 0, "New alg average ticks: %f,\tOld alg average ticks: %f", timer[0]/((double) call[0]), timer[1]/((double) call[1]));
	refresh();
}

void playgame() {
	while (1) {
		showgame();
		ch = getch();
		if (ch == 'q') break;
		if (ch >= '1' && ch <= '9') moveplayer(ch-'1');
		if (ch == ' ') toggle = !toggle;
		if (ch >= 'a' && ch <= 'z') toggle = false;
		if (ch == 'r') {
			moveplayer(rand()%9);
			toggle = rand()%2;
		}
	}
}

void cleanup() {
	endwin();
}

void readfile(char *filepath) {
	f(i,ROW) f(j,COL) rock[i][j] = false;
	FILE *fin = fopen(filepath, "r");
	if (fin == NULL) return;
	int curx = 0, cury = 0;
	while (!feof(fin) && curx < COL && cury < ROW) {
		char c = fgetc(fin);
		if (c == '\n') {
			++cury, curx = 0;
			continue;
		}
		if (c == '#') rock[cury][curx] = true;
		if (c == '@') px = curx, py = cury;
		filechar[cury][curx] = c;
		++curx;
	}
}

int main(int argc, char **argv) {
	distance = 10;
	if (argc > 1) distance = atoi(argv[1]);
	
	initialize();
	
	if (argc > 2) {	// input from file!
		readfile(argv[2]);
		from_file = true;
	} else from_file = false;
	
	playgame();
	
	cleanup();
	
	return 0;
}
