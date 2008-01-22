#include "maze.h"

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

bool getmap(string filepath) {
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
