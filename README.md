# Intelligent Traffic Light System (IS1300 HT24)

Real-time traffic control system for the **STM32 Nucleo-L476RG** and **Traffic Light Shield**.
Implements a comprehensive state machine for managing intersections, pedestrian crossings, and remote configuration via UART.

## Project Scope

This project implements **Tasks 1, 2, 3, and 5** of the IS1300 specifications.

### Implemented Modules

#### Task 1: Pedestrian Crossing (Upper)
*   **File**: `task1.c`
*   **Function**: Manages the upper pedestrian crossing.
*   **Logic**:
    *   Default: Car Green, Pedestrian Red.
    *   On Button Press: Indicator toggles (`toggleFreq`), then Car sequence (Green -> Orange -> Red).
    *   Walk Phase: Pedestrian Green for `walkingDelay`.
    *   Safety: Pedestrian Red whenever Cars are Green/Orange.

#### Task 2: Smart Road Intersection
*   **File**: `task2.c`
*   **Function**: Manages the main 4-way intersection (ignoring pedestrians).
*   **Logic**:
    *   Yields right-of-way based on active car sensors (TL1-TL4).
    *   Prevents overlapping paths (Vertical vs Horizontal).
    *   Implements "Ghost Town" (switch on timeout) and "Yield" (switch on demand) logic.
    *   Timings: `greenDelay`, `orangeDelay`, `redDelayMax`.

#### Task 3: Integrated Control
*   **File**: `task3.c`
*   **Function**: Combines Task 1 and Task 2 into a single cohesive system.
*   **Logic**:
    *   Prioritizes safety: Only one pedestrian crossing active at a time.
    *   Allows Right Turns: Cars turn right when right-lane crossing is green.
    *   **Driver**: Controls 74HC595 Shift Registers via **SPI** (`led_driver.c`) to drive the shield LEDs.

#### Task 5: Remote Configuration (UART)
*   **File**: `task5.c`
*   **Function**: Runtime configuration via UART2 (Virtual COM Port).
*   **Protocol**: 4-Byte Hex Commands.
    *   `Byte 1`: Message ID.
    *   `Byte 2`: `0x00` (Reserved).
    *   `Byte 3-4`: Value (Unsigned 16-bit).
*   **Configurable Parameters**:
    *   `0x01`: `toggleFreq`
    *   `0x02`: `pedestrianDelay`
    *   `0x03`: `walkingDelay`
    *   `0x04`: `orangeDelay`
*   **Validation**: Ensures rules (e.g., `pedestrianDelay > orangeDelay`) are met before applying.

## Software Architecture

### Directory Structure
*   `Core/Src/`: Application Logic.
    *   `main.c`: RTOS Scheduler setup and hardware init.
    *   `freertos.c`: Task creation and hooks.
    *   `taskX.c`: Specific implementation tasks.
    *   `led_driver.c`: Hardware abstraction for LED Shield (SPI).
    *   `input.c`: Hardware abstraction for Buttons/Sensors.
    *   `config.c`: Global configuration repository.
    *   `unit_test.c`: Test suite.
*   `Core/Inc/`: Header files defining public interfaces.

### Hardware Mapping
*   **Actuators**: Traffic Light Shield LEDs (Shift Register via SPI).
*   **Inputs**:
    *   **PL1/PL2**: Pedestrian Buttons.
    *   **TL1-TL4**: Car Presence Sensors.
*   **Comms**: UART2 (115200 Baud) for Task 5.

## Testing Strategy

The project includes a dedicated test suite (`unit_test.c`).
To run tests, enable the configuration in `main.c` (or call specific test functions):

1.  **Unit Tests**: `test_leds()`, `test_switches()`, `test_uart()`.
2.  **Logic Verification**: `test_task3()` runs the full integrated loop.
3.  **Config Validation**: `test_task5()` validates parameter guardrails.

## Requirements Checklist
*   [x] **R0.1**: Project Naming Convention.
*   [x] **R1.1-R1.6**: Pedestrian Logic.
*   [x] **R2.1-R2.8**: Intersection Logic.
*   [x] **R3.1-R3.5**: Integration & SPI Driver.
*   [x] **R5.1-R5.3**: UART Configuration Protocol.
