# DirectX11 Racetrack Simulation

A real-time 3D racetrack simulation written in C++ using DirectX 11, HLSL, the Hieroglyph 3 rendering engine, and DirectXTK.

The scene follows an autonomous sports car as it navigates a checkpoint-defined circuit. Two independently moving bulls patrol their own routes while trees sway, the sky transitions over time, and directional, point, and vehicle-mounted spotlights illuminate the environment. A movable first-person camera and on-screen diagnostics make it possible to observe the simulation from different positions.

## Features

- Autonomous car navigation around a 31-point racetrack route
- Smooth steering towards successive checkpoints
- Two independently animated bulls following separate patrol paths
- Textured OBJ model loading for the car and wildlife
- Directional, point, and car-mounted spotlight calculations
- Dynamic skybox blending that produces a day-night transition
- Procedurally assembled trees with asynchronous wind-sway animation
- Textured racetrack, trees, flag, sky, vehicle, and wildlife
- Free-moving first-person observation camera
- HUD showing car position, orientation, speed, FPS, and world matrix
- Screenshot capture to BMP
- Custom HLSL shaders for lit materials and skybox blending

## Controls

| Key or input | Action |
| --- | --- |
| <kbd>W</kbd> / <kbd>S</kbd> | Move the camera forward or backward |
| <kbd>A</kbd> / <kbd>D</kbd> | Move the camera left or right |
| <kbd>Q</kbd> / <kbd>E</kbd> | Move the camera up or down |
| Hold right mouse button and move the mouse | Rotate the camera |
| Hold <kbd>Space</kbd> | Increase the camera movement speed |
| Release <kbd>Space</kbd> | Capture a BMP screenshot |
| <kbd>Esc</kbd> | Exit the application |

The car and bulls are controlled by the simulation and do not require player input.

## Requirements

- Windows 10 or Windows 11
- A DirectX 11-compatible graphics device
- Visual Studio 2022 with the **Desktop development with C++** workload
- MSVC v143 platform toolset
- Windows 10 SDK

The repository includes the Hieroglyph 3 source and SDK, DirectXTK, and Lua 5.1.4. A separate installation of these dependencies is not required for the supplied Visual Studio solution.

## Building and running

1. Clone the repository:

   ```powershell
   git clone https://github.com/JackGore02/directx-11-racetrack-simulation.git
   cd directx-11-racetrack-simulation
   ```

2. Open `DirectX11 Racetrack Simulation.sln` in Visual Studio.
3. Select either **Debug** or **Release** and the **Win32** platform.
4. In Solution Explorer, right-click **DirectX11 Racetrack Simulation** and select **Set as Startup Project**.
5. Choose **Build > Build Solution** or press <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd>.
6. Run without the debugger using <kbd>Ctrl</kbd>+<kbd>F5</kbd>.

You can also build from a Visual Studio Developer Command Prompt:

```powershell
msbuild "DirectX11 Racetrack Simulation.sln" /p:Configuration=Release /p:Platform=Win32
```

Runtime models, shaders, and textures are stored under `Applications/Data`. The compiled executable is written to `Applications/Bin` and expects the data directory to remain in its supplied relative location.

## Simulation

- The car continuously follows a closed route defined by 31 checkpoints.
- Steering is limited by an angular speed, producing gradual turns rather than instant direction changes.
- Each bull follows a separate looping patrol route and alternates between movement and turning states.
- Sixteen trees are assembled from textured trunk and canopy geometry and sway with varied phase and amplitude values.
- The skybox blends between two textures over time to create a repeating environmental transition.
- Directional light intensity changes with the sky transition.
- A spotlight follows the car's position and forward direction to behave like a headlight.
- A point light provides an additional local coloured light source.
- The HUD continuously reports the car's transform information and the current frame rate.

## Project structure

```text
DirectX11 Racetrack Simulation.sln
|-- Applications/
|   |-- DirectX11 Racetrack Simulation/
|   |   |-- LJMULevelDemo.*             # Scene creation, simulation, lighting, and HUD
|   |   |-- LJMUGeometryActor.*         # Geometry generation and material setup
|   |   |-- LJMUMeshOBJ.*               # OBJ mesh loading
|   |   |-- LJMUSpriteOverlay.*         # 2D sprite overlay rendering
|   |   `-- LJMUTextOverlay.*           # Diagnostic text rendering
|   `-- Data/
<<<<<<< Updated upstream
|       |-- Models/                     # Car, bull, and example OBJ models
|       |-- Shaders/                    # HLSL rendering shaders
|       `-- Textures/                   # Scene, model, sky, and HUD textures
|-- Source/                             # Hieroglyph 3 engine source
|-- SDK/
|   |-- Include/                        # Hieroglyph public headers
|   `-- Library/                        # Hieroglyph static libraries
`-- Dependencies/
    |-- DirectXTK/                      # DirectX helper library
    `-- lua-5.1.4/                      # Bundled Lua source and library project
=======
|       |-- Models/                      # Car, bull, and example OBJ models
|       |-- Shaders/                     # HLSL rendering shaders
|       `-- Textures/                    # Scene, model, sky, and HUD textures
|-- Source/                              # Hieroglyph 3 engine source
|-- SDK/
|   |-- Include/                         # Hieroglyph public headers
|   `-- Library/                         # Hieroglyph static libraries
`-- Dependencies/
    |-- DirectXTK/                       # DirectX helper library
    `-- lua-5.1.4/                       # Bundled Lua source and library project
>>>>>>> Stashed changes
```

## How it works

`LJMULevelDemo` configures the Direct3D 11 renderer, render targets, depth buffer, scene cameras, geometry, materials, textures, lights, and overlays. Its update loop advances the car and bulls, animates the trees, updates the lighting and sky transition, renders the scene, and draws the diagnostic HUD.

The car uses a sequence of world-space checkpoints. Each frame, the simulation calculates a normalised direction towards the next checkpoint, rotates the car towards that direction at a limited angular speed, and moves it forward at a fixed linear speed. Once the car passes a checkpoint, the indices advance and wrap around to keep the route continuous. The bulls use related path-following logic with separate routes and movement states.

`LJMUGeometryActor` creates procedural geometry and configures the custom lit and skybox materials. `LJMULitTexture.hlsl` combines directional, point, and spot lighting, while `LJMUSkyboxTexture.hlsl` blends two sky textures using a time-dependent shader parameter. Hieroglyph 3 supplies the renderer, scene graph, event system, camera, resources, and Direct3D 11 pipeline abstractions.

## Disclaimer

This project was developed as commissioned coursework for university assessment. The source code presented here is published solely for educational and portfolio purposes only. No permission is granted to reproduce, distribute, submit, or adapt it without prior written permission.
<<<<<<< Updated upstream
=======

I was the sole developer of the application-specific scene, simulation, and rendering work. The repository also contains third-party framework and library code used by the supplied coursework template.
>>>>>>> Stashed changes
