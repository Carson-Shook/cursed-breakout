# cursed-breakout v1.0.1
The beloved brick breaking game now running in your favorite terminal emulator! While browsing GitHub, I was unable to find any Breakout clones that were made in pure C (or at least using common C libraries), so I decided to rectify this. Cursed-breakout is built using the ncurses library, hence the name, and should run on most Posix systems. I'll do my best to check whether or not it runs on particular systems, but it would really help if you could test it on your system, and send me a bug report if it doesn't.

It`s just a fun little project, and anyone who wants may use it for commercial or non-commercial projects, as long as the usage adheres to the MIT license. Have fun!

### Features
Move the paddle left and right with the left and right arrow keys to bounce the ball. You can also use the up and down arrow keys if you want to move slower. Try to break all of the bricks in each level by bouncing the ball into them. Each brick broken increases your multiplier. Every level you complete earns 10 x the multiplier in points, and every 2000 points wins you another ball. Don't lose all of your lives, and try to achieve a high score.

Press `p` to pause. Once paused, press it again to resume, or press `q` to quit.

To create your own levels, have a look at the included `example.lvl` file.

### Possible Future Features
- Level format verification
- Ability to restart or return to the Main Menu

# Build Instructions
Type `make` to build the program, and `./breakout` (or `./breakout example.lvl`) to run it.
For debugging, type `make debug` when compiling. You may need to type `make clean` before doing so if you have already built it normally. Make sure that you choose an ample size for your terminal emulator if you use the debug info. It may overlap the game if you do not. 

# Changelog
#### v1.0.1
- Fixed an issue relating to using difftime that caused Infinite Mode to draw no bricks and freeze on Linux systems.

#### v1.0 GM
- Official release, yay!
- Added new ball reward every 2000 points
- Added arrows to file selection to indicate that there are more than ten files
- Made infinite mode generation based on the current CPU clock time for random level generation
- Fixed a bug that would cause the level name to not be erased in preparation for a new one.

#### v1.0 beta 2
- Added title screen with level selection
- Added debug feature: press 's' to skip the current level
- Fixed an issue where the ball could break out of the game field when hitting the paddle and wall at the same time

#### v1.0 beta 1
- Added custom level system (see example.lvl for formatting)
- Added pause screen and moved the quit option into it
- Added directional velocity for the ball. It now bounces off of the paddle at different angles depending upon where you hit it. This is accomplished with separate x and y timers, and a slew of other variables relating to the speed of the ball (or more specifically, the delay before the ball moves again)
- Moved paddle physics into the Collider
- Changed the up/down/left/right arrow keys so that left and right now move at the 2x speed, while up and down move one space at a time. I just flipped them because I found myself using up and down more often anyway.
- Changed the old method of quitting to a `quitFlag` to allow for a more intricate quit sequence
- Replaced `didUpdate` with `paddleUpdate`, `ballUpdate`, and `collisionUpdate` for more precise control over refreshes, and added `leaveok()` to reduce the need to move the curser so often
- Cleaned up calculations for better readability
- Fixed an error in my math caused by "correcting" the previous errors in the Collider
- Fixed an issue where the ball would mysteriously bounce off of the bricks backwards to how it was supposed to by adding a check to ensure that the ball was coming towards the brick first

#### v0.5 alpha
- Added ball reset, life count, and **Game Over** screen
- Added score system with multiplier for each successful hit
- Added up and down arrow key condition checks to allow for moving the paddle twice as fast
- Moved collision system into `main`
- Fixed a bug with the lower right corner of the ball (the case was being skipped on accident)
- Fixed a slight error in my math that sometimes caused problems for bricks that were on the right side of the game screen
- Fixed slowdown caused by debug mode
- Updated Makefile to include the C math library to perform a logarithmic function in the program.

#### v0.4.1 alpha
- Made ball speed processor independent by replacing the counter with `difftime()` check that gets the difference of two `clock()` functions. *Note: this is still compiler dependent, so I'll need to perform more tests to verify my own claims.*
- Replaced instances of `stdscr` with `gamescr` to fix display issues on some Linux systems

#### v0.4 alpha
- Removed old "collision system"
- Added new ball collision system that can check all characters adjacent to itself, and allows bricks to break
- Added a startup check to ensure that the terminal window is large enough
- General code cleanup, equations in particular
- Added debug option when building, `make debug`
- Updated MIT License year to 2015 and copied it into the main program

#### v0.3 alpha
- Fixed paddle edge cases
- Add collision check for ball and paddle

#### v0.2 alpha
- Added ball momentum
- Added ball/wall collision
- Made bricks variable length to allow for screen resizing

#### v0.1 alpha
- Added loop to initialize and draw bricks on screen
- Added paddle drawing and movement
- Added the MIT License