# Zumo328P Library

> **Status: completed.** University project (WiSe 2024/25), kept as a reference implementation.

**Arduino encoder and PD-controller library for the Zumo Shield (Arduino Leonardo / ATmega32U4)**

A port of the Pololu Zumo 32U4 encoder library, adapted for the Zumo Shield v1.2 driven by an Arduino Leonardo (ATmega32U4). It also compiles for the ATmega328P. Adds a discrete-time PD controller for steering, enabling accurate line-following and odometry on the Arduino Leonardo. The controller class is named `Zumo328PPID`; it implements a PD controller (proportional and derivative term, no integral term).

Developed as part of the Master's course *Autonomous Intelligent Systems* (Mechatronics & Robotics, Frankfurt UAS, WiSe 2024/2025).

---

## Background

Pololu's `Zumo32U4Encoders` reads the encoders of the Zumo 32U4, where an XOR of the two encoder channels is routed to a single interrupt pin. This library does not need an XOR chip: it attaches an external interrupt (`CHANGE`) to channel A of each encoder (D2 and D3 on the Leonardo) and reads channel B inside the interrupt service routine to determine the direction of rotation. Only the edges of channel A are counted (see [Counts per Revolution](#counts-per-revolution)). It is designed for use with the [Pololu Magnetic Encoder Pair Kit for Micro Metal Gearmotors, 12 CPR](https://www.pololu.com/product/3081).

---

## Pin Mapping

| Signal | Arduino Leonardo Pin | Notes |
|---|---|---|
| Left encoder A | **D2** | External interrupt INT1, triggers on `CHANGE` |
| Right encoder A | **D3** | External interrupt INT0, triggers on `CHANGE` |
| Left encoder B | **D6** | Read in the ISR to determine direction. Remove buzzer jumper on Zumo Shield |
| Right encoder B | **D12** | Read in the ISR to determine direction. Replaces the user push-button |

> **Note:** Using D2 and D3 for encoder interrupts disables I²C (SDA/SCL share the same lines on some shields). D12 can no longer be used as the user button.

---

## Features

- Interrupt-driven quadrature decoding via `attachInterrupt()` (edges of channel A, direction from channel B) for compatibility with other libraries
- Signed 32-bit tick counters with atomic read (interrupt-safe `cli()`/`sei()`)
- Discrete-time PD controller (proportional + derivative) that turns the lateral line position error into left/right motor speeds
  (the full PID with anti-windup runs on the PC side, in the ROS 2 package [ZumoRobot-ROS2](https://github.com/valnity98/ZumoRobot-ROS2))
- Compatible with the [ZumoRobot-ROS2](https://github.com/valnity98/ZumoRobot-ROS2) project (binary serial protocol)

---

## Installation

1. Clone or download this repository.
2. Copy the `library/` folder into your Arduino `libraries/` directory.
3. Install dependencies via Arduino Library Manager:
   - **ZumoShield** (Pololu)
   - **FastGPIO** (Pololu)
   - **Zumo32U4** (Pololu), only for `example/ZumoRos2/ZumoRos2.ino` on the ATmega32U4
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

See [`example/ZumoRos2/ZumoRos2.ino`](example/ZumoRos2/ZumoRos2.ino) for a full sketch that:
- Receives motor speed commands from a ROS 2 node over serial (7-byte framed protocol)
- Replies with encoder counts (10-byte framed protocol)
- Supports encoder reset via control byte

---

## Encoder Maths Reference

### Counts per Revolution

The number of counts per wheel revolution (CPR) depends on how the encoder signals are decoded. Pololu specifies 12 counts per motor revolution for the magnetic encoders when the edges of both channels are counted. This library counts only the edges of channel A, which gives 6 counts per motor revolution:

```
CPR = gear_ratio × 6 counts/rev
    = 75.81 × 6 ≈ 455 counts/rev  (for a 75.81:1 gearmotor)
```

Calibrate CPR for your own robot: turn the wheel by exactly one revolution, read the tick difference, and use the measured value as `CPR` in the formulas below.

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

## Third-party code

`library/Zumo328PEncoders.cpp` and `library/Zumo328PEncoders.h` are derived from `Zumo32U4Encoders` in Pololu's [zumo-32u4-arduino-library](https://github.com/pololu/zumo-32u4-arduino-library) (MIT License, Copyright (c) 2015-2022 Pololu Corporation). See [THIRD-PARTY-NOTICES.md](THIRD-PARTY-NOTICES.md) for the license text.

---

## License

Copyright (c) 2026 Mutasem Bader — All Rights Reserved.  
Viewing is permitted. Copying, modifying, or submitting as own work is strictly prohibited.  
See [LICENSE](LICENSE) for details.
