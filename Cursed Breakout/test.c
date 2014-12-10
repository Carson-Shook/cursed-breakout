#include <stdio.h>
#include <ncurses.h>
#include <signal.h>
#include <sys/ioctl.h>

void resize() {
	struct winsize ws;
	ioctl(0, TIOCGWINSZ, &ws);
	resizeterm(ws.ws_row, ws.ws_col);
	endwin();
	clear();
	return;
}

int main() {
	int brickRows = 2;
	int brickColumns = 20;
	int bricks[brickRows][brickColumns];
	int i, j, n;
	curs_set(0);
	
	/* Set all bricks as active */
	for (i = 0; i < brickRows; i++) {
		for (j = 0; j < brickColumns; j++) {
			bricks[i][j] = 1;
		}
	}
	signal(SIGWINCH, resize);
	initscr();					/* Start curses mode 		  */
	while (1) {
		for (i = 0; i < brickRows; i++) {
			for (j = 0; j < brickColumns; j++) {
				if (bricks[i][j] == 1) {
					for (n = 1; n < COLS/brickColumns - 1; n++) {
						mvaddch(i*2,(j*(COLS/brickColumns))+n,ACS_HLINE);
						mvaddch((i*2)+1,(j*(COLS/brickColumns))+n,ACS_HLINE);
					}
					mvaddch(i*2,j*(COLS/brickColumns),ACS_ULCORNER);
					mvaddch((i*2)+1,j*(COLS/brickColumns),ACS_LLCORNER);
					mvaddch(i*2,(j*(COLS/brickColumns))+n,ACS_URCORNER);
					mvaddch((i*2)+1,(j*(COLS/brickColumns))+n,ACS_LRCORNER);
				}
				else {
					mvaddch(i*2,j*5,' ');
					mvaddch(i*2,(j*5)+1,' ');
					mvaddch(i*2,(j*5)+2,' ');
					mvaddch(i*2,(j*5)+3,' ');
					mvaddch(i*2,(j*5)+4,' ');
					mvaddch((i*2)+1,j*5,' ');
					mvaddch((i*2)+1,(j*5)+1,' ');
					mvaddch((i*2)+1,(j*5)+2,' ');
					mvaddch((i*2)+1,(j*5)+3,' ');
					mvaddch((i*2)+1,(j*5)+4,' ');
				}
			}
		}
		refresh();					/* Print it on to the real screen */
		timeout(500);
	}
	endwin();					/* End curses mode		  */

	return 0;
}