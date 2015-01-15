# cursed-breakout v0.4.1 alpha
The beloved brick breaking game now running in your favorite terminal emulator! While browsing GitHub, I was unable to find any Breakout clones that were made in pure C, so I decided to rectify this. Cursed-breakout is built using the ncurses library, hence the name, and should run on most Posix systems, though it is under active development, and this is not currently guaranteed. Once in a more polished state, I'll start making the appropriate checks to ensure smooth sailing on other systems. Support for Windows will (maybe) come after the program is more or less complete.

It`s just a fun little project, and anyone who wants may use it for commercial or non-commercial projects, as long as the usage adheres to the MIT license. Have fun!

### Current Features
Move the paddle left and right with the left and right arrow keys to bounce the ball. Try to break all of the bricks in the level by bouncing the ball into them.

Press `q` to quit.

### Features Under Development
- Ball reset on bounce miss
- Scoring system
- Pause/Resume
- Settings menu for changing ball speed/game window size, or whatever else I feel needs an option
- Cleaning up existing calculations to reduce the amount of time that the CPU spends in the collision section
- A method to capture key presses (`key_up` `key_down` sorts of things) to improve paddle movement
- Changing directional velocity according to where the ball strikes the paddle
*Saving this one for later. Getting the calculations right will take lots of fine tuning.*

# Build Instructions
Type `make` to build the program, and `./breakout` to run it.
For debugging (*note: slows the ball down due to extra refresh each cycle*), type `make debug` when compiling. You may need to type `make clean` before doing so if you have already built it normally. Make sure that you choose an ample size for your terminal emulator if you use the debug info. It may overlap the game if you do not. 

# Changelog
- v0.4.1 alpha
-- Made ball speed processor independent by replacing the counter with `difftime()` check that gets the difference of two `clock()` functions. *Note: this is still compiler dependent, so I'll need to perform more tests to verify my own claims.*
-- Replaced instances of `stdscr` with `gamescr` to fix display issues on some Linux systems

- v0.4 alpha
-- Removed old "collision system"
-- Added new ball collision system that can check all characters adjacent to itself, and allows bricks to break
-- Added a startup check to ensure that the terminal window is large enough
-- General code cleanup, equations in particular
-- Added debug option when building, `make debug`
-- Updated MIT License year to 2015 and copied it into the main program

- v0.3 alpha
-- Fixed paddle edge cases
-- Add collision check for ball and paddle

- v0.2 alpha
-- Added ball momentum
-- Added ball/wall collision
-- Made bricks variable length to allow for screen resizing

- v0.1 alpha
-- Added loop to initialize and draw bricks on screen
-- Added paddle drawing and movement
-- Added the MIT License