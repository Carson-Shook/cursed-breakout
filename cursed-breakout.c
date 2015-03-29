
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
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <math.h>

int increaseMult(int, int);
    
int main(int argc, char *argv[]) {
    
    setlocale(LC_ALL, "");
    struct winsize max;                         // max size of the terminal
    ioctl(0, TIOCGWINSZ , &max);                // gets the max size of the terminal
    
    int i, j, n, x = 0;                         // temporary for-loop variables
    int ch;                                     // stores keypresses
   
    int height = 24;                            // game screen height
    int width = 82;                             // game screen width
    int scoreWinHeight = 2;                     // height of the score window
    int brickRows = 6;                          // rows of bricks to generate
    int brickColumns = 20;                      // columns of bricks to generate
    int bricks[brickRows][brickColumns];        // array to hold the locations of each brick
    int brickLength;                            // how long the bricks should be
    
    int paddlePosition;                         // location of the paddle's leftmost point
    int paddleLength = 16;                      // length of the paddle
    
    int ballX, ballY;                           // ball X and Y coordinates
    int velocityX = -1, velocityY = -1;         // ball X and Y directional veloctiy (not true velocity, that's handled through xDelay and yDelay)
    int aroundBall[8];                          // holds characters currently around the ball
    int collisionCase;                          // contains the current collision case for determining ball action
    int xDidBounceAlready = 1;                  // flag to indicate if the ball has already had its x velocity flipped
    int yDidBounceAlready = 1;                  // flag to indicate if the ball has already had its y velocity flipped
    
    int velDiff;                                                // change in velocity, see "VELOCITY ADJUSTER" below
    unsigned int universalDelay = CLOCKS_PER_SEC/20;            // the base rate of delay between ball updates
    unsigned int delayAdjuster = CLOCKS_PER_SEC/2000;           // base multiplier to adjust delay time by
    unsigned int xDelay = universalDelay + delayAdjuster * 140; // seconds until the ball's x coordinate updates
    unsigned int yDelay = universalDelay + delayAdjuster * 10;  // seconds until the ball's y coordinate updates
    clock_t xTimer;                                             // structure that holds the time since the last ball x coordinate update
    clock_t yTimer;                                             // structure that holds the time since the last ball y coordinate update

    int xUpdate = 0;                            // flag to indicate that the ball's x coordinate should change
    int yUpdate = 0;                            // flag to indicate that the ball's y coordinate should change
    int ballUpdate = 1;                         // flag to trip if ball updates
    int paddleUpdate = 1;                       // flag to trip if paddle updates
    int collisionUpdate = 1;                    // flag to trip if bricks update
    int didReset = 1;                           // flag to trigger death sequence actions
    int levelComplete = 1;                      // flag to trigger next level sequence
    int victory = 0;                            // flag to trigger victory quit sequence
    int quitFlag = 0;                           // flag to trigger quit sequence
    int fileProvided = 0;                       // flag to indicated that a file is available for reading
    int ready = 0;                              // flag that indicates player is ready to move past the main menu
    
    unsigned int playerScore = 0;               // the player's score
    int scoreMultiplier = 1;                    // the current score multiplier
    int ballsRemaining = 3;                     // number of lives the player has
    
    WINDOW *fileOpenWin;                        // the file manager window
    WINDOW *gamescr;                            // the game screen window
    WINDOW *scoreWin;                           // the score and life-count window
    WINDOW *gameOverWin;                        // the game over window
#ifdef DEBUG
    WINDOW *debug_win;                          // debug window (I should hope that to be obvious enough)
#endif
    
    FILE *levels;                               // file pointer for accessing any level files
    char *buffer;                               // dynamically allocated buffer that holds all level data
    char title[50];                             // level title
    
    DIR *currentDir;                            // directory to read from
    
#ifndef DEBUG
    // Check terminal size before starting
    if ((max.ws_row < (height + scoreWinHeight)) || (max.ws_col < width)) {
        printf("The terminal is too small. :(\nPlease resize it to at least %d by %d, and try again.\n\n", width, (height + scoreWinHeight));
        return 1;
    }
#endif
    
    /* BEGIN CURSES INITIALIZATION SECTION */
    
    // enters curses mode and sets window properties
    initscr();
    cbreak();
    noecho();
    leaveok(stdscr, TRUE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, FALSE);
    curs_set(0);
    refresh();      // clear the stdscr buffer before doing anything
    
    /* END CURSES INITIALIZATION SECTION */
    
    /* BEGIN FILE READ/MAIN MENU SECTION */
    
    // This is where the provided file is copied into the buffer, and
    // the first level is copied into the brick array. Otherwise, a
    if (argc == 2) {
        levels = fopen(argv[1], "r");
        if (levels) {
            if (fseeko(levels, 0 , SEEK_END) != 0) {
                fclose(levels);
                printf("Error: The file is empty\n");
                return 2;
            }
            int file_size = ftello(levels);
            buffer = (char*)malloc(file_size);
            rewind(levels);
            fread(buffer, sizeof(char), file_size, levels);
            fclose(levels);
            fileProvided = 1;
        }
        else {
            fclose(levels);
            printf("Error: There was a problem reading the file\n");
            return 1;
        }
    }
    else {
        fileOpenWin = newwin(height, width, ((LINES - height) / 2) - (scoreWinHeight / 2), (COLS - width) / 2);
        mvwprintw(fileOpenWin, 2, width/2 - 7, "CURSED-BREAKOUT");
        mvwprintw(fileOpenWin, 4, width/2 - 27, "Copyright (c) 2015 Carson Shook under the MIT License");
        wrefresh(fileOpenWin);
        
        // Borrowed some code from this GitHub repo: https://github.com/morganwilde/labyrinth
        // since it was almost exactly what I needed, and documentation was scarce.
        currentDir = opendir(".");
        struct dirent *dirStream = readdir(currentDir);
        char **files = malloc(sizeof(char *) * 1);
        files[0] = 0;
        // counter to place the files in an array
        n = 0;
        while (dirStream) {
            if ((dirStream = readdir(currentDir)) != NULL) {
                // find files ending with ".lvl"
                char *found = strstr(dirStream->d_name, ".lvl");
                if (found != NULL) {
                    // Save as an option
                    files = realloc(files, sizeof(char *) * (n+1));
                    files[n] = calloc(strlen(dirStream->d_name)+1, sizeof(char));
                    strcpy(files[n], dirStream->d_name);
                    n++;
                }
            }
        }
        
        closedir(currentDir);
        while (!ready) {
            mvwprintw(fileOpenWin, 7, width/2 - 35, "             (F)ile Select     (I)nfinite Mode     (Q)uit             \n\n\n\n\n\n\n\n\n\n\n\n\n");
            wborder(fileOpenWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
            wrefresh(fileOpenWin);

            // Select a menu option
            switch (ch = getch()) {
                case 'f':
                    mvwprintw(fileOpenWin, 7, width/2 - 35, "Select a file with the arrow keys and press (S)elect, or press (B)ack");
                    
                    i = 0; // file selection and offset
                    j = 0; // counter
                    while (!ready && (ch != 'b')) {
                        
                        char *filename = malloc(sizeof(char) * 0);
                        // this is just to clear the screen
                        mvwprintw(fileOpenWin, 8, width/2 - 35, "\n\n\n\n\n\n\n\n\n\n\n\n\n");
                        wborder(fileOpenWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
                        j = 0 + i - (i % 10);
                        if (files[0] != 0) {
                            while ((j < n) && (j < i - (i % 10) + 10)) {
                                filename = files[j];
                                // print the list of files
                                mvwprintw(fileOpenWin, 9 + j - (10 * (i / 10)), 5, "   %s", filename);
                                j++;
                            }
                            mvwprintw(fileOpenWin, 9 + i - (10 * (i / 10)), 5, ">");
                            wrefresh(fileOpenWin);
                            
                            // use curser keys to choose a file to read
                            switch (ch = getch()) {
                                case KEY_DOWN:
                                    if (i < n - 1) {
                                        i++;
                                    }
                                    break;
                                case KEY_UP:
                                    if (i > 0) {
                                        i--;
                                    }
                                    break;
                                case KEY_LEFT:
                                    if (i > 9) {
                                        i -= 10;
                                    }
                                    break;
                                case KEY_RIGHT:
                                    if (i - (i % 10) + 10 < n - 1) {
                                        i = i - (i % 10) + 10;
                                    }
                                    break;
                                case 's':
                                    // attempt to read the file
                                    levels = fopen(files[i], "r");
                                    if (levels) {
                                        if (fseeko(levels, 0 , SEEK_END) != 0) {
                                            fclose(levels);
                                            endwin();
                                            printf("Error: The file is empty\n");
                                            return 2;
                                        }
                                        int file_size = ftello(levels);
                                        buffer = (char*)malloc(file_size);
                                        rewind(levels);
                                        fread(buffer, sizeof(char), file_size, levels);
                                        fclose(levels);
                                        fileProvided = 1;
                                        ready = 1;
                                    }
                                    else {
                                        fclose(levels);
                                        endwin();
                                        printf("Error: There was a problem reading the file\n");
                                        return 1;
                                    }
                                    break;
                                case 'b':
                                    break;
                            }
                        }
                        else {
                            mvwprintw(fileOpenWin, 9+i, 8, "NO FILES FOUND", filename);
                            wrefresh(fileOpenWin);
                            usleep(1500000);
                            ch = 'b';
                        }
                    }
                    break;
                case 'i':
                    ready = 1;
                    break;
                case 'q':
                    endwin();
                    return 0;
                    break;
                default:
                    break;
            }
        }
    }
    delwin(fileOpenWin);
    
    /* END FILE READ/MAIN MENU SECTION */
    
    /* BEGIN GAME INITIALIZATION SECTION */
    
    // setup first level
    if (fileProvided == 1) {
        if ((ch = buffer[x]) != '%') {
            i = 0;
            if (buffer[x] == '*') {
                while (buffer[x++] != '\n') {
                }
            }
            while ((ch = buffer[x++]) != '\n') {
                title[i++] = ch;
            }
            title[i] = '\0';
            for (i = 0; i < brickRows; i++) {
                for (j = 0; j < brickColumns; j++) {
                    bricks[i][j] = buffer[x++] - '0';
                }
                x++;
            }
            didReset = 1;
        }
        else {
            victory = 1;
            levelComplete = 0;
        }
    }
    else {
        for (i = 0; i < brickRows; i++) {
            for (j = 0; j < brickColumns; j++) {
                bricks[i][j] = 1;
            }
        }
        strcpy(title, "Infinite Mode");
    }
    
    // set initial paddle position and ball location
    paddlePosition = (width / 2) - (paddleLength / 2);
    ballX = width / 2;
    ballY = height - 3;
    
    // Set brick length, and set all
    // bricks as active by iteratively
    // initializing all values to 1
    brickLength = (width - 2) / brickColumns;
    
    gamescr = newwin(height, width, ((LINES - height) / 2) - (scoreWinHeight / 2), (COLS - width) / 2);
    scoreWin = newwin(scoreWinHeight, width, ((LINES - height) / 2) + height - (scoreWinHeight / 2), (COLS - width) / 2);
#ifdef DEBUG
    debug_win = newwin(4, width, 0, 0);
#endif
    wborder(gamescr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
    
    nodelay(stdscr, TRUE);
    curs_set(0);
    xTimer = clock();
    yTimer = clock();
    
    /* END GAME INITIALIZATION SECTION */
    
    while (!quitFlag) {
        
        /* BEGIN INPUT AND MOVEMENT SECTION */
        
        ch = getch();
        
        // Detect key presses
        if (ch != ERR) {
            switch (ch) {
                // move left
                case KEY_DOWN:
                    if (paddlePosition > 1) {
                        paddlePosition--;
                        mvwaddch(gamescr, height - 2, paddlePosition + paddleLength + 1 , ' ');
                    }
                    paddleUpdate = 1;
                    break;
                // move right
                case KEY_UP:
                    if (paddlePosition < (width - paddleLength - 2)) {
                        paddlePosition++;
                        mvwaddch(gamescr, height - 2, paddlePosition - 1 , ' ');
                    }
                    paddleUpdate = 1;
                    break;
                // move left faster
                case KEY_LEFT:
                    if (paddlePosition > 2) {
                        paddlePosition -= 2;
                        mvwprintw(gamescr, height - 2, paddlePosition + paddleLength + 1 , "  ");
                    }
                    else if (paddlePosition > 1) {
                        paddlePosition--;
                        mvwaddch(gamescr, height - 2, paddlePosition + paddleLength + 1 , ' ');
                    }
                    paddleUpdate = 1;
                    break;
                // move right faster
                case KEY_RIGHT:
                    if (paddlePosition < (width - paddleLength - 3)) {
                        paddlePosition += 2;
                        mvwprintw(gamescr, height - 2, paddlePosition - 2 , "  ");
                    }
                    else if (paddlePosition < (width - paddleLength - 2)) {
                        paddlePosition++;
                        mvwaddch(gamescr, height - 2, paddlePosition - 1 , ' ');
                    }
                    paddleUpdate = 1;
                    break;
                // pause
                case 'p':
                    nodelay(stdscr, FALSE);
                    mvwprintw(gamescr, height/2 + 2, width/2 - 4, "(P)aused");
                    mvwprintw(gamescr, height/2 + 3, width/2 - 4, "(Q)uit");
                    wrefresh(gamescr);
                    while (!quitFlag && (ch = getch()) != 'p'){
                        usleep(10);
                        if (ch == 'q') {
                            quitFlag = 1;
                        }
                    }
                    mvwprintw(gamescr, height/2 + 2, width/2 - 4, "        ");
                    mvwprintw(gamescr, height/2 + 3, width/2 - 4, "      ");
                    collisionUpdate = 1;
                    nodelay(stdscr, TRUE);
                    break;
                case 's':
#ifdef DEBUG
                    for (i = 0; i < brickRows; i++) {
                        for (j = 0; j < brickColumns; j++) {
                            bricks[i][j] = 0;
                        }
                    }
                    collisionUpdate = 1;
#endif
                    break;
            }
        }
        
        // Check to see if the apropriate amount of time has
        // passed, and update the ball postion if so.
        if (difftime(clock(),xTimer) >= xDelay) {
            xTimer = clock();
            xUpdate = 1;
        }
        if (difftime(clock(),yTimer) >= yDelay) {
            yTimer = clock();
            yUpdate = 1;
        }
        
        if (xUpdate || yUpdate) {
            mvwaddch(gamescr, ballY, ballX , ' ');
            if (xUpdate) {
                ballX += velocityX;
                xDidBounceAlready = 0;
            }
            if (yUpdate) {
                ballY += velocityY;
                yDidBounceAlready = 0;
            }
            xUpdate = 0;
            yUpdate = 0;
            
            aroundBall[0] = mvwinch(gamescr, (int)ballY - 1, (int)ballX - 1);   // top left
            aroundBall[1] = mvwinch(gamescr, (int)ballY - 1, (int)ballX);       // top
            aroundBall[2] = mvwinch(gamescr, (int)ballY - 1, (int)ballX + 1);   // top right
            aroundBall[3] = mvwinch(gamescr, (int)ballY, (int)ballX - 1);       // left
            aroundBall[4] = mvwinch(gamescr, (int)ballY, (int)ballX + 1);       // right
            aroundBall[5] = mvwinch(gamescr, (int)ballY + 1, (int)ballX - 1);   // bottom left
            aroundBall[6] = mvwinch(gamescr, (int)ballY + 1, (int)ballX);       // bottom
            aroundBall[7] = mvwinch(gamescr, (int)ballY + 1, (int)ballX + 1);   // bottom right

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
            mvwprintw(debug_win, 1, 4, "yDelay %d     ", yDelay);
            mvwprintw(debug_win, 2, 4, "xDelay %d     ", xDelay);
#endif
            // only bothers with this code if the ball is near something
            if ((aroundBall[0] != ' ')||(aroundBall[1] != ' ')||(aroundBall[2] != ' ')||(aroundBall[3] != ' ')||(aroundBall[4] != ' ')||(aroundBall[5] != ' ')||(aroundBall[6] != ' ')||(aroundBall[7] != ' ')) {
                
                /*                      THE COLLIDER
                 *          - abandon hope all ye who enter here -
                 * acts a bit like a finite-state machine to manage the collision
                 * system. It examines the individual characters immediately around
                 * the ball and determines how to react based upon the type
                 * of characters next to it. If the character is part of a
                 * brick, then the location in the array is calculated by
                 * reversing the calculation used to place it in the brick
                 * update section of the main program, and setting that array
                 * value to zero. It also flips the appropriate velocity.
                 *
                 * There might still be bugs still hiding in here.
                 */
                
                collisionCase = 0;
                
                while (collisionCase != 999) {
                    switch (collisionCase) {
                            
                        case 0:
                            // top
                            if ((aroundBall[1] != ' ') && (velocityY < 0) && !yDidBounceAlready) {
                                if (aroundBall[1] == ACS_LLCORNER) {
                                    bricks[(((ballY - 1) - 2) / 2)][(ballX - 1) / brickLength] = 0;
                                    velocityY *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[1] == ACS_LRCORNER) {
                                    bricks[(((ballY - 1) - 2) / 2)][((ballX - 1) - (brickLength - 1)) / brickLength] = 0;
                                    velocityY *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[1] == ACS_HLINE) {
                                    if (ballY == 1) {
                                        velocityY *= -1;
                                        collisionCase = 8;
                                    }
                                    else {
                                        bricks[(((ballY - 1) - 2) / 2)][(ballX - (ballX % (brickLength))) / brickLength] = 0;
                                        velocityY *= -1;
                                        playerScore += scoreMultiplier;
                                        scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                                yDidBounceAlready = 1;
                            }
                            else collisionCase = 1;
                            break;
                        case 1:
                            // bottom
                            if ((aroundBall[6] != ' ') && (velocityY > 0) && !yDidBounceAlready) {
                                // paddle collsion state
                                if (aroundBall[6] == ACS_CKBOARD) {
                                    velocityY *= -1;
                                    
                                    /*                 VELOCITY ADJUSTER
                                     * calcuates a velocity change based on the way that the
                                     * ball hits the paddle.
                                     * velDiff is either positive or negative depending upon
                                     * which side of the paddle it strikes. The degree to
                                     * which this affects the current directional velocity                       |
                                     * can be altered by changing the number at the very end                    \|/
                                     * of this line.
                                     */
                                    
                                    velDiff = ((paddlePosition + (paddleLength / 2)) - ballX) * delayAdjuster * 10;
                                    if (velocityX > 0) {
                                        xDelay += velDiff;
                                        yDelay -= velDiff;
                                    }
                                    else if (velocityX < 0) {
                                        xDelay -= velDiff;
                                        yDelay += velDiff;
                                    }
                                    
                                    // a bunch of magic numbers that determine the maximum and
                                    // minimum velocity of the the ball. Tuned to perfection (I hope)
                                    
                                    // maximum x
                                    if (xDelay > universalDelay + delayAdjuster * 140) {
                                        xDelay = universalDelay + delayAdjuster * 140;
                                    }
                                    // minimum x
                                    else if (xDelay < universalDelay - delayAdjuster * 10) {
                                        xDelay = universalDelay - delayAdjuster * 10;
                                    }
                                    // maximum y
                                    if (yDelay > universalDelay + delayAdjuster * 170) {
                                        yDelay = universalDelay + delayAdjuster * 170;
                                    }
                                    // minimum y
                                    else if (yDelay < universalDelay + delayAdjuster * 20) {
                                        yDelay = universalDelay + delayAdjuster * 20;
                                    }
                                    collisionCase = 8;
                                }
                                else if (aroundBall[6] == ACS_ULCORNER) {
                                    bricks[(((ballY + 1) - 1) / 2)][(ballX - 1) / brickLength] = 0;
                                    velocityY *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[6] == ACS_URCORNER) {
                                    bricks[(((ballY + 1) - 1) / 2)][((ballX - 1) - (brickLength - 1)) / brickLength] = 0;
                                    velocityY *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 8;
                                }
                                else if (aroundBall[6] == ACS_HLINE) {
                                    // this is the death state
                                    if (ballY + 2 == height) {
                                        ballsRemaining--;
                                        velocityY *= -1;
                                        ballX = width / 2;
                                        ballY = height - 3;
                                        for (i = 0; i <= paddleLength; i++) {
                                            mvwaddch(gamescr, height - 2, paddlePosition + i, ' ');
                                        }
                                        paddlePosition = (width/2) - (paddleLength/2);
                                        scoreMultiplier = 1;
                                        paddleUpdate = 1;
                                        ballUpdate = 1;
                                        didReset = 1;
                                        collisionCase = 999;
                                    }
                                    else {
                                        bricks[(((ballY + 1) - 1) / 2)][(ballX - (ballX % (brickLength))) / brickLength] = 0;
                                        velocityY *= -1;
                                        playerScore += scoreMultiplier;
                                        scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                                yDidBounceAlready = 1;
                            }
                            else collisionCase = 2;
                            break;
                        case 2:
                            // left
                            if ((aroundBall[3] != ' ') && (velocityX < 0) && !xDidBounceAlready) {
                                if (aroundBall[3] == ACS_URCORNER) {
                                    bricks[(ballY - 1) / 2][((ballX - 1) - (brickLength - 1)) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_LRCORNER) {
                                    bricks[(ballY - 2) / 2][((ballX - 1) - (brickLength - 1)) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_VLINE) {
                                    if (ballX == 1) {
                                        velocityX *= -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                                xDidBounceAlready = 1;
                            }
                            else collisionCase = 3;
                            break;
                        case 3:
                            // right
                            if ((aroundBall[4] != ' ') && (velocityX > 0) && !xDidBounceAlready) {
                                if (aroundBall[4] == ACS_ULCORNER) {
                                    bricks[(ballY - 1) / 2][((ballX + 1) - 1) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_LLCORNER) {
                                    bricks[(ballY - 2) / 2][((ballX + 1) - 1) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_VLINE) {
                                    if (ballX + 2 == width) {
                                        velocityX *= -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                                xDidBounceAlready = 1;
                            }
                            else collisionCase = 4;
                            break;
                        case 4:
                            // top left
                            if ((aroundBall[0] == ACS_LRCORNER) && (velocityX < 0) && (velocityY < 0) && !(xDidBounceAlready || yDidBounceAlready)) {
                                bricks[((ballY - 1) - 2) / 2][((ballX - 1) - (brickLength - 1)) / brickLength] = 0;
                                velocityX *= -1;
                                velocityY *= -1;
                                playerScore += scoreMultiplier;
                                scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                collisionCase = 999;
                                yDidBounceAlready = 1;
                                xDidBounceAlready = 1;
                            }
                            else collisionCase = 5;
                            break;
                        case 5:
                            // top right
                            if ((aroundBall[2] == ACS_LLCORNER) && (velocityX > 0) && (velocityY < 0) && !(xDidBounceAlready || yDidBounceAlready)) {
                                bricks[((ballY - 1) - 2) / 2][((ballX + 1) - 1) / brickLength] = 0;
                                velocityX *= -1;
                                velocityY *= -1;
                                playerScore += scoreMultiplier;
                                scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                collisionCase = 999;
                                yDidBounceAlready = 1;
                                xDidBounceAlready = 1;
                            }
                            else collisionCase = 6;
                            break;
                        case 6:
                            // lower left
                            if ((aroundBall[5] != ' ') && (velocityX < 0) && (velocityY > 0) && !(xDidBounceAlready || yDidBounceAlready)) {
                                if (aroundBall[5] == ACS_URCORNER) {
                                    bricks[((ballY + 1) - 1) / 2][((ballX - 1) - (brickLength - 1)) / brickLength] = 0;
                                    velocityX *= -1;
                                    velocityY *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                    yDidBounceAlready = 1;
                                    xDidBounceAlready = 1;
                                }
                                else if (aroundBall[5] == ACS_CKBOARD) {
                                    velocityY *= -1;
                                    velocityX *= -1;
                                    yDidBounceAlready = 1;
                                    xDidBounceAlready = 1;
                                    collisionCase = 999;
                                }
                                else collisionCase = 999;
                            }
                            else collisionCase = 7;
                            break;
                        case 7:
                            // lower right
                            if ((aroundBall[7] != ' ') && (velocityX > 0) && (velocityY > 0) && !(xDidBounceAlready || yDidBounceAlready)) {
                                if (aroundBall[7] == ACS_ULCORNER) {
                                    bricks[((ballY + 1) - 1) / 2][((ballX + 1) - 1) / brickLength] = 0;
                                    velocityX *= -1;
                                    velocityY *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                    yDidBounceAlready = 1;
                                    xDidBounceAlready = 1;
                                }
                                else if (aroundBall[7] == ACS_CKBOARD) {
                                    velocityY *= -1;
                                    velocityX *= -1;
                                    yDidBounceAlready = 1;
                                    xDidBounceAlready = 1;
                                    collisionCase = 999;
                                }
                                else collisionCase = 999;
                            }
                            else collisionCase = 999;
                            break;
                        case 8:
                            // left if top or bottom
                            if ((aroundBall[3] != ' ') && (velocityX < 0) && !xDidBounceAlready) {
                                if (aroundBall[3] == ACS_URCORNER) {
                                    bricks[(ballY - 1) / 2][((ballX - 1) - brickLength) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_LRCORNER) {
                                    bricks[(ballY - 2) / 2][((ballX - 1) - brickLength) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[3] == ACS_VLINE) {
                                    if (ballX == 1) {
                                        velocityX *= -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                                xDidBounceAlready = 1;
                            }
                            else collisionCase = 9;
                            break;
                        case 9:
                            // right if top or bottom
                            if ((aroundBall[4] != ' ') && (velocityX > 0) && !xDidBounceAlready) {
                                if (aroundBall[4] == ACS_ULCORNER) {
                                    bricks[(ballY - 1) / 2][((ballX + 1) - 1) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_LLCORNER) {
                                    bricks[(ballY - 2) / 2][((ballX + 1) - 1) / brickLength] = 0;
                                    velocityX *= -1;
                                    playerScore += scoreMultiplier;
                                    scoreMultiplier = increaseMult(scoreMultiplier, 1);
                                    collisionCase = 999;
                                }
                                else if (aroundBall[4] == ACS_VLINE) {
                                    if (ballX + 2 == width) {
                                        velocityX *= -1;
                                        collisionCase = 999;
                                    }
                                }
                                else collisionCase = 999;
                                xDidBounceAlready = 1;
                            }
                            else collisionCase = 999;
                            break;
                        default:
                            collisionCase = 999;
                            break;
                    }
                }
                collisionUpdate = 1;
            }
            ballUpdate = 1;
        }
        
        /* END INPUT AND MOVEMENT SECTION */
        
        /* BEGIN RENDERING SECTION */
        
        // Update the bricks if an update occurs
        if (collisionUpdate) {
            levelComplete = 1;
            do {
                for (i = 0; i < brickRows; i++) {
                    for (j = 0; j < brickColumns; j++) {
                        if (bricks[i][j] == 1) {
                            levelComplete = 0;
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
                
                /*                  LEVEL COMPLETE
                 * Okay, so this part is kind of a hack. I needed
                 * something that could detect when all of the bricks
                 * were gone (set to 0), and I wanted to do so without
                 * adding too many extra cycles to the game. Since the
                 * brick renderer already does this, I figured that it
                 * would be best to place the check in here. If there
                 * are any bricks left, then this check will return
                 * false and the loop to rerender will not continue either.
                 */
                
                if (levelComplete) {
                    if (fileProvided == 1) {
                        if ((ch = buffer[x]) != '%') {
                            i = 0;
                            if (buffer[x] == '*') {
                                while (buffer[x++] != '\n') {
                                }
                            }
                            while ((ch = buffer[x++]) != '\n') {
                                title[i++] = ch;
                            }
                            title[i] = '\0';
                            for (i = 0; i < brickRows; i++) {
                                for (j = 0; j < brickColumns; j++) {
                                    bricks[i][j] = buffer[x++] - '0';
                                }
                                x++;
                            }
                            didReset = 1;
                        }
                        else {
                            victory = 1;
                            levelComplete = 0;
                        }
                    }
                    else {
                        for (i = 0; i < brickRows; i++) {
                            for (j = 0; j < brickColumns; j++) {
                                bricks[i][j] = 1;
                            }
                        }
                        didReset = 1;
                    }
                    mvwaddch(gamescr, ballY, ballX , ' ');
                    playerScore += 10 * scoreMultiplier;
                    ballX = width / 2;
                    ballY = height - 3;
                    velocityY = -1;
                    for (i = 0; i <= paddleLength; i++) {
                        mvwaddch(gamescr, height - 2, paddlePosition + i, ' ');
                    }
                    paddlePosition = (width/2) - (paddleLength/2);
                    paddleUpdate = 1;
                    ballUpdate = 1;
                }
                
            } while (levelComplete);
        }
        
        if (ballUpdate && ballsRemaining > -1) {
                mvwaddch(gamescr, ballY, ballX , 'o');
        }
        
        // Update the paddle if a paddleUpdate occurs
        if (paddleUpdate) {
            for (i = 0; i <= paddleLength; i++) {
                mvwaddch(gamescr, height - 2, paddlePosition + i, ACS_CKBOARD);
            }
        }
        
        // Update the score, ball count, and multiplier
        if (collisionUpdate) {
            mvwprintw(scoreWin, 0, 0, "LIVES");
            mvwprintw(scoreWin, 0, 6, " ");
            for (i = 0; i < ballsRemaining; i++) {
                mvwprintw(scoreWin, 0, (i * 2) + 6, "o  ");
            }
            mvwprintw(scoreWin, 0, width-14, "%2dX MULTIPLIER", scoreMultiplier);
            mvwprintw(scoreWin, 1, 0, "SCORE %d", playerScore);
            mvwprintw(scoreWin, 1, width/2 - (strlen(title)/2), "%s", title);
        }
        
        /* END RENDERING SECTION */
        
        /* BEGIN REDRAW SECTION */
        
        // Update the screen, and reset the Update variables
        if (collisionUpdate) {
            wnoutrefresh(scoreWin);
        }
        if (ballUpdate || paddleUpdate) {
            wnoutrefresh(gamescr);
#ifdef DEBUG
            wnoutrefresh(debug_win);
#endif
        }
        if (collisionUpdate || ballUpdate || paddleUpdate) {
            doupdate();
            ballUpdate = 0;
            paddleUpdate = 0;
            collisionUpdate = 0;
            if (didReset && (ballsRemaining > -1)) {
                didReset = 0;
                usleep(1000000);
            }
            // Game Over screen
            else if (didReset) {
                nodelay(stdscr, FALSE);
                gameOverWin = newwin(9, 32, (LINES - 9) / 2, (COLS - 32) / 2);
                wborder(gameOverWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
                mvwprintw(gameOverWin, 2, 11, "GAME OVER");
                mvwprintw(gameOverWin, 4, (32 - (6 + (log10(playerScore)))) / 2, "SCORE %d", playerScore);
                mvwprintw(gameOverWin, 6, 13, "(Q)uit");
                wrefresh(gameOverWin);
                while ((ch = getch()) != 'q'){
                    usleep(10);
                }
                quitFlag = 1;
            }
            // Victory screen. It's just the game over screen with a different message at present.
            if (victory) {
                nodelay(stdscr, FALSE);
                gameOverWin = newwin(9, 32, (LINES - 9) / 2, (COLS - 32) / 2);
                wborder(gameOverWin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);
                mvwprintw(gameOverWin, 2, 12, "YOU WIN!");
                mvwprintw(gameOverWin, 4, (32 - (6 + (log10(playerScore)))) / 2, "SCORE %d", playerScore);
                mvwprintw(gameOverWin, 6, 13, "(Q)uit");
                wrefresh(gameOverWin);
                while ((ch = getch()) != 'q'){
                    usleep(10);
                }
                quitFlag = 1;
            }
        }
        
        /* END REDRAW SECTION */
    }
    endwin();
    return 0;
}

// increases the current multiplier, but no higher than 20X
int increaseMult(int currentMultiplier, int AmountToAdd) {
    if (currentMultiplier < 20)
        return (currentMultiplier + AmountToAdd);
    else
        return currentMultiplier;
}