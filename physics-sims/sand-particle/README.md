# Falling Sand Simulation (raylib, C++)

A real-time **falling-sand / cellular automata** simulation written in **C++** using **raylib**.  
The project models simple material interactions such as gravity, fluid flow, fire spread, and combustion using a discrete grid.


## Features

- **Materials**
  - Air
  - Sand (falls, piles)
  - Water (flows, spreads)
  - Stone (static)
  - Oil (flammable, flows)
  - Fire (spreads, burns out)

- **Physics / Rules**
  - Gravity & diagonal settling
  - Fluid lateral spreading
  - Fire propagation & lifetime decay
  - Randomized scan order to reduce bias

- **Interaction**
  - Paint materials with mouse
  - Adjustable brush size
  - Pause / resume simulation
  - Live material switching

---

## Controls

| Input | Action |
|-----|------|
| **1** | Sand |
| **2** | Water |
| **3** | Stone |
| **4** | Oil |
| **5** | Fire |
| **Left Click** | Place selected material |
| **Right Click** | Erase (Air) |
| **← / →** | Decrease / Increase brush size |
| **Space** | Pause / Resume |

---

## Project Structure

