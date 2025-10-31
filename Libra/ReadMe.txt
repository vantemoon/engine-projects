Libra Foundation (SD1-A05)


Build Instructions:

Open Starship.sln in Visual Studio.
Build the solution in Debug or Release mode for x64.
The executable will be located in SD/Libra/Run/Libra_[Debug|Release]_x64.exe.


How to Play:

The game starts in Attract Mode.
Press P (keyboard) or Start (controller) to begin.
Press Esc or Back to pause or exit.


Controls:

Xbox Controller (Recommended)
Move tank: Left joystick (direction and magnitude)
Aim turret: Right joystick
Pause/Resume: Start
Quit/Back to Attract: Back

Keyboard
Move tank: W A S D
Aim turret: I J K L
Pause/Resume: P
Quit/Back to Attract: Esc

Developer Features
Toggle time slow (×0.1): Hold T
Toggle time fast (×4): Hold Y
Debug toggle: F1
Noclip toggle: F3
Debug camera: F4
Hard restart: F8


Gameplay Details:

Player Tank:
Controlled via keyboard or controller. Moves up to 1 tile per second and rotates smoothly (180 degrees per second). 
The turret rotates independently (360 degrees per second) toward aiming input.

World and Map:
Randomly generated tile-based grid with two terrain types: GRASS and STONE.

Camera:
Follows the player and keeps it centered. Clamped to map bounds; F4 toggles a full-map debug camera.

Physics:
The tank behaves as a disc and slides along solid tiles. Implements corrective push-out against neighboring solids.

Game Modes:
Attract Mode – looping music and idle screen.
Playing – active gameplay.
Paused – translucent overlay; simulation halted.


Known Issues:

None currently known.


Sound Credits:

Attract Screen Music: SMU Guildhall_MX Tracks [Music by Guido Arcella Diez]\01_Magnetika\02 Magnetik Chaos [Music by Guido Arcella Diez].mp3
Gameplay Music: SMU Guildhall_MX Tracks [Music by Guido Arcella Diez]\01_Magnetika\01 Welcome To The RS-Magnetikia [Music by Guido Arcella Diez].mp3