Doomenstein Gold - Sylvia Sun

Author note:
This assignment extends the existing Doomenstein first-person shooter framework into a simplified 3D virtual pet system inspired by Tamagotchi. Instead of focusing on combat, the new mode asks the player to care for demon pets using first-person interactions.

This implementation focuses on turning the existing FPS framework into a non-combat virtual pet care loop. The feature set intentionally reuses existing engine and gameplay systems where possible, while adding new behavior on top of them.


============================================================
OVERVIEW
============================================================

The new gameplay loop is based around caring for demon pets.

Core actions:
- Feed the demon by shooting food projectiles.
- Clean mess created by the demon.
- Maintain hunger, cleanliness, and happiness.
- Discipline the demon when it misbehaves.
- Collect pickups to support demon care.
- Observe the demon evolving based on care quality.


============================================================
CORE MECHANICS
============================================================

1. Feed the demon by shooting food projectiles

The player can equip the Food Gun and fire food projectiles at the demon. When the projectile collides with a virtual pet, it restores hunger and slightly increases happiness. The projectile is removed immediately after feeding the pet.

Food projectiles do not damage the pet.


2. Clean mess created by the demon

The demon creates DemonMess actors when cleanliness drops below the configured threshold. The player can equip the Cleaning Tool and clean the mess using a short-range cleaning ray/cone.

When mess is cleaned:
- the DemonMess actor is removed
- the nearest virtual pet gains cleanliness
- the nearest virtual pet gains a small amount of happiness


3. Maintain hunger, cleanliness, and happiness

Each virtual pet tracks three needs:
- Hunger
- Cleanliness
- Happiness

These values decay over time. They are clamped between 0 and 100.

The player must use care tools and pickups to keep the pet healthy. Low values can cause the pet to misbehave, which increases pressure on the player. If any of the values reaches 0, the pet dies immediately.


4. Discipline the demon when it misbehaves

When one or more of the demon's needs are low, the demon can enter a misbehaving state. Misbehaving demons wander more actively and create mess faster.

The discipline command is implemented as a non-damaging melee-style weapon. If the player disciplines a misbehaving demon, the demon stops misbehaving temporarily. If the player disciplines a demon that is not misbehaving, the demon loses more happiness.


5. Collect pickups to support demon care

Three pickup types were added:
- FoodPickup
- CleaningPickup
- ToyPickup

Food pickups restore food ammo. Cleaning pickups restore cleaning charges. Toy pickups increase the happiness of a nearby demon.

Pickups respawn at their original positions after a short delay, so the player can continue caring for the pets during longer play sessions.


============================================================
FEATURE 1: FOOD GUN
============================================================

The Food Gun is a new weapon that supports the virtual pet care loop.

Purpose:
- Feeds the demon without damaging it.

Implementation summary:
- Added a FoodGun weapon definition.
- Added a FoodProjectile actor definition.
- FoodGun fires FoodProjectile actors using the existing projectile weapon path.
- FoodProjectile checks collision with virtual pets.
- If the hit actor is a virtual pet, the pet's hunger increases.
- Happiness also increases slightly as a reward.
- FoodProjectile is destroyed after a successful hit.
- The food gun consumes food ammo.


============================================================
FEATURE 2: CLEANING TOOL
============================================================

The Cleaning Tool is a new mechanic for removing mess created by the demon.

Purpose:
- Lets the player maintain the demon's cleanliness.

Implementation summary:
- Added DemonMess actors.
- Virtual pets spawn DemonMess when cleanliness is low.
- Mess spawning is controlled by a timer.
- Nearby mess count is capped so the room does not fill endlessly.
- Added a CleaningTool weapon definition.
- CleaningTool uses a short-range multi-ray cone to make cleaning easier.
- If the CleaningTool hits DemonMess, the mess is removed.
- The nearest virtual pet gains cleanliness and slight happiness.
- CleaningTool consumes cleaning charges.


============================================================
FEATURE 3: NEEDS SYSTEM
============================================================

The Needs System is the foundation of the virtual pet gameplay.

Purpose:
- Gives the demon persistent care-based stats.

Implemented stats:
- Hunger
- Cleanliness
- Happiness

Implementation summary:
- Added virtual pet fields to Actor.
- Added a virtualPet flag to ActorDefinition.
- Virtual pets decay hunger, cleanliness, and happiness over time.
- Values are clamped between 0 and 100.
- Low needs can trigger misbehavior.
- Pet stats are displayed on a custom pet HUD.

Pet HUD:
- Appears when the player is facing a pet within a wide vision cone.
- Uses a cone check instead of a strict raycast so the HUD feels less finicky.
- Displays hunger, cleanliness, happiness, behavior state, and evolution stage.


============================================================
FEATURE 4: PICKUPS
============================================================

Pickups support the care loop by giving the player limited resources.

Purpose:
- Encourages the player to move around the room and manage resources.

Implemented pickup types:
- FoodPickup: adds food ammo
- CleaningPickup: adds cleaning charges
- ToyPickup: increases nearby pet happiness

Implementation summary:
- Added pickup flags and respawn timing to actors.
- Pickups are hidden and deactivated when collected.
- Pickups respawn at their original position after a short delay.
- Pickups use the existing actor collision system.
- Pickup resources are shown through the weapon HUD.


============================================================
STRETCH GOAL: DISCIPLINE SYSTEM
============================================================

The Discipline System adds a simple behavioural response to poor care.

Purpose:
- Makes low needs affect gameplay, not just HUD values.

Implementation summary:
- Virtual pets can enter a misbehaving state when hunger, cleanliness, or happiness is too low.
- Misbehaving pets wander more actively.
- Misbehaving pets create mess faster.
- Added DisciplineCommand as a melee weapon.
- The command is non-damaging.
- Correct discipline stops misbehaviour temporarily but slightly lowers happiness.
- Incorrect discipline lowers happiness more.


============================================================
STRETCH GOAL: EVOLUTION
============================================================

The Evolution System adds care-based actor variation.

Purpose:
- Rewards good care and punishes poor care with different long-term pet states.

Implementation summary:
- Pets track good care and bad care over time.
- Good care is counted when hunger, cleanliness, and happiness stay high.
- Bad care is counted when any need becomes too low.
- After the evolution timer finishes, the pet evolves into one of two states:
  - GoodCare
  - Neglected

GoodCare effects:
- Needs decay more slowly.
- Mess pressure is reduced.

Neglected effects:
- Needs decay faster.
- Mess pressure is increased.

The current evolution stage is displayed on the pet HUD.


============================================================
PET AI CHANGES
============================================================

Virtual pets no longer use normal combat AI.

Implementation summary:
- AI::Update checks whether the actor is a virtual pet.
- If it is a virtual pet, it uses virtual pet AI instead of enemy targeting.
- Pets wander around instead of chasing and attacking players.
- Low needs make pets sluggish.
- Misbehaving pets move more actively.
- A simple variation system gives pets slightly different wander styles.


============================================================
PET ROOM MAP
============================================================

A smaller pet-focused map setup was added.

PetRoom includes:
- two player spawn points
- two demon pets
- one food pickup
- one cleaning pickup
- one toy pickup

The room is intended to be small enough for quick testing and multiplayer support.


============================================================
CONTROLS
============================================================

[Attract Mode]
Keyboard                         Controller
Space                            Start / P-style start input: Start game
Esc                              Exit


[First-Person Camera]
Keyboard                         Controller
Mouse                            Right Stick         Look, yaw / pitch
WASD                             Left Stick          Move relative to camera
Shift                            A, hold             Sprint
Left Mouse Button                Right Trigger       Fire weapon
Arrow Keys                       D-Pad Left / Right  Switch weapon
1                                X                   Weapon 1
2                                Y                   Weapon 2
3                                B                   Weapon 3
F                                -                   Toggle free-fly
N                                -                   Possess next actor
P                                Start               Pause / Resume
Esc                              Back                Back to attract mode


[Free-Fly Camera]
Keyboard                         Controller
Mouse                            Right Stick         Look, yaw / pitch
WASD                             Left Stick          Move relative to camera
Z / C                            LB / RB             Move up / down
Shift                            A, hold             Fast move
F                                -                   Toggle free-fly
P                                Start               Pause / Resume
H                                Back                Reset camera
Esc                              Back                Back to attract mode


[Virtual Pet Care Controls]
Keyboard / Controller
Use weapon switching controls to select care tools.
Use Fire Weapon to activate the selected tool.

Care weapons:
- FoodGun: shoot food projectiles at a demon to restore hunger.
- CleaningTool: fire at DemonMess to clean it and restore cleanliness.
- DisciplineCommand: use as a melee command on a misbehaving demon.
