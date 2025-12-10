Math Visual Tests (MP1-A06)


Build Instructions:

Open MathVisualTests.sln in Visual Studio.
Build the solution in Debug or Release mode for x64.
The executable will be located in SD/MathVisualTests/Run/MathVisualTests_[Debug|Release]_x64.exe.


How to Use:

The application launches directly into visual test mode.
Press F7 to cycle between the available visual tests:

Nearest Point 2D
Raycast vs. Discs 2D

Press Esc to exit the program.


Controls:

Shared Controls
Slow time: Hold T
Randomize objects: F8
Switch test mode: F7

Nearest Point Mode
Move reference point: Arrow keys or W A S D or E S D F
Snap reference point to mouse: Hold Left Mouse Button

Raycast vs. Discs Mode
Move ray start: W A S D or E S D F
Move ray end: I J K L
Move entire ray: Arrow keys
Snap ray start to mouse: Hold Left Mouse Button
Snap ray end to mouse: Hold Right Mouse Button


Visual Tests:

Nearest Point 2D
Displays seven geometric shapes: Triangle, Disc, AABB, OBB, Capsule, Line Segment, and Infinite Line. A white reference point moves around the screen. Each shape shows its nearest point in gold. Shapes brighten when the reference point is inside them, and lines connect the reference point to each nearest point.

Raycast vs. Discs 2D
Displays a ray interacting with multiple randomly placed discs. The ray visualizes impact information including hit location, normal, and coloured ray segments. If the ray hits a disc, the impacted disc is highlighted and the ray is drawn with grey, red, and yellow components. If no hit occurs, the ray is drawn in green.


Engine Features Demonstrated:

FloatRange and IntRange interval types.
Point-in-shape tests and nearest-point queries for all common 2D primitives.
Rendering utilities for discs, rings, capsules, triangles, AABBs, OBBs, and line segments.
RaycastResult2D and disc-raycasting utilities.
Mouse input support including normalized cursor UV and mouse-button tracking.


Known Issues:

None currently known.