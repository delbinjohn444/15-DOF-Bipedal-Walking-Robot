# 15-DOF Bipedal Walking Robot

A bipedal humanoid-style walking robot built from scratch — NodeMCU (ESP8266) + PCA9685 PWM driver + 15x MG995/MG996R servos, with a browser-based slider interface for live pose tuning and a captured-pose walking gait.

<img width="600" height="800" alt="IMG_9765" src="https://github.com/user-attachments/assets/65ab85e3-bc13-4321-a623-572d048746c3" />

## Status

- ✅ Assembled and standing (head, arms, legs)
- ✅ Servo channel mapping confirmed
- ✅ Web-based slider control (per-channel angle tuning over WiFi AP)
- ✅ Static in-place weight-shift + lift tested
- ✅ Forward stepping sequence working (9-phase captured gait)
- 🔧 In progress: smoothing the loop-back transition, stride length tuning, speed tuning
- ⏳ Planned: turning gait, IMU-based balance correction

## Hardware

| Component | Qty | Notes |
|---|---|---|
| NodeMCU (ESP8266) | 1 | Main controller, hosts WiFi AP + web UI |
| PCA9685 16-channel PWM driver | 1 | I2C servo driver |
| MG995 / MG996R servos | 15 | 1 head, 3+3 arms, 4+4 legs |
| External 5-6V servo power supply | 1 | Rated 3-4A+ — do not power servos from NodeMCU |

### Wiring

| PCA9685 | NodeMCU |
|---|---|
| VCC | 3V3 |
| GND | GND (common with servo power GND) |
| SDA | D2 (GPIO4) |
| SCL | D1 (GPIO5) |
| V+ | External 5-6V supply |

## Servo channel map

| Channel | Joint | Axis |
|---|---|---|
| CH0 | Head | Pan |
| CH1 | L Shoulder | Roll |
| CH2 | L Shoulder | Pitch |
| CH3 | L Elbow | — |
| CH4 | R Shoulder | Roll |
| CH5 | R Shoulder | Pitch |
| CH6 | R Elbow | — |
| CH7 | L Hip | Roll |
| CH8 | L Hip | Pitch |
| CH9 | L Knee | — |
| CH10 | L Ankle | — |
| CH11 | R Hip | Roll |
| CH12 | R Hip | Pitch |
| CH13 | R Knee | — |
| CH14 | R Ankle | — |
| CH15 | *(spare)* | — |

L/R = the robot's own left and right.

## Repo structure

```
firmware/
  idle_pose.ino          - sets all 15 servos to 90 (calibration baseline)
  channel_test.ino        - moves one channel at a time for verification
  slider_control.ino      - WiFi AP + web UI with per-leg sliders, live tuning
  walking_sequence.ino     - captured 9-phase forward-walking gait (legs only)
media/
  standing.jpg            - robot standing, front view
  leg_side.jpg             - single leg side profile
```

## Using the slider control tool

1. Flash `firmware/slider_control.ino`.
2. Connect to the WiFi network `15-DOF` (password in the sketch).
3. Open `http://192.168.4.1` in a browser.
4. Drag sliders to find poses; use "Copy current pose as code" to grab the 8 leg-channel angles as a ready-to-paste array.

## Walking gait

The current gait is a 9-phase static walk cycle (weight shift → lift → swing → place → shift), captured by hand using the slider tool rather than derived analytically. See `firmware/walking_sequence.ino` for the full phase sequence and captured angle values.

## Known issues / next steps

- Loop-back transition between the last phase and the first phase of the next cycle is not yet perfectly smooth — needs a recaptured connecting pose.
- Stride length and speed are conservative (tuned for stability first); both can likely be increased now that the base cycle works.
- No turning gait yet.
- Balance is fully open-loop (no IMU feedback) — sensitive to uneven surfaces.

## License

MIT — see [LICENSE](LICENSE).
