#include "maze.h"

// based on that paper on random generation of finite sturmian words. You know the one.
// too lazy to implement Farey sequences right now. I just want to see if this works first...
// optimizations come later, if ever

void arrowline(int dir, int n, int p, int q, int a) {
	int b = (q - (a*p)%q)%q;
	for (int ad1 = 1, ad2 = 0, eps = b; ad1 <= n; ++ad1) {
		eps += p;
		if ((ad1 <= a && eps >= q) || (ad1 > a && eps > q)) {
			eps -= q;
			++ad2;
		}
		
		int cx = ad1, cy = ad2;
		if (dir&1) cx = -cx;
		if (dir&2) cy = -cy;
		if (dir&4) cx ^= cy, cy ^= cx, cx ^= cy;
		mvaddch(cy+centery, cx+centerx, '%');
	}
	getch();
}

int gcd(int a, int b) {
	if (a > b) a ^= b, b ^= a, a ^= b;
	if (a == 0) return b;
	return gcd (b%a, a);
}

void genpath(int n, int h) {
	if (n == 0) return;
	
	int dir = 0;
	if (abs(n) < abs(h)) dir |= 4, n ^= h, h ^= n, n ^= h;
	if (n < 0) dir |= 1, n = -n;
	if (h < 0) dir |= 2, h = -h;
	
	int p, q, a;
	while (1) {
		q = (rand()%n) + 1;
		p = (rand()%n) + 1;
		a = rand()%(n+1);
		
		int b = (q - (a*p)%q)%q;
		if (gcd(p, q) == 1 && a+q <= n && p*n + b > q*h && p*n + b <= q*h + q) break;
		if (p == 1 && q == 1 && a == n && n == h) break;
	}
	
	arrowline(dir, n, p, q, a);
}
