/* 
 * Under The MIT License (MIT)
 * the following program is
 * Copyright (c) 2014 Carson Shook
 *
 * for a full list of terms and conditionas,
 * please view the included LICENSE file.
 */

#include <stdio.h>
#include <libc.h>
#include <ncurses.h>
#include <string.h>

int main() {
    int height = 24;
    int width = 82;
	int brickRows = 2;
	int brickColumns = 20;
	int bricks[brickRows][brickColumns];
    int i, j, n, innerLength, ch, paddlePosition, paddleLength = 16;
    float ballX, ballY;
    int ballTick = 0, ballDelay = 10000;
    float velocityX = -1, velocityY = -1;
    int didUpdate = 1;
    
    paddlePosition = (width/2) - (paddleLength/2);
    ballX = (width/2) - (paddleLength/2);
    ballY = height - 5;
    
    // Set all bricks as active by iteratively
    // initializing all values to 1
    for (i = 0; i < brickRows; i++) {
        for (j = 0; j < brickColumns; j++) {
            bricks[i][j] = 1;
        }
    }
    
    /* Old code for resizing the game.
     Keeping it as a comment just in case I ever do anything with it.
     
     if (cycle == 2000) {
     struct winsize ws;
     ioctl(0, TIOCGWINSZ, &ws);
     resizeterm(ws.ws_row, (ws.ws_col/brickColumns)*brickColumns);
     clear();
     cycle = 0;
     mvprintw(5,0,"%d by %d", COLS, LINES);
     }
     else {
     cycle++;
     }
     */
    
    // Sets window properties
	initscr();
    stdscr = newwin(height, width, (LINES - height)/2, (COLS - width)/2);
	wborder(stdscr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    innerLength = width/brickColumns;
	while ((ch = getch()) != 113) {
		
        /* Detect left or right arrow key press */
        if (ch != ERR) {
            didUpdate = 1;
            switch (ch) {
                case KEY_LEFT:
                    if (paddlePosition > 1) {
                        paddlePosition--;
                        mvwaddch(stdscr, height - 2, paddlePosition + paddleLength + 1 , ' ');
                    }
                    break;
                case KEY_RIGHT:
                    if (paddlePosition < (width - paddleLength - 2)) {
                        paddlePosition++;
                        mvwaddch(stdscr, height - 2, paddlePosition - 1 , ' ');
                    }
                    break;
            }
        }

        /* Update the paddle accordingly if key is pressed */
        if (didUpdate == 1) {
            for (i = 0; i <= paddleLength; i++) {
                mvwaddch(stdscr, height - 2, paddlePosition + i, '#');
            }
        }
        
        /* Ball */
        if (ballTick < ballDelay) {
            ballTick++;
        }
        else {
            didUpdate = 1;
            mvwaddch(stdscr, ballY, ballX , ' ');
            ballX = ballX + velocityX;
            ballY = ballY + velocityY;
            mvwaddch(stdscr, ballY, ballX , 'o');
            if ((mvwinch(stdscr, (int)ballY, (int)ballX + 1) == ACS_VLINE) || (mvwinch(stdscr, (int)ballY, (int)ballX - 1) == ACS_VLINE)) {
                velocityX = velocityX * -1;
            }
            if ((mvwinch(stdscr, (int)ballY + 1, (int)ballX) == ACS_HLINE) || (mvwinch(stdscr, (int)ballY - 1, (int)ballX) == ACS_HLINE)) {
                velocityY = velocityY * -1;
            }
            if (mvwinch(stdscr, (int)ballY + 1, (int)ballX) == '#') {
                velocityY = velocityY * -1;
            }
            ballTick = 0;
        }
        
        /* Update the bricks accordingly if an update occurs */
        if (didUpdate == 1) {
            for (i = 0; i < brickRows; i++) {
                for (j = 0; j < brickColumns; j++) {
                    if (bricks[i][j] == 1) {
                        for (n = 1; n < innerLength - 1; n++) {
                            mvwaddch(stdscr,(i*2)+1,(j*(innerLength))+n+1,ACS_HLINE);
                            mvwaddch(stdscr,(i*2)+2,(j*(innerLength))+n+1,ACS_HLINE);
                        }
                        mvwaddch(stdscr,(i*2)+1,j*(innerLength)+1,ACS_ULCORNER);
                        mvwaddch(stdscr,(i*2)+2,j*(innerLength)+1,ACS_LLCORNER);
                        mvwaddch(stdscr,(i*2)+1,(j*(innerLength))+n+1,ACS_URCORNER);
                        mvwaddch(stdscr,(i*2)+2,(j*(innerLength))+n+1,ACS_LRCORNER);
                    }
                    else {
                        for (n = 1; n < innerLength - 1; n++) {
                            mvwaddch(stdscr,(i*2)+1,(j*(innerLength))+n+1,' ');
                            mvwaddch(stdscr,(i*2)+2,(j*(innerLength))+n+1,' ');
                        }
                        mvwaddch(stdscr,(i*2)+1,j*(innerLength)+1,' ');
                        mvwaddch(stdscr,(i*2)+2,j*(innerLength)+1,' ');
                        mvwaddch(stdscr,(i*2)+1,(j*(innerLength))+n+1,' ');
                        mvwaddch(stdscr,(i*2)+2,(j*(innerLength))+n+1,' ');
                    }
                }
            }
        }
        /* Update the screen, and reset the didUpdate variable */
		wrefresh(stdscr);
        didUpdate = 0;
	}
	endwin();

	return 0;
}