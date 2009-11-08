#include "maze.h"

void remember(int dis, int MEM) {
	pos mem[MEM][MEM];
	bool visited[MEM][MEM];
	int dist[MEM][MEM];
	
	f(i,MEM) f(j,MEM) {
		mem[i][j].grid = -2;
		visited[i][j] = false;
		if (i*j*(MEM-i-1)*(MEM-j-1) == 0) visited[i][j] = true;
		dist[i][j] = MEM*MEM;
	}
	int st = 0, en = 1, qx[MEM*MEM], qy[MEM*MEM];
	qx[0] = qy[0] = MEM/2;
	mem[qx[0]][qy[0]] = player;
	visited[qx[0]][qy[0]] = true;
	dist[qx[0]][qy[0]] = 0;
	
	while (st < en) {
		f(dir,8) if (dist[qx[st]+adx[dir]][qy[st]+ady[dir]] < dist[qx[st]][qy[st]] && mem[qx[st]+adx[dir]][qy[st]+ady[dir]].grid == -3) {
			mem[qx[st]][qy[st]].grid = -3;
		}
		if (mem[qx[st]][qy[st]].grid >= 0 && seen[mem[qx[st]][qy[st]].grid]) {
			f(dir,8) if (!visited[qx[st]+adx[dir]][qy[st]+ady[dir]]) {
				qx[en] = qx[st]+adx[dir], qy[en] = qy[st]+ady[dir];
				pos tmp = mem[qx[st]][qy[st]], tmp2 = mem[qx[en]][qy[en]];
				if(!tmp.move(dir)) tmp.grid = -1, tmp.rot = 0, tmp.mir = 1;
				if (mem[qx[en]][qy[en]].grid == -2) {
					mem[qx[en]][qy[en]] = tmp;
					dist[qx[en]][qy[en]] = dist[qx[st]][qy[st]]+1;
					++en;
				} else if ((tmp2.grid != tmp.grid || tmp2.rot != tmp.rot || tmp2.mir != tmp.mir) && dist[qx[en]][qy[en]] > dist[qx[st]][qy[st]]) {
					mem[qx[st]+adx[dir]][qy[st]+ady[dir]].grid = -3;
				}
			}
		}
		visited[qx[st]][qy[st]] = true;
		++st;
	}
	
	for (int i = qx[0]-dis; i <= qx[0]+dis; ++i) for (int j = qy[0]-dis; j <= qy[0]+dis; ++j) {
		if (mem[i][j].grid > -2) mem_draw(mem[i][j].grid, i-qx[0]+centerx, j-qy[0]+centery);
	}
}
