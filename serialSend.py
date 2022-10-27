import serial

ser = serial.Serial(port = "COM6", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

ser.write(0x10)
