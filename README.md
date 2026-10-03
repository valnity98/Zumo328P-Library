# Zumo328P Library

> **Status: completed.** University project (WiSe 2024/25), kept as a reference implementation.

**Arduino encoder and PID library for the Zumo Shield (Arduino Leonardo / ATmega32U4)**

A port of the Pololu Zumo 32U4 encoder library, adapted for the Zumo Shield v1.2 driven by an Arduino Leonardo (ATmega32U4). The library name comes from the Zumo 328P shield variant, and it also compiles for the ATmega328P. Adds a discrete-time PD controller for steering, enabling accurate line-following and odometry on the Arduino Leonardo.

Developed as part of the Master's course *Autonomous Intelligent Systems* (Mechatronics & Robotics, Frankfurt UAS, WiSe 2024/2025).

---

## Background

The original Zumo 32U4 uses an on-board XOR chip to reduce the required interrupt pins for quadrature encoders. This library replaces that hardware XOR with a software equivalent, enabling the same encoder functionality on the Leonardo's external-interrupt pins D2 and D3. It is designed for use with the [Pololu Magnetic Encoder Pair Kit for Micro Metal Gearmotors, 12 CPR](https://www.pololu.com/product/3081).

---

## Pin Mapping

| Signal | Arduino Leonardo Pin | Notes |
|---|---|---|
| Left encoder A (XOR input) | **D2** | External interrupt INT1 |
| Right encoder A (XOR input) | **D3** | External interrupt INT0 |
| Left encoder B | **D6** | Remove buzzer jumper on Zumo Shield |
| Right encoder B | **D12** | Replaces the user push-button |

> **Note:** Using D2 and D3 for encoder interrupts disables I²C (SDA/SCL share the same lines on some shields). D12 can no longer be used as the user button.

---

## Features

- Interrupt-driven quadrature decoding via `attachInterrupt()` for compatibility with other libraries
- Signed 32-bit tick counters with atomic read (interrupt-safe `cli()`/`sei()`)
- Discrete-time PD controller (proportional + derivative) that turns the lateral line position error into left/right motor speeds
  (the full PID with anti-windup runs on the PC side, in the ROS 2 package)
- Compatible with the ZumoRobot-ROS 2 project (binary serial protocol)

---

## Installation

1. Clone or download this repository.
2. Copy the `library/` folder into your Arduino `libraries/` directory.
3. Install dependencies via Arduino Library Manager:
   - **ZumoShield** (Pololu)
   - **FastGPIO** (Pololu)
4. Restart the Arduino IDE.

---

## Usage

### Encoder counting

```cpp
#include <Zumo328PEncoders.h>

Zumo328PEncoders encoders;

void loop() {
    int32_t left  = encoders.getCountsLeft();
    int32_t right = encoders.getCountsRight();
    // or reset-on-read:
    // int32_t left = encoders.getCountsAndResetLeft();
}
```

### ROS 2 integration example

See [`example/ZumoRos2.ino`](example/ZumoRos2.ino)  for a full sketch that:
- Receives motor speed commands from a ROS 2 node over serial (7-byte framed protocol)
- Replies with encoder counts (10-byte framed protocol)
- Supports encoder reset via control byte

---

## Encoder Maths Reference

### Counts per Revolution

```
CPR = gear_ratio × 12 counts/rev
    = 75.81 × 12 ≈ 909.7 counts/rev  (for the 75:1 motor, without XOR doubling)
```

### Linear Speed

```
v = 2π × r × (RPM / 60)        [m/s]
  = 2π × 0.0195 × (RPM / 60)
```

### Distance from Ticks

```
s = (N / CPR) × (2π × r)       [m]
```

### RPM from Ticks

```
RPM = (N / CPR) × (60 / t)
```

where `N` = ticks counted, `t` = measurement interval in seconds.

---

## License

Copyright (c) 2026 Mutasem Bader — All Rights Reserved.  
Viewing is permitted. Copying, modifying, or submitting as own work is strictly prohibited.  
See [LICENSE](LICENSE) for details.
