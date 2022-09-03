# rc_robot_firmware

## Remote-Controlled Robot Firmware

### Requirements

- Remote controlled using video feed
- Streams camera video over Wifi
- Programmed in C
- Battery powered
- Discrete MOSFET motor drivers
- All electronics contained on one PCB
- Map keys to driving direction
- 3D printed chassis that securely holds PCB, motors, batteries, and camera

### Design 1

- ESP 32 microcontroller
- 2 wheels/motors in the rear (RWD)
- 1 wheel/motor in the front

### Design 2

- Non-arduino based microcontroller
- Discrete Wifi ASIC

### Timeline

- Phase 1 (MVP)
  - Breadboard circuitry
  - Any scrap chassis, motors and wheels
  - Write initial software
- Phase 2 (Feature-Complete)
  - Complete PCB design rev1 and fabricate
  - Complete chassis design rev1 and fabricate
  - Decide on final motors and wheels
  - Finish all software features
- Phase 3 (Design 2)
  - Select new microcontroller
  - Select WIFI chip
  - Update firmware to support new hardware
  - Update PCB layout and fabricate
  - Update chassis and fabricate
