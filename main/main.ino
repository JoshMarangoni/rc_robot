
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include <Wire.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

//This demo can only work on OV2640_MINI_2MP or OV5642_MINI_5MP or OV5642_MINI_5MP_BIT_ROTATION_FIXED platform.
#if !( defined OV2640_MINI_2MP_PLUS)
#error Please select the hardware platform and camera module in the ../libraries/ArduCAM/memorysaver.h file
#endif

#define BMPIMAGEOFFSET 66

const char bmp_header[BMPIMAGEOFFSET] PROGMEM =
{
    0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
    0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
    0x00, 0x00
};

const int CS = 5; // slave select pin
bool is_header = false;
int mode = 0;
uint8_t start_capture = 0;

ArduCAM myCAM( OV2640, CS );

uint8_t read_fifo_burst(ArduCAM myCAM);

void setup() {
    uint8_t vid, pid, temp;
    Wire.begin();
    Serial.begin(115200);

    SerialBT.println(F("ACK CMD ArduCAM Start! END"));
    SerialBT.begin("ESP32test"); //Bluetooth device name
    SerialBT.println("Initialization Successful.");
    SerialBT.println("The device started, now you can pair it with bluetooth!");

    // set the CS as an output:
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);

    // initialize SPI:
    SPI.begin();

    //Reset the CPLD
    myCAM.write_reg(0x07, 0x80);
    delay(100);
    myCAM.write_reg(0x07, 0x00);
    delay(100);

    while (1)
    {
        //Check if the ArduCAM SPI bus is OK
        myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
        temp = myCAM.read_reg(ARDUCHIP_TEST1);
        if (temp != 0x55)
        {
            SerialBT.println(F("ACK CMD SPI interface Error!END"));
            delay(1000);
            continue;
        }
        else
        {
            SerialBT.println(F("ACK CMD SPI interface OK.END"));
            break;
        }
    }

    while (1)
    {
        //Check if the camera module type is OV2640
        myCAM.wrSensorReg8_8(0xff, 0x01);
        myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
        myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
        if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 )))
        {
            SerialBT.println(F("ACK CMD Can't find OV2640 module!"));
            delay(1000); continue;
        }
        else
        {
            SerialBT.println(F("ACK CMD OV2640 detected.END")); break;
        }
    }
    //Change to JPEG capture mode and initialize the OV5642 module
    myCAM.set_format(JPEG);
    myCAM.InitCAM();
    myCAM.OV2640_set_JPEG_size(OV2640_320x240);
    delay(1000);
    myCAM.clear_fifo_flag();
}

void loop() {
    uint8_t temp = 0xff, temp_last = 0;
    bool is_header = false;

    if (SerialBT.available())
    {
        temp = SerialBT.read();
        switch (temp)
        {
            case 0:
                myCAM.OV2640_set_JPEG_size(OV2640_160x120); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_160x120END"));
                temp = 0xff;
                break;
            case 1:
                myCAM.OV2640_set_JPEG_size(OV2640_176x144); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_176x144END"));
                temp = 0xff;
                break;
            case 2:
                myCAM.OV2640_set_JPEG_size(OV2640_320x240); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_320x240END"));
                temp = 0xff;
                break;
            case 3:
                temp = 0xff;
                myCAM.OV2640_set_JPEG_size(OV2640_352x288); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_352x288END"));
                break;
            case 4:
                temp = 0xff;
                myCAM.OV2640_set_JPEG_size(OV2640_640x480); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_640x480END"));
                break;
            case 5:
                temp = 0xff;
                myCAM.OV2640_set_JPEG_size(OV2640_800x600); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_800x600END"));
                break;
            case 6:
                temp = 0xff;
                myCAM.OV2640_set_JPEG_size(OV2640_1024x768); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_1024x768END"));
                break;
            case 7:
                temp = 0xff;
                myCAM.OV2640_set_JPEG_size(OV2640_1280x1024); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_1280x1024END"));
                break;
            case 8:
                temp = 0xff;
                myCAM.OV2640_set_JPEG_size(OV2640_1600x1200); delay(1000);
                SerialBT.println(F("ACK CMD switch to OV2640_1600x1200END"));
                break;
            case 0x10:
                mode = 1;
                temp = 0xff;
                start_capture = 1;
                SerialBT.println(F("ACK CMD CAM start single shoot.END"));
                break;
            case 0x11:
                temp = 0xff;
                SerialBT.println(F("ACK CMD Change OK.END"));
                myCAM.set_format(JPEG);
                myCAM.InitCAM();
                myCAM.OV2640_set_JPEG_size(OV2640_320x240); 
                break;
            case 0x20:
                mode = 2;
                temp = 0xff;
                start_capture = 2;
                SerialBT.println(F("ACK CMD CAM start video streaming.END"));
                break;
            case 0x30:
                mode = 3;
                temp = 0xff;
                start_capture = 3;
                SerialBT.println(F("ACK CMD CAM start single shoot.END"));
                break;
            case 0x31:
                temp = 0xff;
                myCAM.set_format(BMP);
                myCAM.InitCAM();
                break;
            default:
                break;
        }
    }
    if (mode == 1)
    {
        if (start_capture == 1)
        {
            myCAM.flush_fifo();
            myCAM.clear_fifo_flag();
            //Start capture
            myCAM.start_capture();
            start_capture = 0;
        }
        if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        {
            SerialBT.println(F("ACK CMD CAM Capture Done.END"));
            delay(50);
            read_fifo_burst(myCAM);
            //Clear the capture done flag
            myCAM.clear_fifo_flag();
        }
    }
    else if (mode == 2)
    {
        while (1)
        {
            temp = SerialBT.read();
            if (temp == 0x21)
            {
            start_capture = 0;
            mode = 0;
            SerialBT.println(F("ACK CMD CAM stop video streaming.END"));
            break;
            }
            if (start_capture == 2)
            {
            myCAM.flush_fifo();
            myCAM.clear_fifo_flag();
            //Start capture
            myCAM.start_capture();
            start_capture = 0;
            }
            if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
            {
            uint32_t length = 0;
            length = myCAM.read_fifo_length();
            if ((length >= MAX_FIFO_SIZE) | (length == 0))
            {
                myCAM.clear_fifo_flag();
                start_capture = 2;
                continue;
            }
            myCAM.CS_LOW();
            myCAM.set_fifo_burst();//Set fifo burst mode
            temp =  SPI.transfer(0x00);
            length --;
            while ( length-- )
            {
                temp_last = temp;
                temp =  SPI.transfer(0x00);
                if (is_header == true)
                {
                SerialBT.write(temp);
                }
                else if ((temp == 0xD8) & (temp_last == 0xFF))
                {
                is_header = true;
                SerialBT.println(F("ACK CMD IMG END"));
                SerialBT.write(temp_last);
                SerialBT.write(temp);
                }
                if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
                break;
                delayMicroseconds(15);
            }
            myCAM.CS_HIGH();
            myCAM.clear_fifo_flag();
            start_capture = 2;
            is_header = false;
            }
        }
    }
    else if (mode == 3)
    {
        if (start_capture == 3)
        {
            //Flush the FIFO
            myCAM.flush_fifo();
            myCAM.clear_fifo_flag();
            //Start capture
            myCAM.start_capture();
            start_capture = 0;
        }
        if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
        {
            SerialBT.println(F("ACK CMD CAM Capture Done.END"));
            delay(50);
            uint8_t temp, temp_last;
            uint32_t length = 0;
            length = myCAM.read_fifo_length();
            
            if (length >= MAX_FIFO_SIZE )
            {
                SerialBT.println(F("ACK CMD Over size.END"));
                myCAM.clear_fifo_flag();
                return;
            }
            if (length == 0 ) //0 kb
            {
                SerialBT.println(F("ACK CMD Size is 0.END"));
                myCAM.clear_fifo_flag();
                return;
            }
            myCAM.CS_LOW();
            myCAM.set_fifo_burst();//Set fifo burst mode

            SerialBT.write(0xFF);
            SerialBT.write(0xAA);

            for (temp = 0; temp < BMPIMAGEOFFSET; temp++)
            {
                SerialBT.write(pgm_read_byte(&bmp_header[temp]));
            }
            char VH, VL;
            int i = 0, j = 0;
    
            for (i = 0; i < 240; i++)
            {
                for (j = 0; j < 320; j++)
                {
                    VH = SPI.transfer(0x00);;
                    VL = SPI.transfer(0x00);;
                    SerialBT.write(VL);
                    delayMicroseconds(12);
                    SerialBT.write(VH);
                    delayMicroseconds(12);
                }
            }

            SerialBT.write(0xBB);
            SerialBT.write(0xCC);
            myCAM.CS_HIGH();
            //Clear the capture done flag
            myCAM.clear_fifo_flag();
        }
    }
}

uint8_t read_fifo_burst(ArduCAM myCAM)
{
    uint8_t temp = 0, temp_last = 0;
    uint32_t length = 0;
    length = myCAM.read_fifo_length();
    SerialBT.println(length, DEC);

    if (length >= MAX_FIFO_SIZE) //512 kb
    {
        SerialBT.println(F("ACK CMD Over size.END"));
        return 0;
    }
    
    if (length == 0 ) //0 kb
    {
        SerialBT.println(F("ACK CMD Size is 0.END"));
        return 0;
    }
    
    myCAM.CS_LOW();
    myCAM.set_fifo_burst();//Set fifo burst mode
    temp =  SPI.transfer(0x00);
    length --;

    while ( length-- )
    {
        temp_last = temp;
        temp =  SPI.transfer(0x00);

        if (is_header == true)
        {
            SerialBT.write(temp);
        }
        else if ((temp == 0xD8) & (temp_last == 0xFF))
        {
            is_header = true;
            SerialBT.println(F("ACK CMD IMG END"));
            SerialBT.write(temp_last);
            SerialBT.write(temp);
        }
        if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
        {
            break;
        }
        delayMicroseconds(15);
    }
    myCAM.CS_HIGH();
    is_header = false;

    return 1;
}
