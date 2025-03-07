# Zumo328P Library

**Zumo328P** is a library designed for reading and managing encoder counts and controlling the speed of the Zumo Shield for Arduino v1.2 using the ATmega328P microcontroller (such as the Arduino Uno). This library provides an efficient and reliable way to monitor motor rotation, enabling accurate tracking of movement and direction.

## Background

This encoder-based implementation is derived from the Zumo 32U4 library and specifically adapted for the Zumo Shield for Arduino v1.2. It is designed to work with external encoders such as the Magnetic Encoder Pair Kit for Micro Metal Gearmotors, 12 CPR, 2.7–18V from (https://www.pololu.com/product/3081) Pololu. The PID algorithm has been created to control motor direction and speed, improving line-following capabilities.

### Pin Configuration Changes

- **Left Encoder XORed Input**: Mapped to **Pin 2**.
- **Right Encoder XORed Input**: Mapped to **Pin 3**.
- **Left Encoder Input**: Mapped to **Pin 6**.
- **Right Encoder Input**: Mapped to **Pin 12**.
- **Note**:
  - **Pin 2 and 3**: The use of I2C is no longer possible due to the use of external interrupts for reading encoder signals.
  - **Pin 12**: Used for a user push button, so it can no longer be used for that purpose.
  - **Pin 6**: Connected to the buzzer via a jumper; the jumper must be removed to enable encoder functionality.

### Implementation Details

On the Zumo 32U4 board, an XOR chip was used to reduce the required number of interrupt pins for encoders. This has been addressed in this library through software-based solutions, allowing the same functionality using fewer hardware resources. We use `attachInterrupt()` instead of directly defining `ISR(INTx_vect)` to ensure compatibility with other code that also utilizes `attachInterrupt()`.

## Features

- **Ported for ATmega328P**: Adapted specifically for use with the Arduino Uno and compatible boards.
- **Encoder Count Monitoring**: Tracks both left and right encoders using interrupts for high accuracy.
- **Seamless Integration**: Uses `attachInterrupt()` for ease of compatibility with other code.

## Installation

1. Clone or download this repository.
2. Attach the `Zumo328P` folder in your Arduino `libraries` directory.
3. Download dependencies such as FAST GPIO, Zumo Shield from Library Manager
4. Restart the Arduino IDE and include the library in your sketch.

## Encoder Frequency and Speed Calculation for Zumo Robot

This guide explains how to calculate the frequency and speed of a Zumo robot using encoders with a specified resolution.

### 1. Frequency Calculation from Encoder Counts

The encoder provides 12 counts per revolution (CPR) of the motor shaft when counting both edges of both channels. To compute the counts per revolution of the drive sprockets, you multiply the gearbox’s gear ratio by 12.

For example, if 75:1 motors are used (with a more accurate gear ratio of 75.81:1), the encoder provides approximately:

CPR = 75.81 * 12 ≈ 909.7

To calculate the frequency of encoder counts per second, use the following formula:

f_enc = (RPM * CPR) / 60

Where:
- `RPM` is the revolutions per minute of the motor.
- `CPR` is the counts per revolution of the drive sprockets (909.7 in this case).
- The division by 60 converts RPM to revolutions per second.

#### Example

For a motor running at 200 RPM, the encoder count frequency would be:

f_enc = (200 * 909.7) / 60 ≈ 3032.33 counts/s

### 2. Speed Calculation (Linear Speed)

To calculate the linear speed of the robot, you need to convert the number of rotations of the drive sprockets into linear motion. This can be done if you know the radius of the wheels.

v = 2 * π * r * (RPM / 60)

Where:
- `r` is the radius of the wheel.
- `v` is the linear velocity of the robot in meters per second (m/s).

#### Example

The radius of the Zumo wheel is `r = 0.039 m / 2 = 0.0195 m`.

For a motor running at 200 RPM, the linear velocity would be:

v = 2 * π * 0.0195 * (200 / 60) ≈ 1.63 m/s

For a motor running at 400 RPM, the linear velocity would be:

v = 2 * π * 0.0195 * (400 / 60) ≈ 3.27 m/s


### 3. Rotation Speed Calculation (RPM and Angular Velocity)

To compute the **rotation speed (RPM)** from encoder ticks, use the following formula:

RPM = (N / CPR) * (60 / t)

Where:
- `N` = Number of **measured encoder ticks**.
- `CPR` = **Counts per Revolution** of the encoder (**909.7** in this case).
- `t` = Measurement **time in seconds**.
- `60` = Converts **seconds to minutes**.

#### Angular Velocity Calculation
To convert **RPM to Angular Velocity** in **radians per second (rad/s)**:

ω = (2 * π * RPM) / 60

#### Example
If the encoder records **5000 ticks** in **2 seconds**, the RPM would be:

RPM = (5000 / 909.7) * (60 / 2)

RPM = (5000 / 909.7) * 30 ≈ 164.7 RPM

To get the **angular velocity**:

ω = (2 * π * 164.7) / 60 ≈ 17.24 rad/s

### 4. Distance Calculation

To calculate the **distance traveled**, use the formula:

s = (N / CPR) * U

Where:
- `s` = Distance traveled (**in cm or m**).
- `N` = Number of **encoder ticks** counted.
- `CPR` = Encoder's **Counts per Revolution**.
- `U` = **Wheel circumference** calculated as:

U = π * d = 2 * π * r

#### Example
- **CPR = 1000**
- **Wheel diameter = 1.95 cm**
- **Measured ticks = 5000**
  
Wheel circumference: U = π * 1.95 = 6.13 cm

s = (5000 / 1000) * 6.13 = 30.65 cm

## Developers
- **Mutasem Bader** 
- **Felix Fritz Biermann**
