#include "maze.h"

pos leftward[PL][4];
int lefttime[PL][4], leftnow = 0, leftmir[PL][4];

void testinit(int size) {
	f(i,size) f(j,4) lefttime[i][j] = -1;
}

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
			
			if (num1 == -1 && p1.move(7)) return false;
			if (num2 == -1 && p2.move(7)) return false;
			
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
	f(iter, 2) {	// just in case
		pos orig1 = p1, orig2 = p2;
		if (p1.follow_wall() == -1 && p1.move(7)) return false;
		if (p2.follow_wall() == -1 && p2.move(7)) return false;
		p1 = orig2, p2 = orig1;
		p1.rotate(4), p2.rotate(4);
		p1.reflect(), p2.reflect();
	}
	return true;
}
