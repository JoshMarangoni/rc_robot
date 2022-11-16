# rc_robot_firmware

## Remote-Controlled Robot Firmware

### Requirements

- Remote controlled using video feed
- Programmed in C/C+
- Battery powered
- Map "WASD" keys to driving direction
- chassis that securely holds PCB, motors, batteries, and camera

### Better-ifs

- Non-arduino based coding envrionment
- Write compilers manually
- Custom 3D printed chassis
- Programmed in C/C+
- Single power source
- All electronics contained on one PCB
- No lag between key presses and robot response (FreeRTOS?)

### Design 1 (MVP) - DONE

- ESP 32 microcontroller programmed in Arduino
- 2 wheels/motors in the rear (RWD)
- 1 wheel/motor in the front
- esp32CAM for video streaming
- Plexiglass chassis

### Design 2 (Feature Complete) - In Progress

- ESP32 programmed not in arduino
- Use custom Makefiles
- Custom 3D printed chassis
- PCB designed in KiCAD and fabricated by JLC PCB

## Planning
- get 3D printer working________________________Nov.19 (Sat)
- receive MOSFETs_______________________________Nov.19 (Sat)
- get H-bridges working_________________________Nov.20 (Sun)
- test singular power source with capacitor_____Nov.20 (Sun)
- finish PCB design and send order for fab______Nov.27 (Sun)
- make CAD drawing for chassis and 3D print_____Dec.4  (Sun)
- receive PCB___________________________________Dec.7  (Wed)
- solder components on PCB and test_____________Dec.11 (Sun)
- finish all software __________________________Dec.18 (Sun)

### Notes
- even though a motor draws 0.2 A while running it can produce a stall current of 2+ A, causing the driver to underpower the arduino
  - meaured the 5V output of the Arduino and saw the voltage hang around 3 V sometimes dropping even lower as the motors change directions. I need to either put the motors on a separate power supply or use large capacitors to buffer voltage spikes
- Got video streaming over Bluetooth by changing ArduCAM's serial prints to use SerialBT, then using a BT virtual COM port for the host app
- Tried combining all the camera source code, bluetooth serial, http webserver, and motor controls all in one program, but the program is too large for the ESP32. Further, I do not want the camera streaming software to be delayed by running motor controls and http server responses in the same control loop. I could use FreeRTOS to split up these tasks, but that seems overkill. There is no need to have all this code on one processor when I can buy an integrated esp32-CAM that I can dedicate to video streaming. Having the robot be controlled over WIFI is also superior to Bluetooth as it allows me to operate the vehicle compleletely remotely as long as I have the WIFI router's IP address and port forwarding setup.
