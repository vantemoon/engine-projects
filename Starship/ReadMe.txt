Starship Gold (SD1-A04)


While working on Starship, I started noticing how often I was rewriting code with almost the same logic, just in slightly different ways, like updating entities or spawning new ones. At first it felt faster to just copy and tweak what I already had, but the more entity types and features I added, the messier things got. It became especially annoying when I added a new entity type and had to hunt through different functions to paste in the same code again. Eventually, I took some time to pull out the repeating parts and turn them into small, reusable functions. That made everything easier to manage and debug, and I realized how much time clean, reusable code actually saves in the long run.

I also got a lot more comfortable with refactoring, something we did often for the engine. I used to be scared of touching code that already worked because I didn’t want to break anything, and honestly, I did break things a few times. But over time, I learned that refactoring isn’t about fixing what’s broken, it’s about understanding your logic and improving it. Every time I reorganized a messy system, I saw it more clearly and felt more confident in the code. Now I see refactoring as part of the creative process, not something to avoid.


Build Instructions:

Open Starship.sln in Visual Studio.
Build the solution in Debug or Release mode for x64.
The executable will be placed in SD/Starship/Run as Starship_[Debug|Release]_x64.exe.


How to Play:

The game starts in Attract mode. Press Space/N on keyboard, or A/Start on a controller to begin.
Escape exits or returns to Attract mode after a game ends.


Controls:

Xbox Controller (Recommended)
Left joystick: Facing and thrust
A or Right Trigger: Fire a bullet
Left Trigger: Enter the scan mode
D-Pad: Change the selected target in scan mode
Right Shoulder: Detonate/telefrag the selected target
Start: Respawn if dead
Back: Return to Attract mode or exit if already there

Keyboard
A/D: Rotate ship left/right
W: Apply thrust
Space: Fire a bullet
T: Enter the scan mode, time slowed to 1/10
Arrow Keys: Change the selected target (only in scan mode)
F: Detonate/telefrag the selected target (only in scan mode)
N: Respawn if dead
P: Pause or resume
O: Advance one frame, then pause
Esc: Return to Attract mode or exit if already there

DEBUG:
F1: Toggle debug mode
F8: Hard-reset the game (only in debug mode)
I: Spawn an asteroid (only in debug mode)
K: Kill all hostile entities in this wave (only in debug mode)


Gameplay Details:

The Player Ship can be controlled with either a keyboard or an Xbox controller. The ship can rotate, thrust, and fire bullets as before, with smooth auto-fire support when the right trigger is held. Upon respawning, the Player Ship is temporarily invincible and flashes to indicate this state.

Asteroids now wrap around the screen edges and spawn near boundaries. Beetles move at a fixed speed, while Wasps accelerate toward the player. Both enemies face the player, take multiple hits to destroy, and deal damage on contact. Offscreen Beetles and Wasps are shown by directional indicators at the screen edges.

Activating Scan Mode slows time to 10%, allowing the player to inspect and target enemies. The selected target’s details are displayed onscreen. When an asteroid is selected, it can be detonated; when a Beetle or Wasp is selected, it can be telefragged. These actions consume energy, which regenerates gradually.

The HUD displays an energy bar and the current wave number. An impact wave visual effect appears on major explosions, and a Matrix-inspired animated background adds visual style to both Attract mode and gameplay.

Gameplay progresses in waves of increasing difficulty, with victory achieved after the fifth wave. The player starts with four lives, represented by ship icons at the top-left of the screen. When all lives are lost, the game ends and returns to Attract mode.


Known Issues:
None at this time.


Sound Credits:
- Player ship shoots bullet: Bxfr generated
- Player ship on hit/dies: Bxfr generated
- Player ship respawns: Bxfr generated
- Entities die: Bxfr generated
- Defeat: Guildhall sound library "99Sounds/ 99 Sound Effect/Braam - Retro Pulse.wav"
- Victory: Guildhall sound library "99Sounds/ 99 Sound Effect/Braam - Zone End.wav"
- Attract: Guildhall sound library "SoundFx/SceneScape/Scc01/12 Track 12.mp3"