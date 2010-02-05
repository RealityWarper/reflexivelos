#include "maze.h"

int mov[PL][8], rotor[PL][8], pl = 1;	// rotor is for cycles which leave you rotated, pl is number of places
bool mirror[PL][8];	// does the connection reverse chirality?

// 0 is down, 1 is down-right, 2 is right, 3 is right-up, etc: rotating is adding and mirroring is subtracting
int trans[9] = {7, 0, 1, 6, -1, 2, 5, 4, 3}, adx[8] = {0, 1, 1, 1, 0, -1, -1, -1}, ady[8] = {1, 1, 0, -1, -1, -1, 0, 1};

pos::pos(int g1, int r1, int m1) : grid(g1), rot(r1), mir(m1) {}

void pos::rotate(int r2) {
	rot = (rot+r2*mir+8)%8;
}

void pos::reflect() {
	mir = -mir;
}

bool pos::move(int dir) {
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
void pos::patch(int dir, pos p2) {
	int rdir = (dir*mir + rot + 16)%8, nrot = rot;
	mov[grid][rdir] = p2.grid;
	if (mir != p2.mir) {
		mirror[grid][rdir] = true;
		nrot = (8+2*rdir-nrot)%8;
	} else mirror[grid][rdir] = false;
	rotor[grid][rdir] = (p2.rot-nrot+8)%8;
	if (grid >= pl) pl = grid+1;
}

int pos::follow_wall() {	// different from the other one - no diagonals
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

void pos::destroy() {
	f(dir,8) {
		pos tmp(grid, rot, mir);
		if (tmp.move(dir)) {
			int rdir1 = (dir*mir + rot + 16)%8, rdir2 = ((dir+4)*tmp.mir + tmp.rot + 16)%8;
			mov[grid][rdir1] = mov[tmp.grid][rdir2] = -1;
		}
	}
}

pos player;

pos follow_wall(pos p) {	// this is different - diagonal movement is allowed!
	if (p.move(5)) p.rotate(2);
	else if (!p.move(4)) p.rotate(-2);
	return p;
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

