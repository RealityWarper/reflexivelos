#include "maze.h"

void save() {
	FILE *fout = fopen("save.txt", "w");
	if (fout != NULL) {
		fprintf(fout, "%d %d\n", player.grid, rspot);
		int tmp = player.grid;
		f(g,pl) if (seen[g]) {
			fprintf(fout, "\n3 3\n");
			pos p[4][4];	// row, column
			f(i,4) f(j,4) p[i][j].grid = -1, p[i][j].rot = 0, p[i][j].mir = 1;
			p[1][1].grid = g;
			f(x,3) f(y,3) f(dir,8) if (x+adx[dir] >= 0 && y+ady[dir] >= 0 && p[y+ady[dir]][x+adx[dir]].grid >= 0 && p[y][x].grid == -1) {
				p[y][x].grid = p[y+ady[dir]][x+adx[dir]].grid;
				p[y][x].rot = p[y+ady[dir]][x+adx[dir]].rot;
				p[y][x].mir = p[y+ady[dir]][x+adx[dir]].mir;
				if (!p[y][x].move(dir+4)) p[y][x].grid = -1, p[y][x].rot = 0, p[y][x].mir = 1;
				break;
			}
			f(y,3) {
				f(x,3) {
					if (p[y][x].rot != 0 || p[y][x].mir != 1)
						fprintf(fout, "%d%c\t", p[y][x].grid, 'a' + ((char) p[y][x].rot+((1-p[y][x].mir)/2)));
					else fprintf(fout, "%d\t", p[y][x].grid);
				}
				fprintf(fout, "\n");
			}
		}
		player.grid = tmp;
		fprintf(fout, "\n0 0\n");
	} else return;
	fclose(fout);
}

