#include "maze.h"

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
