# OOCatcher Project

An implementation of an anthropomorphic body simulation that can catch objects and throw snowballs.
## Overview

The OOCatcher project implements an anthropomorphic body using line segments and generates a sequence of moves to:
1. Walk towards and catch an object (Walker scenario)
2. Throw a snowball at a target (Snowball scenario)

The body maintains anthropomorphic limitations on segment rotations and ensures the segments stay connected while moving.

## Key Features

- Anthropomorphic body with connected segments
- Physics-based movement and constraints
- Two scenarios:
  - Walker: Body walks to catch an object
  - Snowball: Body throws a snowball at a target
- Visualization using SFML
- Configurable parameters
- Logging system for movement sequences

## Technical Implementation

The project is implemented in C++ with an object-oriented design. Key classes include:

- `Body`: Manages the anthropomorphic body and its segments
- `Segment`: Represents a single line segment with rotation capabilities
- `Circle`: Models circular objects (targets and snowballs)
- `Vector2D`: Provides vector mathematics functionality
- `Walker` and `Snowball`: Implements the two main scenarios
- `Simulation`: Manages the overall simulation and visualization
- `Logger`: Provides logging functionality

## Build Instructions

To build the project, you'll need:
- C++ compiler with C++17 support
- SFML library for visualization

### Linux/Mac

```bash
# Install SFML (Ubuntu/Debian)
sudo apt-get install libsfml-dev

# Install SFML (Mac with Homebrew)
brew install sfml

# Build the project
g++ -std=c++17 -o OOCatcher src/*.cpp -lsfml-graphics -lsfml-window -lsfml-system
