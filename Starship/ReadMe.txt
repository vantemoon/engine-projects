Starship Prototype (SD1-A02)

Build Instructions:

Open Starship.sln in Visual Studio.
Build the solution in Debug or Release mode for x64.
The executable will be placed in SD/Starship/Run as Starship_[Debug|Release]_x64.exe.

How to Play:

At the start, six Asteroids spawn at random positions and drift across the screen while spinning.
The PlayerShip starts at the centre, facing east.
Use thrust and rotation to navigate, fire bullets to destroy Asteroids, and avoid collisions.
The game ends only when the player quits or resets.

Controls:

S / F: Rotate ship left / right
E: Apply thrust in the current facing direction
Space: Fire a bullet from the ship’s nose
N: Respawn the PlayerShip if it has died
I: Spawn a new Asteroid (up to 12; shows error if exceeded)
F1: Toggle debug mode (radii, vectors, velocities drawn)
F8: Hard-reset the game
T: Hold to slow time to 1/10th speed
P: Pause or unpause the game
O: Advance one frame, then pause
Q: Quit the game

Gameplay Details:

The PlayerShip bounces off the edges of the world by reversing velocity.
Bullets expire when off-screen or on collision, and a maximum of 20 bullets can exist at once.
Asteroids have 3 health, and die when reduced to zero or when off-screen.
Colliding with an Asteroid kills the PlayerShip; press N to respawn.
Dead entities are flagged and cleaned up safely at the end of updates.

Known Issues:

None at this time.