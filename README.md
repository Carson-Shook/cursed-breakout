# cursed-breakout v0.3 alpha
The beloved brick breaking game now running in your favorite terminal emulator! While browsing GitHub, I was unable to find any Breakout clones that were made in pure C, so I decided to rectify this. Cursed-breakout is built using the ncurses library, hence the name, and should run on any Posix system, though it is under active development, and nothing is guaranteed. Support for Windows will (hopefully) come after the program is more or less complete.

It`s just a fun little project, and anyone who wants may use it for commercial or non-commercial projects, as long as the usage adheres to the MIT license. Have fun!

### Current Features
Move the paddle left and right with the left and right arrow keys to bounce the ball. Try to break all of the bricks in the level by bouncing the ball into them.

Press `q` to quit.

### Planned Features
- Adding ball reset on bounce miss.
- Adding scoring.
- Cleaning up existing calculations to reduce the amount of time that the CPU spends in the collision
- Figuring out a method to capture key presses to improve paddle movement.
- ~~Properly making the ball's directional velocity change according to where the paddle is struck~~.
**Change of plans on that one.** I'll save it for later. Getting the calculations right will take lots of fine tuning.

# Build Instructions
Type `make` to build the program, and `./breakout` to run it.
For debugging (*note: slows the ball down due to extra refresh each cycle*), type `make debug` when compiling. You may need to type `make clean` before doing so if you have already built it normally. Make sure that you choose an ample size for your terminal emulator if you use the debug info. It may overlap the game if you do not. 