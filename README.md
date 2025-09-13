# Agriprobe Simulation

## Overview
This project is control system for the Agriprobe soil sampling system using Zephyr RTOS.

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
west build -p always -b native_sim .
./build/zephyr/zephyr.exe



