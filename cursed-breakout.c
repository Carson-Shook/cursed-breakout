
 /******************************** cursed-breakout *********************************
 *                                                                                 *
 * The MIT License (MIT)                                                           *
 *                                                                                 *
 * Copyright (c) 2015 Carson Shook                                                 *
 *                                                                                 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy    *
 * of this software and associated documentation files (the "Software"), to deal   *
 * in the Software without restriction, including without limitation the rights    *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
 * copies of the Software, and to permit persons to whom the Software is           *
 * furnished to do so, subject to the following conditions:                        *
 *                                                                                 *
 * The above copyright notice and this permission notice shall be included in all  *
 * copies or substantial portions of the Software.                                 *
 *                                                                                 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
 * SOFTWARE.                                                                       *
 *                                                                                 *
 **********************************************************************************/

#include <locale.h>
#include <stdio.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>

/* Saving this for later
 #include <signal.h>
 void* resizeHandler(int);
 
 int main(void) {
 signal(SIGWINCH, resizeHandler);
 }
 
 void* resizeHandler(int sig)
 {
 int nh, nw;
 getmaxyx(stdscr, nh, nw);
}
*/

/* collider
 * Probably takes way too may variables for its own good,
 * and I'm planning on looking into whether or not making
 * some of the variables public to reduce the need to pass
 * so many.
 * This function acts a bit like a finite-state machine to
 * manage the collision system. There is no ball reset at
 * the moment, but that will come in the next update. Also,
 * I may just take this out of this seperate function entirely.
 * It examines the individual characters immediately around
 * the ball and determines how to react based upon the type
 * of characters next to it. If the character is part of a 
 * brick, then the location in the array is calculated by
 * reversing the calculation used to place it in the brick
 * update section of the main program, and setting that array
 * value to zero. It also flips the appropriate velocity.
 *
 * To be honest, there's probably at least one or two bugs
 * still hiding in here, so I'll continue to review it.
 */
int collider(int *character, int X, int Y, int *velX, int *velY, int (*brickArray)[20], int brickL, int h, int w) {
    int collisionCase = 0;
    
    while (1) {
        switch (collisionCase) {
                
            case 0:
                // top
                if (character[1] != ' ') {
                    if (character[1] == ACS_LLCORNER) {
                        brickArray[(((Y-1)-2)/2)][(X-1)/brickL] = 0;
                        (* velY) = (* velY) * -1;
                        collisionCase = 8;
                    }
                    else if (character[1] == ACS_LRCORNER) {
                        brickArray[(((Y-1)-2)/2)][((X-1)-brickL)/brickL] = 0;
                        (* velY) = (* velY) * -1;
                        collisionCase = 8;
                    }
                    else if (character[1] == ACS_HLINE) {
                        if (Y == 1) {
                            (* velY) = (* velY) * -1;
                            collisionCase = 8;
                        }
                        else {
                            brickArray[(((Y-1)-2)/2)][(X-(X%brickL))/brickL] = 0;
                            (* velY) = (* velY) * -1;
                            collisionCase = 999;
                        }
                    }
                    else collisionCase = 1;
                }
                else collisionCase = 1;
                break;
            case 1:
                // bottom
                if (character[6] != ' ') {
                    if (character[6] == ACS_ULCORNER) {
                        brickArray[(((Y+1)-1)/2)][(X-1)/brickL] = 0;
                        (* velY) = (* velY) * -1;
                        collisionCase = 8;
                    }
                    else if (character[6] == ACS_URCORNER) {
                        brickArray[(((Y+1)-1)/2)][((X-1)-brickL)/brickL] = 0;
                        (* velY) = (* velY) * -1;
                        collisionCase = 8;
                    }
                    else if (character[6] == ACS_HLINE) {
                        if (Y + 2 == h) {
                            (* velY) = (* velY) * -1;
                            collisionCase = 8;
                        }
                        else {
                            brickArray[(((Y+1)-1)/2)][(X-(X%brickL))/brickL] = 0;
                            (* velY) = (* velY) * -1;
                            collisionCase = 999;
                        }
                    }
                    else collisionCase = 2;
                }
                else collisionCase = 2;
                break;
            case 2:
                // left
                if (character[3] != ' ') {
                    if (character[3] == ACS_URCORNER) {
                        brickArray[(Y-1)/2][((X-1)-brickL)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[3] == ACS_LRCORNER) {
                        brickArray[(Y-2)/2][((X-1)-brickL)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[3] == ACS_VLINE) {
                        if (X == 1) {
                            (* velX) = (* velX) * -1;
                            collisionCase = 999;
                        }
                    }
                    else collisionCase = 3;
                }
                else collisionCase = 3;
                break;
            case 3:
                // right
                if (character[4] != ' ') {
                    if (character[4] == ACS_ULCORNER) {
                        brickArray[(Y-1)/2][((X+1)-1)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[4] == ACS_LLCORNER) {
                        brickArray[(Y-2)/2][((X+1)-1)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[4] == ACS_VLINE) {
                        if (X + 2 == w) {
                            (* velX) = (* velX) * -1;
                            collisionCase = 999;
                        }
                    }
                    else collisionCase = 4;
                }
                else collisionCase = 4;
                break;
            case 4:
                // top left
                if ((character[0] == ACS_LRCORNER) && ((* velX) < 0) && ((* velY) < 0)) {
                    brickArray[((Y-1)-2)/2][((X-1)-brickL)/brickL] = 0;
                    (* velX) = (* velX) * -1;
                    (* velY) = (* velY) * -1;
                    collisionCase = 999;
                }
                else collisionCase = 5;
                break;
            case 5:
                // top right
                if (character[2] == ACS_LLCORNER && ((* velX) > 0) && ((* velY) < 0)) {
                    brickArray[((Y-1)-2)/2][((X+1)-1)/brickL] = 0;
                    (* velX) = (* velX) * -1;
                    (* velY) = (* velY) * -1;
                    collisionCase = 999;
                }
                else collisionCase = 6;
                break;
            case 6:
                // lower left
                if (character[5] == ACS_URCORNER && ((* velX) < 0) && ((* velY) > 0)) {
                    brickArray[((Y+1)-1)/2][((X-1)-brickL)/brickL] = 0;
                    (* velX) = (* velX) * -1;
                    (* velY) = (* velY) * -1;
                    collisionCase = 999;
                }
                else collisionCase = 999;
                break;
            case 7:
                // lower right
                if (character[7] == ACS_ULCORNER && ((* velX) > 0) && ((* velY) > 0)) {
                    brickArray[((Y+1)-1)/2][((X+1)-1)/brickL] = 0;
                    (* velX) = (* velX) * -1;
                    (* velY) = (* velY) * -1;
                    collisionCase = 999;
                }
                else collisionCase = 999;
                break;
            case 8:
                // left if top or bottom
                if (character[3] != ' ') {
                    if (character[3] == ACS_URCORNER) {
                        brickArray[(Y-1)/2][((X-1)-brickL)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[3] == ACS_LRCORNER) {
                        brickArray[(Y-2)/2][((X-1)-brickL)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[3] == ACS_VLINE) {
                        if (X == 1) {
                            (* velX) = (* velX) * -1;
                            collisionCase = 999;
                        }
                    }
                    else collisionCase = 9;
                }
                else collisionCase = 9;
                break;
            case 9:
                // right if top or bottom
                if (character[4] != ' ') {
                    if (character[4] == ACS_ULCORNER) {
                        brickArray[(Y-1)/2][((X+1)-1)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[4] == ACS_LLCORNER) {
                        brickArray[(Y-2)/2][((X+1)-1)/brickL] = 0;
                        (* velX) = (* velX) * -1;
                        collisionCase = 999;
                    }
                    else if (character[4] == ACS_VLINE) {
                        if (X + 2 == w) {
                            (* velX) = (* velX) * -1;
                            collisionCase = 999;
                        }
                    }
                    else collisionCase = 999;
                }
                else collisionCase = 999;
                break;
            case 999:
                return 1;
                break;
            default:
                return 1;
                break;
        }
    }
}

int main(void) {
    
    setlocale(LC_ALL, "");
    
    int height = 24;
    int width = 82;
	int brickRows = 2;
	int brickColumns = 20;
	int bricks[brickRows][brickColumns];
    int aroundBall[8];
    int aroundballX, aroundBallY;
    int i, j, n, brickLength, ch, paddlePosition, paddleLength = 16;
    int ballX, ballY;
    int ballTick = 0, ballDelay = 10000;
    int velocityX = -1, velocityY = -1;
    int didUpdate = 1;
    clock_t timer;
    double seconds = CLOCKS_PER_SEC/10;
    WINDOW *gamescr;
    
    struct winsize max;
    ioctl(0, TIOCGWINSZ , &max);
    
#ifdef DEBUG
    WINDOW *local_win;
#endif
    
    // Check terminal size before starting
    if ((max.ws_row < height) || (max.ws_col < width)) {
        printf("The terminal is too small. :(\nPlease resize it to at least %d by %d, and try again.\n\n", width, height);
        return(1);
    }
    
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
    // Sets window properties
	initscr();
    gamescr = newwin(height, width, (LINES - height)/2, (COLS - width)/2);
	wborder(gamescr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    //NCURSES_NO_UTF8_ACS = 1;
    brickLength = width/brickColumns;
#ifdef DEBUG
    local_win = newwin(4, 20, 0, 0);
#endif
    timer = clock();
    /* Begin infinite loop */
	while ((ch = getch()) != 113) {
		
        /* Detect left or right arrow key press */
        if (ch != ERR) {
            didUpdate = 1;
            switch (ch) {
                case KEY_LEFT:
                    if (paddlePosition > 1) {
                        paddlePosition--;
                        mvwaddch(gamescr, height - 2, paddlePosition + paddleLength + 1 , ' ');
                    }
                    break;
                case KEY_RIGHT:
                    if (paddlePosition < (width - paddleLength - 2)) {
                        paddlePosition++;
                        mvwaddch(gamescr, height - 2, paddlePosition - 1 , ' ');
                    }
                    break;
            }
        }

        /* Update the paddle accordingly if key is pressed */
        if (didUpdate == 1) {
            for (i = 0; i <= paddleLength; i++) {
                mvwaddch(gamescr, height - 2, paddlePosition + i, '#');
            }
        }
        
        /* Check to see if the apropriate amount of time has
         * passed, and update the ball postion if so.
         */
        if (difftime(clock(),timer) >= seconds) {
            didUpdate = 1;
            
            aroundBall[0] = mvwinch(gamescr, (int)ballY - 1, (int)ballX - 1);
            aroundBall[1] = mvwinch(gamescr, (int)ballY - 1, (int)ballX);
            aroundBall[2] = mvwinch(gamescr, (int)ballY - 1, (int)ballX + 1);
            aroundBall[3] = mvwinch(gamescr, (int)ballY, (int)ballX - 1);
            aroundBall[4] = mvwinch(gamescr, (int)ballY, (int)ballX + 1);
            aroundBall[5] = mvwinch(gamescr, (int)ballY + 1, (int)ballX - 1);
            aroundBall[6] = mvwinch(gamescr, (int)ballY + 1, (int)ballX);
            aroundBall[7] = mvwinch(gamescr, (int)ballY + 1, (int)ballX + 1);
#ifdef DEBUG
            mvwaddch(local_win, 0, 0, aroundBall[0]);
            mvwaddch(local_win, 0, 1, aroundBall[1]);
            mvwaddch(local_win, 0, 2, aroundBall[2]);
            mvwaddch(local_win, 1, 0, aroundBall[3]);
            mvwaddch(local_win, 1, 2, aroundBall[4]);
            mvwaddch(local_win, 2, 0, aroundBall[5]);
            mvwaddch(local_win, 2, 1, aroundBall[6]);
            mvwaddch(local_win, 2, 2, aroundBall[7]);
            mvwprintw(local_win, 0, 4, "%d %d ", ballY, ballX);
#endif
            if ((aroundBall[0] != ' ')||(aroundBall[1] != ' ')||(aroundBall[2] != ' ')||(aroundBall[3] != ' ')||(aroundBall[4] != ' ')||(aroundBall[5] != ' ')||(aroundBall[6] != ' ')||(aroundBall[7] != ' ')) {
                if (aroundBall[6] == '#') {
                    velocityY = velocityY * -1;
                }
                
                didUpdate = collider(aroundBall, ballX, ballY, &velocityX, &velocityY, bricks, brickLength, height, width);
            }
            mvwaddch(gamescr, ballY, ballX , ' ');
            ballX = ballX + velocityX;
            ballY = ballY + velocityY;
            
            timer = clock();
        }
        
        
        /* Update the bricks accordingly if an update occurs */
        if (didUpdate == 1) {
            for (i = 0; i < brickRows; i++) {
                for (j = 0; j < brickColumns; j++) {
                    if (bricks[i][j] == 1) {
                        for (n = 1; n < brickLength - 1; n++) {
                            mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,ACS_HLINE);
                            mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,ACS_HLINE);
                        }
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+1,ACS_ULCORNER);
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+1,ACS_LLCORNER);
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,ACS_URCORNER);
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,ACS_LRCORNER);
                    }
                    else {
                        for (n = 1; n < brickLength - 1; n++) {
                            mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,' ');
                            mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,' ');
                        }
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+1,' ');
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+1,' ');
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,' ');
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,' ');
                    }
                }
            }
            mvwaddch(gamescr, ballY, ballX , 'o');
        }
        /* Update the screen, and reset the didUpdate variable */
        wrefresh(gamescr);
#ifdef DEBUG
        wrefresh(local_win);
#endif
        didUpdate = 0;
	}
	endwin();

	return 0;
}