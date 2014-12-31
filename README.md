# cursed-breakout v0.3 alpha
The beloved brick breaking game now running in your favorite terminal emulator! While browsing GitHub, I was unable to find any Breakout clones that were made in pure C, so I decided to rectify this. Cursed-breakout is built using the ncurses library, hence the name, and should run on any Posix system, though it is under active development, and nothing is guaranteed. Support for Windows will (hopefully) come after the program is more or less complete.

It's just a fun little project, and anyone who wants may use it for commercial or non-commercial projects, as long as the usage adheres to the MIT license. Have fun!

### What currently works
Ball bounces, paddle moves, and pressing 'q' quits the game, but that's it for now. 

### What is in development
- ~~Properly making the ball's directional velocity change according to where the paddle is struck~~.
**Change of plans on that one.** I'll save it for later. getting the calculations right will takes lots of fine tuning.
- Making bricks breakable, with proper associated ball physics included.
- Adding ball reset on bounce miss.
- Adding scoring.
- Figuring out a method to capture key presses to improve paddle movement.