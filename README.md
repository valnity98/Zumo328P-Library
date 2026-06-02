# Zumo328P Library

**Arduino encoder and PID library for the Zumo Shield (ATmega328P / Arduino Uno)**

A port of the Pololu Zumo 32U4 encoder library, adapted for the Zumo Shield v1.2 with an ATmega328P microcontroller. Adds a discrete-time PID controller for motor speed and direction control, enabling accurate line-following and odometry on the Arduino Uno.

---

## Background

The original Zumo 32U4 uses an on-board XOR chip to reduce the required interrupt pins for quadrature encoders. This library replaces that hardware XOR with a software equivalent, enabling the same encoder functionality on the Uno's two external-interrupt pins (D2 and D3). It is designed for use with the [Pololu Magnetic Encoder Pair Kit for Micro Metal Gearmotors, 12 CPR](https://www.pololu.com/product/3081).

---

## Pin Mapping

| Signal | Arduino Uno Pin | Notes |
|---|---|---|
| Left encoder A (XOR input) | **D2** | External interrupt INT0 |
| Right encoder A (XOR input) | **D3** | External interrupt INT1 |
| Left encoder B | **D6** | Remove buzzer jumper on Zumo Shield |
| Right encoder B | **D12** | Replaces the user push-button |

> **Note:** Using D2 and D3 for encoder interrupts disables I²C (SDA/SCL share the same lines on some shields). D12 can no longer be used as the user button.

---

## Features

- Interrupt-driven quadrature decoding via `attachInterrupt()` for compatibility with other libraries
- Signed 32-bit tick counters with atomic read (interrupt-safe `cli()`/`sei()`)
- Discrete-time PID controller with proportional, integral, and derivative terms
- Anti-windup integral clamping and derivative low-pass filter
- Compatible with the ZumoRobot-ROS 2 project (binary serial protocol)

---

## Installation

1. Clone or download this repository.
2. Copy the `Zumo328P Arduino/` folder into your Arduino `libraries/` directory.
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

See [`example/ZumoRos2/ZumoRos2.ino`](example/ZumoRos2/ZumoRos2.ino) for a full sketch that:
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

## Contributors

- **Mutasem Bader**
- **Felix Fritz Biermann**

---

## License

Apache-2.0
