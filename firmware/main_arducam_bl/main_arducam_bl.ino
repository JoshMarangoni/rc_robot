
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

#include <WiFi.h>

#define IN1 18
#define IN2 19
#define IN3 22
#define IN4 23
#define LED 32

String Drive_State = "off";

const char* ssid = "";
const char* password = "";

WiFiServer server(80);

String header;

// Auxiliar variables to store the current output state
String LED_State = "off";

// Current time
unsigned long currentTime = millis();

// Previous time
unsigned long previousTime = 0;

// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void straight() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, HIGH);
    digitalWrite(IN3, LOW);
}

void left() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(IN3, HIGH);
}

void right() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN4, HIGH);
    digitalWrite(IN3, LOW);
}

void off() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(IN3, LOW);
}

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

    // motors
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    off();
    Serial.println("Motor Driver Initialized");

    // webserver
    pinMode(LED, OUTPUT);
    digitalWrite(LED, HIGH);
    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
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

    WiFiClient client = server.available();   // Listen for incoming clients

    if (client)
    {
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client.");          // print a message out in the serial port
        String currentLine = "";                // make a String to hold incoming data from the client

        while (client.connected() && currentTime - previousTime <= timeoutTime)
        {
            currentTime = millis();

            if (client.available())
            {
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                header += c;

                if (c == '\n'){
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        // turns the GPIOs on and off
                        if (header.indexOf("GET /LED/on") >= 0)
                        {
                            Serial.println("LED on");
                            LED_State = "on";
                            digitalWrite(LED, HIGH);
                        }
                        else if (header.indexOf("GET /LED/off") >= 0)
                        {
                            Serial.println("LED off");
                            LED_State = "off";
                            digitalWrite(LED, LOW);
                        }
                        else if (header.indexOf("GET /drive/straight") >= 0)
                        {
                            Serial.println("DRIVE STRAIGHT");
                            Drive_State = "straight";
                            straight();
                        }
                        else if (header.indexOf("GET /drive/off") >= 0)
                        {
                            Serial.println("END DRIVE");
                            Drive_State = "off";
                            off();
                        }

                        // Display the HTML web page
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");
                        // CSS to style the on/off buttons
                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                        client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".button2 {background-color: #555555;}</style></head>");

                        // Web Page Heading
                        client.println("<body><h1>ESP32 Robot Web Server</h1>");

                        // Display current state, and ON/OFF buttons for LED
                        client.println("<p>LED - State " + LED_State + "</p>");
                        // If the LED is off, it displays the ON button

                        if (LED_State=="off")
                        {
                            client.println("<p><a href=\"/LED/on\"><button class=\"button\">ON</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/LED/off\"><button class=\"button button2\">OFF</button></a></p>");
                        }
                        client.println("<p><a href=\"/drive/straight\"><button class=\"button\">DRIVE</button></a></p>");
                        client.println("<p><a href=\"/drive/off\"><button class=\"button\">STOP</button></a></p>");
                        client.println("</body></html>");

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                  }
                  else
                  {   // if you got a newline, then clear currentLine
                      currentLine = "";
                  }
               }
            else if (c != '\r')
            {   // if you got anything else but a carriage return character,
                currentLine += c;      // add it to the end of the currentLine
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
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
