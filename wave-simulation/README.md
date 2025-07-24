# Wave Simulation with Shaders

An interactive 3D wave simulation using OpenGL shaders and Allegro 5.

## Features
- Real-time wave rendering using vertex shaders
- Interactive mouse controls (click to create ripples)
- Dynamic lighting with specular highlights
- Adjustable wave parameters

## Controls
- **Q/A** - Increase/Decrease wave speed
- **W/S** - Increase/Decrease wave height  
- **E/D** - Increase/Decrease wave frequency
- **Mouse Click** - Create ripples
- **ESC** - Exit

## Dependencies
- Allegro 5 with OpenGL support
- OpenGL headers and libraries
- C++11 compiler

## Building

### Option 1: System-wide Allegro
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install liballegro5-dev libgl1-mesa-dev

# Build
make -f Makefile.simple
```

### Option 2: Local Allegro
If you have Allegro installed locally, update the Makefile with the correct paths.

### Running
```bash
cd build
./wave_simulation
```

## Troubleshooting

If you get OpenGL header errors, install:
```bash
# Ubuntu/Debian
sudo apt-get install libgl1-mesa-dev mesa-common-dev

# Or for minimal OpenGL support
sudo apt-get install libglu1-mesa-dev freeglut3-dev
```