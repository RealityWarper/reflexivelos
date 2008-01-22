#include "maze.h"

int ccw(int x1, int y1, int x2, int y2, int x3, int y3) {	// positive iff they are counterclockwise
	return (x1*y2 + x2*y3 + x3*y1 - x1*y3 - x2*y1 - x3*y2);
}

// runs in O(N), "point" (read: unit length segment) to "point" line of sight that also checks intermediate "point"s.
// Gives identical results to the other algorithm, amazingly. Both are equivalent to checking for digital lines.
// you see those inner loops? Amortized time. Each while loop is entered at most N times, total.
void trace(int dir, int n, int h) {
	int changed1[8] = {2, 6, 2, 6, 0, 4, 0, 4}, changed2[8] = {1, 7, 3, 5, 1, 3, 7, 5};	// don't ask
	int dir1 = changed1[dir], dir2 = changed2[dir], ndir;	// dir1 is straight, dir2 is diagonal
	pos npos[2] = {player, player};
	int rad2[2] = {0, 0};
	
	int topx[n+2], topy[n+2], botx[n+2], boty[n+2];	// convex hull of obstructions
	int curt = 0, curb = 0;	// size of top and bottom convex hulls
	int s[2][2] = {{0, 0}, {0, 0}};	// too lazy to think of real variable names, four critical points on the convex hulls - these four points determine what is visible
	topx[0] = botx[0] = boty[0] = 0, topy[0] = 1;
	for (int ad1 = 1, ad2[2] = {0, 0}, eps[2] = {0, n-1}; ad1 <= n; ++ad1) {
		bool moveable[2] = {true, true};
		f(i,2) {
			eps[i] += h;	// good old Bresenham
			ndir = dir1;
			if (eps[i] >= n) {
				eps[i] -= n;
				++ad2[i], ++rad2[i];
				ndir = dir2;
			}
			
			if (rad2[i] < ad2[i] && ndir == dir1) ndir = dir2, ++rad2[i];
			if (rad2[i] > ad2[i] && ndir == dir2) ndir = dir1, --rad2[i];
			
			if (rad2[i] != ad2[i]) moveable[i] = false;
			if (!npos[i].move(ndir)) moveable[i] = false;
		}
		f(i,2) if (!moveable[i]) {
			npos[i] = npos[!i];
			rad2[i] = rad2[!i];
		}
		
		f(i,2) if (ccw(topx[s[!i][1]], topy[s[!i][1]], botx[s[i][0]], boty[s[i][0]], ad1, ad2[i]+i) <= 0) return;	// the relevant region is no longer visible. If we don't exit the loop now, strange things happen.
		
		int cx[2] = {ad1, ad1}, cy[2] = {ad2[0], ad2[1]};
		f(i,2) {
			if (dir&1) cx[i] = -cx[i];
			if (dir&2) cy[i] = -cy[i];
			if (dir&4) cx[i] ^= cy[i], cy[i] ^= cx[i], cx[i] ^= cy[i];
			
			if (ccw(topx[s[i][1]], topy[s[i][1]], botx[s[!i][0]], boty[s[!i][0]], ad1, ad2[i]+1-i) > 0) {
				if (inview(cx[i], cy[i], n)) draw(npos[i].grid, cx[i]+centerx, cy[i]+centery, moveable[i]);
			}
		}
		
		if (!moveable[0] || opaque(npos[0].grid)) {	// new obstacle, update convex hull
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
		
		if (!moveable[1] || opaque(npos[1].grid)) {	// same as above
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

void showdir(int dir, int dis) {
	//if (rand()%2) {	// for testing
		f(i,dis+1) trace(dir, dis, i);	// use the new algorithm
		return;	// don't use the old one
	//}
	
	// O(dis^3) symmetric line of sight using the mathematical formalism of digital lines.
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
