
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
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

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

int increaseMult(int);

int main(void) {
    
    setlocale(LC_ALL, "");
   
    int height = 24;                        // game screen height
    int width = 82;                         // game screen width
    int scoreWinHeight = 2;                 // height of the score window
    int brickRows = 3;                      // rows of bricks to generate
    int brickColumns = 20;                  // collumns of bricks to generate
    int bricks[brickRows][brickColumns];    // array to hold the locations of each brick
    int brickLength;                        // how long the bricks should be
    
    int i, j, n;                            // Temporary for-loop variables
    int ch;                                 // Stores keypresses
    
    int paddlePosition;                     // Location of the paddle's leftmost point
    int paddleLength = 16;                  // Length of the paddle
    
    int ballX, ballY;                       // ball X and Y coordinates
    int velocityX = -1, velocityY = -1;     // ball X and Y velocity
    int aroundBall[8];                      // array to hold the
    int collisionCase;                      // contains the current collision case for determining ball action
  /*float velDiff                           difference in velocity, intended for changing the ball speed
                                             but there are some... issues with that since ncurses is based
                                             on an integer array. See below for details*/
    
    int didUpdate = 1;                      // flag to trip if anything changes onscreen
    int didDie = 1;                         // flag to trigger death sequence actions
    double seconds = CLOCKS_PER_SEC/12;     // seconds until the ball moves. May add separate X and Y in the future
    clock_t timer;                          // structure that holds the time since the last ball update
    
    unsigned int playerScore = 0;           // the player's score
    int scoreMultiplier = 1;                // the current score multiplier
    int ballsRemaining = 3;                 // number of lives the player has
    
    WINDOW *gamescr;                        // the game screen window
    WINDOW *score_win;                      // the score and life-count window
    WINDOW *gameOverWin;
#ifdef DEBUG
    WINDOW *debug_win;                      // debug window (I should hope that to be obvious enough)
#endif
    
    struct winsize max;                     // max size of the terminal
    ioctl(0, TIOCGWINSZ , &max);            // gets the max size of the terminal
    
#ifndef DEBUG
    // Check terminal size before starting
    if ((max.ws_row < (height + scoreWinHeight)) || (max.ws_col < width)) {
        printf("The terminal is too small. :(\nPlease resize it to at least %d by %d, and try again.\n\n", width, (height + scoreWinHeight));
        return(1);
    }
#endif
    
    // set initial paddle position and ball location
    paddlePosition = (width/2) - (paddleLength/2);
    ballX = (width/2);
    ballY = height - 3;
    
    // Set all brick length, and set all
    // bricks as active by iteratively
    // initializing all values to 1
    brickLength = (width - 2)/brickColumns;
    for (i = 0; i < brickRows; i++) {
        for (j = 0; j < brickColumns; j++) {
            bricks[i][j] = 1;
        }
    }
    
    // enters curses mode and sets window properties
    initscr();
    gamescr = newwin(height, width, ((LINES - height) / 2) - (scoreWinHeight / 2), (COLS - width) / 2);
    score_win = newwin(scoreWinHeight, width, ((LINES - height) / 2) + height - (scoreWinHeight / 2), (COLS - width) / 2);
#ifdef DEBUG
    debug_win = newwin(4, 50, 0, 0);
#endif
    wborder(gamescr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    timer = clock();
    
    // Begin infinite loop, press q to quit
    while ((ch = getch()) != 'q') {
        
        // Detect left or right arrow key press, as well as up or down
        if (ch != ERR) {
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
                case KEY_DOWN:
                    if (paddlePosition > 2) {
                        paddlePosition = paddlePosition - 2;
                        mvwprintw(gamescr, height - 2, paddlePosition + paddleLength + 1 , "  ");
                    }
                    break;
                case KEY_UP:
                    if (paddlePosition < (width - paddleLength - 3)) {
                        paddlePosition = paddlePosition + 2;
                        mvwprintw(gamescr, height - 2, paddlePosition - 2 , "  ");
                    }
                    break;
            }
            didUpdate = 1;
        }
        
         // Check to see if the apropriate amount of time has
         // passed, and update the ball postion if so.
        if (difftime(clock(),timer) >= seconds) {
            timer = clock();
            
            mvwaddch(gamescr, ballY, ballX , ' ');
            ballX = ballX + velocityX;
            ballY = ballY + velocityY;
            
            aroundBall[0] = mvwinch(gamescr, (int)ballY - 1, (int)ballX - 1);
            aroundBall[1] = mvwinch(gamescr, (int)ballY - 1, (int)ballX);
            aroundBall[2] = mvwinch(gamescr, (int)ballY - 1, (int)ballX + 1);
            aroundBall[3] = mvwinch(gamescr, (int)ballY, (int)ballX - 1);
            aroundBall[4] = mvwinch(gamescr, (int)ballY, (int)ballX + 1);
            aroundBall[5] = mvwinch(gamescr, (int)ballY + 1, (int)ballX - 1);
            aroundBall[6] = mvwinch(gamescr, (int)ballY + 1, (int)ballX);
            aroundBall[7] = mvwinch(gamescr, (int)ballY + 1, (int)ballX + 1);
#ifdef DEBUG
            mvwaddch(debug_win, 0, 0, aroundBall[0]);
            mvwaddch(debug_win, 0, 1, aroundBall[1]);
            mvwaddch(debug_win, 0, 2, aroundBall[2]);
            mvwaddch(debug_win, 1, 0, aroundBall[3]);
            mvwaddch(debug_win, 1, 2, aroundBall[4]);
            mvwaddch(debug_win, 2, 0, aroundBall[5]);
            mvwaddch(debug_win, 2, 1, aroundBall[6]);
            mvwaddch(debug_win, 2, 2, aroundBall[7]);
            mvwprintw(debug_win, 0, 4, "%d %d ", ballY, ballX);
#endif
            // only bothers with this code if the ball is near something
            if ((aroundBall[0] != ' ')||(aroundBall[1] != ' ')||(aroundBall[2] != ' ')||(aroundBall[3] != ' ')||(aroundBall[4] != ' ')||(aroundBall[5] != ' ')||(aroundBall[6] != ' ')||(aroundBall[7] != ' ')) {
                
                if (aroundBall[6] == '#') {
                    
                    velocityY = velocityY * -1;
                    
                    /*
                     // Calcuates a velocity change based on the way that the
                     // ball hits the paddle. Works in theory, not in practice
                     // since moving too fast causes the ball to miss important
                     // collision checks. Will revisit in the future.
                     
                     velDiff = ((paddlePosition + (paddleLength / 2)) - ballX) * 0.10;
                     #ifdef DEBUG
                     mvwprintw(debug_win, 0, 20, "%.2f ", velDiff);
                     #endif
                     velocityX = velocityX - velDiff;
                     velocityY = velocityY + velDiff;
                     */
                }
                
                /* collider
                 * acts a bit like a finite-state machine to
                 * manage the collision system. It examines the individual
                 * characters immediately around
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
                
                collisionCase = 0;
                
                while (collisionCase != 999) {
                    switch (collisionCase) {
                            
                        case 0:
                            // top
                            if (aroundBall[1] != ' ') {
                                if (aroundBall[1] == ACS_LLCORNER) {
                                    bricks[(((ballY-1)-2)/2)][(ballX-1)/brickLength] = 0;
                                    velocityY = velocityY * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[1] == ACS_LRCORNER) {
                                    bricks[(((ballY-1)-2)/2)][((ballX-1)-(brickLength-1))/brickLength] = 0;
                                    velocityY = velocityY * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[1] == ACS_HLINE) {
                                    if (ballY == 1) {
                                        velocityY = velocityY * -1;
                                        collisionCase = 8;
                                    }
                                    else {
                                        bricks[(((ballY-1)-2)/2)][(ballX-(ballX%(brickLength-1)))/brickLength] = 0;
                                        velocityY = velocityY * -1;
                                        playerScore = playerScore + scoreMultiplier;
                                        scoreMultiplier = increaseMult(scoreMultiplier);
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 1;
                            }
                            else collisionCase = 1;
                            break;
                        case 1:
                            // bottom
                            if (aroundBall[6] != ' ') {
                                if (aroundBall[6] == ACS_ULCORNER) {
                                    bricks[(((ballY+1)-1)/2)][(ballX-1)/brickLength] = 0;
                                    velocityY = velocityY * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[6] == ACS_URCORNER) {
                                    bricks[(((ballY+1)-1)/2)][((ballX-1)-(brickLength-1))/brickLength] = 0;
                                    velocityY = velocityY * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[6] == ACS_HLINE) {
                                    // this is the death state
                                    if ((ballY) + 2 == height) {
                                        ballsRemaining--;
                                        velocityY = velocityY * -1;
                                        paddlePosition = (width/2) - (paddleLength/2);
                                        ballX = width / 2;
                                        ballY = height - 3;
                                        scoreMultiplier = 1;
                                        didDie = 1;
                                        wclear(gamescr);
                                        wborder(gamescr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
                                        collisionCase = 999;
                                    }
                                    else {
                                        bricks[(((ballY+1)-1)/2)][(ballX-(ballX%(brickLength-1)))/brickLength] = 0;
                                        velocityY = velocityY * -1;
                                        playerScore = playerScore + scoreMultiplier;
                                        scoreMultiplier = increaseMult(scoreMultiplier);
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 2;
                            }
                            else collisionCase = 2;
                            break;
                        case 2:
                            // left
                            if (aroundBall[3] != ' ') {
                                if (aroundBall[3] == ACS_URCORNER) {
                                    bricks[(ballY-1)/2][((ballX-1)-(brickLength-1))/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_LRCORNER) {
                                    bricks[(ballY-2)/2][((ballX-1)-(brickLength-1))/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_VLINE) {
                                    if (ballX == 1) {
                                        velocityX = velocityX * -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 3;
                            }
                            else collisionCase = 3;
                            break;
                        case 3:
                            // right
                            if (aroundBall[4] != ' ') {
                                if (aroundBall[4] == ACS_ULCORNER) {
                                    bricks[(ballY-1)/2][((ballX+1)-1)/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_LLCORNER) {
                                    bricks[(ballY-2)/2][((ballX+1)-1)/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_VLINE) {
                                    if (ballX + 2 == width) {
                                        velocityX = velocityX * -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 4;
                            }
                            else collisionCase = 4;
                            break;
                        case 4:
                            // top left
                            if ((aroundBall[0] == ACS_LRCORNER) && (velocityX < 0) && (velocityY < 0)) {
                                bricks[((ballY-1)-2)/2][((ballX-1)-(brickLength-1))/brickLength] = 0;
                                velocityX = velocityX * -1;
                                velocityY = velocityY * -1;
                                playerScore = playerScore + scoreMultiplier;
                                scoreMultiplier = increaseMult(scoreMultiplier);
                                collisionCase = 999;
                            }
                            else collisionCase = 5;
                            break;
                        case 5:
                            // top right
                            if ((aroundBall[2] == ACS_LLCORNER) && (velocityX > 0) && (velocityY < 0)) {
                                bricks[((ballY-1)-2)/2][((ballX+1)-1)/brickLength] = 0;
                                velocityX = velocityX * -1;
                                velocityY = velocityY * -1;
                                playerScore = playerScore + scoreMultiplier;
                                scoreMultiplier = increaseMult(scoreMultiplier);
                                collisionCase = 999;
                            }
                            else collisionCase = 6;
                            break;
                        case 6:
                            // lower left
                            if ((aroundBall[5] == ACS_URCORNER) && (velocityX < 0) && (velocityY > 0)) {
                                bricks[((ballY+1)-1)/2][((ballX-1)-(brickLength-1))/brickLength] = 0;
                                velocityX = velocityX * -1;
                                velocityY = velocityY * -1;
                                playerScore = playerScore + scoreMultiplier;
                                scoreMultiplier = increaseMult(scoreMultiplier);
                                collisionCase = 999;
                            }
                            else collisionCase = 7;
                            break;
                        case 7:
                            // lower right
                            if ((aroundBall[7] == ACS_ULCORNER) && (velocityX > 0) && (velocityY > 0)) {
                                bricks[((ballY+1)-1)/2][((ballX+1)-1)/brickLength] = 0;
                                velocityX = velocityX * -1;
                                velocityY = velocityY * -1;
                                playerScore = playerScore + scoreMultiplier;
                                scoreMultiplier = increaseMult(scoreMultiplier);
                                collisionCase = 999;
                            }
                            else collisionCase = 999;
                            break;
                        case 8:
                            // left if top or bottom
                            if (aroundBall[3] != ' ') {
                                if (aroundBall[3] == ACS_URCORNER) {
                                    bricks[(ballY-1)/2][((ballX-1)-brickLength)/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_LRCORNER) {
                                    bricks[(ballY-2)/2][((ballX-1)-brickLength)/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_VLINE) {
                                    if (ballX == 1) {
                                        velocityX = velocityX * -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 9;
                            }
                            else collisionCase = 9;
                            break;
                        case 9:
                            // right if top or bottom
                            if (aroundBall[4] != ' ') {
                                if (aroundBall[4] == ACS_ULCORNER) {
                                    bricks[(ballY-1)/2][((ballX+1)-1)/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_LLCORNER) {
                                    bricks[(ballY-2)/2][((ballX+1)-1)/brickLength] = 0;
                                    velocityX = velocityX * -1;
                                    playerScore = playerScore + scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_VLINE) {
                                    if (ballX + 2 == width) {
                                        velocityX = velocityX * -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                            }
                            else collisionCase = 999;
                            break;
                        default:
                            collisionCase = 999;
                            break;
                    }
                }
            }
            didUpdate = 1;
        }
        
        // Update the bricks if an update occurs
        if (didUpdate) {
            for (i = 0; i < brickRows; i++) {
                for (j = 0; j < brickColumns; j++) {
                    if (bricks[i][j] == 1) {
                        // Left side of brick
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+1,ACS_ULCORNER);
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+1,ACS_LLCORNER);
                        // Middle portions of brick, variable length just in case
                        for (n = 1; n < brickLength - 1; n++) {
                            mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,ACS_HLINE);
                            mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,ACS_HLINE);
                        }
                        // Right side of brick
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,ACS_URCORNER);
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,ACS_LRCORNER);
                    }
                    else {
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+1,' ');
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+1,' ');
                        for (n = 1; n < brickLength - 1; n++) {
                            mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,' ');
                            mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,' ');
                        }
                        mvwaddch(gamescr,(i*2)+1,(j*brickLength)+n+1,' ');
                        mvwaddch(gamescr,(i*2)+2,(j*brickLength)+n+1,' ');
                    }
                }
            }
            mvwaddch(gamescr, ballY, ballX , 'o');
            if (ballsRemaining < 0) {
                mvwaddch(gamescr, ballY, ballX, ' ');
            }
        }
        
        // Update the paddle if an update occurs
        if (didUpdate) {
            for (i = 0; i <= paddleLength; i++) {
                mvwaddch(gamescr, height - 2, paddlePosition + i, '#');
            }
        }
        
        // Update the score, ball count, and multiplier
        if (didUpdate) {
            wclear(score_win);
            mvwprintw(score_win, 0, 0, "LIVES");
            for (i = 0; i < ballsRemaining; i++) {
                mvwaddch(score_win, 0, (i * 2) + 6, 'o');
            }
            mvwprintw(score_win, 0, 30, "MULTIPLIER x%d", scoreMultiplier);
            mvwprintw(score_win, 1, 0, "SCORE %d", playerScore);
        }
        
        // Update the screen, and reset the didUpdate variable
        if (didUpdate) {
            didUpdate = 0;
            wrefresh(gamescr);
            wrefresh(score_win);
#ifdef DEBUG
            wrefresh(debug_win);
#endif
            if (didDie && (ballsRemaining > -1)) {
                didDie = 0;
                usleep(1000000);
            }
            // Game Over screen
            else if (didDie) {
                nodelay(stdscr, FALSE);
                gameOverWin = newwin(9, 32, (LINES - 9) / 2, (COLS - 32) / 2);
                wborder(gameOverWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
                mvwprintw(gameOverWin, 2, 11, "GAME OVER");
                mvwprintw(gameOverWin, 4, (32 - (6 + (log10(playerScore)))) / 2, "SCORE %d", playerScore);
                mvwprintw(gameOverWin, 6, 12, "(Q)uit");
                wrefresh(gameOverWin);
                while ((ch = getch()) != 'q'){
                    usleep(10);
                }
                endwin();
                return 0;
            }
        }
    }
    endwin();
    return 0;
}

int increaseMult(int currentMultiplier) {
    if (currentMultiplier < 20)
        return (currentMultiplier + 1);
    else
        return currentMultiplier;
}