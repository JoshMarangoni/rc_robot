import serial

ser = serial.Serial(port = "COM3", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

while(1):
    if(ser.in_waiting > 0):
        byte = ser.read()
        print(byte)
