# OOCatcher Project - Text Version

An implementation of an anthropomorphic body simulation that can catch objects and throw snowballs. 

## Overview

The OOCatcher project implements an anthropomorphic body using line segments and generates a sequence of moves to:
1. Walk towards and catch an object (Walker scenario)
2. Throw a snowball at a target (Snowball scenario)

The body maintains anthropomorphic limitations on segment rotations and ensures the segments stay connected while moving.

## This Text Version

Due to limitations in the environment, this version provides a text-based interface to the simulation. The physics and movement algorithms are fully implemented, but visualization is provided through text status updates rather than graphics.

## Key Features

- Anthropomorphic body with connected segments
- Physics-based movement and constraints
- Two scenarios:
  - Walker: Body walks to catch an object
  - Snowball: Body throws a snowball at a target
- Text-based visualization of simulation status
- Configurable parameters
- Logging system for movement sequences

## How to Use

1. Run the program with `./TextOOCatcher`
2. Use the following commands:
   - s - Execute a single step
   - a - Auto-execute all steps
   - r - Reset simulation
   - w - Switch to Walker scenario
   - b - Switch to Snowball scenario
   - q - Quit

## Technical Implementation

The project is implemented in C++ with an object-oriented design. Key classes include:

- `Body`: Manages the anthropomorphic body and its segments
- `Segment`: Represents a single line segment with rotation capabilities
- `Circle`: Models circular objects (targets and snowballs)
- `Vector2D`: Provides vector mathematics functionality
- `Walker` and `Snowball`: Implements the two main scenarios
- `TextSimulation`: Manages the overall simulation with text output
- `Logger`: Provides logging functionality

## Build Instructions

To build the project:

```bash
g++ -std=c++17 -I./include -o TextOOCatcher src/TextMain.cpp src/Vector2D.cpp src/Segment.cpp src/Circle.cpp src/Body.cpp src/Walker.cpp src/Snowball.cpp src/Logger.cpp
```

## Configuration

The project uses a configuration file at `config/default.cfg` which allows customization of:
- Initial positions
- Ground level
- Target size
- Gravity
- Default scenario type