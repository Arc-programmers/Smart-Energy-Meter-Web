# Smart-Energy-Meter-Web
## **Code Documentation for Smart Energy Meter Web**

### **Overview**

This project uses an ESP8266 microcontroller to create a smart energy meter that measures power consumption and serves the data via a web server. The PZEM-004T sensor module is used to measure power data, and the data is displayed on a web page hosted by the ESP8266.

#### **Libraries:**

  ESP8266WiFi.h: Enables Wi-Fi connectivity for the ESP8266.
  PZEM004Tv30.h: Interfaces with the PZEM-004T v3.0 power sensor.
  SoftwareSerial.h: Creates a software serial port for communication with the PZEM-004T module.
  ESP8266WebServer.h: Creates a web server on the ESP8266.

#### **Constants**

  WIFI_SSID: Wi-Fi network SSID.
  WIFI_PASSWORD: Wi-Fi network password.
  STATIC_IP_ADDRESS: Desired fixed IP address for the ESP8266.
  PZEM_RX_PIN: GPIO pin for receiving data from the PZEM module.
  PZEM_TX_PIN: GPIO pin for transmitting data to the PZEM module.

#### **Global Variables**

  pzemSWSerial: Software serial object for PZEM communication.
  pzem: PZEM object for power measurement.
  server: Web server object on port 80.
  lastReadingTime: Timestamp of the last power reading.
  totalPowerConsumed: Accumulated power consumption in watt-hours (Wh).

#### **Functions**

  #### void setup()
    Initializes the serial communication.
    Configures a fixed IP address, gateway, and subnet mask.
    Connects to the Wi-Fi network.
    Starts the web server and prints the assigned IP address.

  #### void loop()
    Checks if a client has connected to the web server.
    Reads the HTTP request from the client.
    Generates the web page with power data.
    Sends the HTTP response to the client.
    Closes the client connection.
    
  #### String getPowerData()
    Reads power and power factor data from the PZEM module.
    Calculates the time difference since the last reading.
    Converts the time difference to hours and calculates the power consumed.
    Accumulates the total power consumed.
    Returns a formatted string containing the power data.
    
  #### String generateWebPage(String powerData)
    Generates an HTML web page containing the power data.
    Includes styles and scripts for displaying and resetting data.
    Formats the data log sheet (currently empty) for future expansion.
