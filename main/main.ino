
#include <WiFi.h>

#define IN1 18
#define IN2 19
#define IN3 22
#define IN4 23

String header;

const char* ssid = "";
const char* password = "";

WiFiServer server(80);

unsigned long currentTime = millis();

unsigned long previousTime = 0;

const long timeoutTime = 2000;  // ms

void straight()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, HIGH);
    digitalWrite(IN3, LOW);
}

void left()
{
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(IN3, HIGH);
}

void right()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN4, HIGH);
    digitalWrite(IN3, LOW);
}

void off()
{
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN4, LOW);
    digitalWrite(IN3, LOW);
}

void setup()
{
    Serial.begin(115200);

    // motors
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    off();
    Serial.println("Motor Driver Initialized");

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

void loop()
{
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client)
    {
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client.");
        String currentLine = "";

        while (client.connected() && currentTime - previousTime <= timeoutTime)
        {
            currentTime = millis();

            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                header += c;
                if (c == '\n')
                {
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
                        
                        if (header.indexOf("GET /drive/straight") >= 0)
                        {
                            Serial.println("DRIVE STRAIGHT");
                            straight();
                        }
                        else if (header.indexOf("GET /drive/left") >= 0)
                        {
                            Serial.println("TURNING LEFT");
                            left();
                        }
                        else if (header.indexOf("GET /drive/right") >= 0)
                        {
                            Serial.println("TURNING RIGHT");
                            right();
                        }
                        else if (header.indexOf("GET /drive/off") >= 0)
                        {
                            Serial.println("END DRIVE");
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
                {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }
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
