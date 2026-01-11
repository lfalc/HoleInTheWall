# HoleInTheWall

An interactive game project built on STM32L476 microcontroller hardware with real-time sensor input processing and game logic.

**[Full Documentation](https://lfalc.github.io/HoleInTheWall/)**

## Project Description

HoleInTheWall is an embedded systems project that implements a dynamic game on an STM32L476RGTx microcontroller. The game features real-time graphics rendering, sensor-based input handling, and game state management. The project combines low-level hardware programming with interactive gameplay mechanics.

## Features & Functionality

- **Real-Time Game Engine**: Implements a GameFrame system for rendering and a GameTimer for precise timing
- **Sensor Input Processing**: Processes analog sensor data with DMA (Direct Memory Access) support for efficient data transfer
- **GPIO Control**: Manages digital I/O pins for hardware interaction
- **Serial Communication**: USART2 interface for debugging and potential external communication
- **Dynamic Game Mechanics**: Features wall generation and collision detection with the player
- **Hardware Optimization**: Utilizes FPU (Floating Point Unit) and DMA to maximize performance

## File Structure Overview

### Firmware
The main embedded firmware for the STM32L476 microcontroller:

```
firmware/
├── Src/                           # Source files
│   ├── main.c                    # Main program entry point
│   ├── GameFrame.c               # Game rendering and display logic
│   ├── GameTimer.c               # Game timing and frame control
│   ├── GPIO.c                    # GPIO configuration and control
│   ├── sensor.c                  # Analog sensor input handling
│   ├── USART2.c                  # Serial communication driver
│   ├── DMA1.c                    # Direct Memory Access configuration
│   └── syscalls.c, sysmem.c      # System functions
├── Inc/                           # Header files
│   ├── GameFrame.h, GameTimer.h
│   ├── GPIO.h, sensor.h
│   ├── USART2.h, DMA1.h
│   └── [corresponding headers]
├── Startup/                       # STM32 startup files
├── CMSIS/                         # ARM CMSIS-Core files
└── Debug/                         # Build artifacts
```

### Web Interface
```
HoleInTheWall/
├── index.html                     # Game display interface
├── styles.css                     # UI styling
└── README.md                      # This file
```