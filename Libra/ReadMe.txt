Libra (SD1-A08)


Build Instructions:

Open Libra.sln in Visual Studio.
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
Fire bullets: Right trigger
Flamethrower attack: Left trigger
Pause/Resume: Start
Quit/Back to Attract: Back

Keyboard
Move tank: W A S D
Aim turret: I J K L
Fire bullets: Space
Flamethrower attack: E
Pause/Resume: P
Quit/Back to Attract: Esc

Developer Features
Toggle time slow (×0.1): Hold T
Toggle time fast (×4): Hold Y
Debug toggle: F1
Noclip toggle: F3
Debug camera: F4
Next heatmap: F6 (only when debug mode is on)
Hard restart: F8
Next map: F9
Open/close dev console: ~
Test dev console command: Enter (only when dev console is open)


Gameplay Details

Player Tank: Controlled with keyboard or controller, moving and rotating smoothly with an independently rotating turret. Fires straight-moving bullets from the gun tip that impact destructible walls or enemies. 

World and Map: Maps are randomly generated from tile definitions unless a map image is provided, each with unique dimensions, terrain, enemy counts, and an exit. Invalid maps regenerate until a valid path from the start to the exit exists. 

Camera: Follows and centers on the player while remaining clamped to map bounds. A full-map debug view can be toggled with F4. 

Physics: Entities use disc-based collision, pushing or being pushed depending on their physics flags. Tanks slide along solid tiles and resolve overlap through corrective push-out. 

Game Modes: Attract Mode shows an idle screen with looping audio, while Playing enables full gameplay. Paused stops simulation, and GameOver appears when the player dies and awaits respawn or exit input. Victory screen appears when the player enters the exit of the last map.


Enemy Types

Scorpio: A stationary turret that rotates toward the player when visible and fires when properly aligned, otherwise slowly turns on its own. 

Leo: A mobile tank that chases the player when seen, firing when facing the player, and otherwise wanders by choosing random orientations. 

Aries: Moves and navigates like Leo but does not shoot; instead, it uses a forward shield that deflects incoming bullets.


Known Issues:

None currently known.