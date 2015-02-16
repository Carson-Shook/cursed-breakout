# cursed-breakout v0.5 alpha
The beloved brick breaking game now running in your favorite terminal emulator! While browsing GitHub, I was unable to find any Breakout clones that were made in pure C, so I decided to rectify this. Cursed-breakout is built using the ncurses library, hence the name, and should run on most Posix systems, though it is under active development and this is not currently guaranteed. Once in a more polished state, I'll start making the appropriate optimizations to ensure smooth sailing on other systems. Support for Windows will (maybe) come after the program is more or less complete.

It`s just a fun little project, and anyone who wants may use it for commercial or non-commercial projects, as long as the usage adheres to the MIT license. Have fun!

### Current Features
Move the paddle left and right with the left and right arrow keys to bounce the ball. You can also use the up and down arrow keys to move a little faster. Try to break all of the bricks in the level by bouncing the ball into them. Don't loose all of your lives, and try to achieve a high score.

Press `q` to quit.

### Features Under Development
- Pause/Resume
- Custom level system
- Cleaning up existing calculations to improve readability
- Changing directional velocity according to where the ball strikes the paddle. *This one is proving difficult to implement, but I have an idea that I'm pondering on.*
- ~~A method to capture key presses (`key_up` and `key_down` sorts of things) to improve paddle movement~~ *I've found that this is almost impossible at the moment, but if anyone knows a way to get the events and write appropriate handlers for them, let me know.*

# Build Instructions
Type `make` to build the program, and `./breakout` to run it.
For debugging, type `make debug` when compiling. You may need to type `make clean` before doing so if you have already built it normally. Make sure that you choose an ample size for your terminal emulator if you use the debug info. It may overlap the game if you do not. 

# Changelog
v0.5 alpha
- Added ball reset, life count, and **Game Over** screen
- Added score system with multiplier for each successful hit
- Added up and down arrow key condition checks to allow for moving the paddle twice as fast
- Moved collision system into `main`
- Fixed a bug with the lower right corner of the ball (the case was being skipped on accident)
- Fixed a slight error in my math that sometimes caused problems for bricks that were on the right side of the game screen
- Fixed slowdown caused by debug mode
- Updated Makefile to include the C math library to perform a logarithmic function in the program.

v0.4.1 alpha
- Made ball speed processor independent by replacing the counter with `difftime()` check that gets the difference of two `clock()` functions. *Note: this is still compiler dependent, so I'll need to perform more tests to verify my own claims.*
- Replaced instances of `stdscr` with `gamescr` to fix display issues on some Linux systems

v0.4 alpha
- Removed old "collision system"
- Added new ball collision system that can check all characters adjacent to itself, and allows bricks to break
- Added a startup check to ensure that the terminal window is large enough
- General code cleanup, equations in particular
- Added debug option when building, `make debug`
- Updated MIT License year to 2015 and copied it into the main program

v0.3 alpha
- Fixed paddle edge cases
- Add collision check for ball and paddle

v0.2 alpha
- Added ball momentum
- Added ball/wall collision
- Made bricks variable length to allow for screen resizing

v0.1 alpha
- Added loop to initialize and draw bricks on screen
- Added paddle drawing and movement
- Added the MIT License