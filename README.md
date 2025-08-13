# Agriprobe Simulation

## Overview
This project simulates the control logic for the Agriprobe soil sampling system using Zephyr RTOS.
It runs in `native_sim` mode with no hardware required.

## Hardware Model
- **Z-axis stepper** – moves auger up and down on a lead screw.
- **Auger motor** – drills into soil (forward) or unloads (reverse).
- **Plate stepper** – rotates test tubes under the auger.

## Process
1. Home Z-axis.
2. Home plate.
3. Drill down, spin forward to collect soil.
4. Drill up.
5. Rotate plate to next empty slot.
6. Spin reverse to unload soil.
7. Repeat until all slots are marked full.

## Simulation Features
- Step count–based motion simulation.
- Homing sequences for both Z-axis and plate.
- Slot tracking with state machine control loop.

## Build & Run
```bash
west build -p always -b

## Software Architecture Plan
- **motor_control.cpp / .h**
  - Functions for Z-axis stepper, plate stepper, auger motor.
  - Homing, movement by steps, speed control.

- **slot_manager.cpp / .h**
  - Tracks full/empty tube states.
  - Finds next empty slot.

- **main.cpp**
  - Initializes system, runs main control loop.
  - Calls motor_control and slot_manager functions.

This modular design will make it easier to replace simulation code with hardware control later.

