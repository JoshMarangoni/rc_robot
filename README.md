# rc_robot_firmware

## Remote-Controlled Robot Firmware

### Requirements

- Remote controlled using video feed
- Programmed in C/C++
- Battery powered
- All electronics contained on one PCB
- Map "WASD" keys to driving direction
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
  - Create PCB to hold all electronics
  - 3D print custom chassis
  - Optmize software
- Phase 3 (Bonus)
  - Do not use Arduino
  - Build H-bridge using discrete MOSFET drivers
  - Use smallest and cheapest parts possible, minimize COGS

### Notes
- even though a motor draws 0.2 A while running it can produce a stall current of 2+ A, causing the driver to underpower the arduino
  - meaured the 5V output of the Arduino and saw the voltage hang around 3 V sometimes dropping even lower as the motors change directions. I need to either put the motors on a separate power supply or use large capacitors to buffer voltage spikes
- Got video streaming over Bluetooth by changing ArduCAM's serial prints to use SerialBT, then using a BT virtual COM port for the host app
- Tried combining all the camera source code, bluetooth serial, http webserver, and motor controls all in one program, but the program is too large for the ESP32. Further, I do not want the camera streaming software to be delayed by running motor controls and http server responses in the same control loop. I could use FreeRTOS to split up these tasks, but that seems overkill. There is no need to have all this code on one processor when I can buy an integrated esp32-CAM that I can dedicate to video streaming. Having the robot be controlled over WIFI is also superior to Bluetooth as it allows me to operate the vehicle compleletely remotely as long as I have the WIFI router's IP address and port forwarding setup.




