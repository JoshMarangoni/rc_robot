import serial

ser = serial.Serial(port = "COM4", baudrate=115200, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)

dataObtained = False

with open("data.jpg", "wb") as myfile:
    while(1):
        if(ser.in_waiting > 0):
            byte = ser.read()
            myfile.write(byte)
            print(byte)
            if not dataObtained:
                dataObtained = True
        else:
            if dataObtained:
                break
