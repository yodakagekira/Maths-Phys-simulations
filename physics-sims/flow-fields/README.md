# 3D Wind & Tornado Flowfield Simulation (Raylib)

A real-time 3D particle flowfield simulation written in C++ using Raylib.
The project models wind-like atmospheric motion with a structured tornado
vortex combined with turbulence. Particle trails reveal the underlying
flow structure.

---

## Features

- 3D wind flowfield simulation
- Tornado-style vortex:
  - rotational swirl
  - radial inflow
  - vertical updraft
- Background atmospheric wind shear
- Turbulence via 3D Perlin / fBm noise
- Thousands of particles with trail-based visualisation
- Configurable boundary behaviour (wrap / bounce / respawn)
- Free 3D camera controls

---

## Controls

- WASD + Mouse : Free camera
- T            : Toggle trails
- 1            : Wrap bounds
- 2            : Bounce bounds
- 3            : Respawn particles

---

## Project Structure

src/
- main.cpp        : Application entry point, rendering, wind field
- particle.hpp    : Particle definition
- particle.cpp    : Particle integration, forces, trails
- perlin.hpp      : Noise interface
- perlin.cpp      : Perlin / fBm noise implementation

---

## Build (Windows / MinGW)
Build:
g++ src/perlin.cpp src/particle.cpp src/main.cpp -o main.exe \
    -lraylib -lopengl32 -lgdi32 -lwinmm

Run:
./main.exe

---

## Wind Model

The wind field combines structured flow and noise:
swirl, inflow, and updraft define the tornado core, while
wind shear and low-amplitude noise add realistic atmospheric motion.

---

## Author

Ahmad Abdullah
