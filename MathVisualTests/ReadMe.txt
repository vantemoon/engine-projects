Math Visual Tests (MP2-A04)

Build Instructions:

Open MathVisualTests.sln in Visual Studio.
Build the solution in Debug or Release mode for x64.
The executable will be located in:
SD/MathVisualTests/Run/MathVisualTests_[Debug|Release]_x64.exe

How to Use:

The application launches directly into visual test mode.

Press F6 and F7 to cycle backward and forward through the available visual tests:

Nearest Point 2D
Raycast vs. Discs 2D
Raycast vs. Line Segments 2D
Raycast vs. AABB 2D
Test Shapes 3D

Press F8 to reset and re-randomize the current test scene.
Press Esc to quit the program.

Controls:

Shared Controls
Slow time: Hold T
Randomize scene objects: F8
Switch test mode: F6/F7
Quit application: Esc

Nearest Point 2D
Move reference point: Arrow keys or W A S D
Snap reference point to mouse: Hold Left Mouse Button

Raycast vs. Discs 2D
Move ray start: W A S D
Move ray end: I J K L
Move entire ray: Arrow keys
Snap ray start to mouse: Hold Left Mouse Button
Snap ray end to mouse: Hold Right Mouse Button

Raycast vs. Line Segments 2D
Move ray start: W A S D
Move ray end: I J K L
Move entire ray: Arrow keys
Snap ray start to mouse: Hold Left Mouse Button
Snap ray end to mouse: Hold Right Mouse Button

Raycast vs. AABB 2D
Move ray start: W A S D
Move ray end: I J K L
Move entire ray: Arrow keys
Snap ray start to mouse: Hold Left Mouse Button
Snap ray end to mouse: Hold Right Mouse Button

Test Shapes 3D
Move horizontally: W A S D
Move vertically: Q / E
Look around: Move mouse
Lock or unlock nearest-point and raycast reference position: Space
Grab or release highlighted object: Left Mouse Button

Visual Tests:

Nearest Point 2D
Displays several 2D geometric shapes including a triangle, disc, AABB, OBB, capsule, line segment, and infinite line. A reference point moves around the screen, and each shape shows its nearest point in gold. Shapes brighten when the reference point is inside them, and lines are drawn from the reference point to each nearest point.

Raycast vs. Discs 2D
Displays a ray interacting with multiple randomly placed discs. The ray supports bouncing, so after hitting a disc it reflects and continues travelling until it runs out of remaining length or reaches the bounce limit. Impact positions, normals, and ray segments are drawn, and all impacted discs are highlighted.

Raycast vs. Line Segments 2D
Displays a ray interacting with several randomly generated line segments. The ray bounces off line segments based on the impact normal, and each hit shows the impact point, normal, and continuation of the reflected ray. All impacted line segments are highlighted.

Raycast vs. AABB 2D
Displays a ray interacting with several randomly generated AABBs. The ray reflects after impact and continues tracing through the scene, showing multiple hits when applicable. Impact positions, normals, and impacted AABBs are visualized consistently with the other raycast modes.

Test Shapes 3D
Displays a 3D test scene containing a random mix of AABB3s, spheres, and z-cylinders. Objects are shown using a mix of textured solid rendering and wireframe.

This mode supports 3D camera movement, nearest-point queries, overlap detection, raycasting, and object grabbing.

Each object shows its nearest point to the current reference position using a small orange sphere, while the single closest point overall is shown in green. Pressing Space toggles whether the reference position follows the camera or remains locked in place.

Objects that overlap with any other object pulse darker to indicate intersection.

A 3D raycast is fired forward from the reference position. On impact, the hit point is shown with a white sphere, the impact normal is shown with a yellow arrow, and the impacted object is highlighted. When the reference ray is locked, the full ray is also drawn in the world.

If the currently highlighted object is clicked with the left mouse button, it becomes grabbed, turns red, and remains at a fixed position relative to the camera until released.

Engine Features Demonstrated:

Support for both 2D and 3D rendering
Textured and untextured mesh rendering
Multiple visual test modes with seamless switching
HUD text rendering in all modes
2D nearest-point queries for common primitives
2D raycasting against discs, line segments, and AABBs
Multi-bounce ray reflection logic
3D shape support for AABB3, Sphere, and zCylinder
3D nearest-point queries
3D overlap testing between all shape combinations
3D raycasting against all supported shape types
Object grabbing and camera-relative object movement
Mouse and keyboard input support across 2D and 3D modes

Known Issues:

None currently known.