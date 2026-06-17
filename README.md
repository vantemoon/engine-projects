# Custom Engine Projects

A collection of game projects and technical demos built with my custom C++ game engine.

This repository showcases gameplay programming, rendering, engine systems, developer tools, and real-time interaction.

<img width="1148" height="572" alt="image" src="https://github.com/user-attachments/assets/2cd433a1-5b9d-48b5-8327-22f4a079e7d4" />


## Projects

Each folder contains a separate project built on top of the custom engine. Projects explore areas such as gameplay systems, input, cameras, physics, UI, animation, shaders, and 2D/3D rendering.

## Technical Highlights

- **Developer Console Tools**  
  Runtime commands for testing gameplay, debugging systems, and quickly triggering features.

- **Data-Driven Gameplay**  
  XML-based definitions for gameplay content, making systems easier to tune and expand.

- **Custom Rendering**  
  Engine-level rendering work including textured geometry, vertex buffers, shaders, lighting, and cameras.

- **Gameplay State Machines**  
  Organized logic for player actions, game flow, and interaction states.

- **Debug Features**  
  Debug rendering and test tools for visualizing gameplay data and checking edge cases.

## Technologies

- C++
- Custom C++ game engine
- Visual Studio
- Real-time rendering
- Shader programming
- XML data files
- Developer console tools

## Build and Run

### Requirements

- Windows 10 or later
- Visual Studio 2022
- C++ desktop development workload
- Windows SDK
- Git

### Steps

1. Clone the repository:

   `git clone https://github.com/vantemoon/engine-projects.git`

2. Open the project folder you want to run.

3. Open the `.sln` file in Visual Studio.

4. Set the game project as the startup project.

5. Select `Debug x64` or `Release x64`.

6. Build and run with **Local Windows Debugger**.

## Notes

Some projects may require the correct working directory so the executable can find its `Data` folder. If assets, shaders, textures, or XML files fail to load, check the project’s Visual Studio debugging settings.

## Purpose

This repository highlights my experience building gameplay features and engine systems from the ground up.

## Author

Created by [Sylvia Sun](https://github.com/vantemoon).
