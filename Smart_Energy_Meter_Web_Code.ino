#include <ESP8266WiFi.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include <ESP8266WebServer.h>

// Wi-Fi credentials - replace with your own credentials or use external configuration
#define WIFI_SSID "your_ssid"            // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "your_password"    // Replace with your Wi-Fi password

// Static IP configuration - change as needed
#define STATIC_IP_ADDRESS { 192, 168, 140, 4 } 

// Pin definitions for PZEM module
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13

SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);

WiFiServer server(80);

// Variables for power data
unsigned long lastReadingTime = 0; // Last time reading was taken
float totalPowerConsumed = 0;      // Total power consumed in watt-hours (Wh)

void setup() {
    Serial.begin(115200);

    // Set up fixed IP address
    IPAddress staticIP(STATIC_IP_ADDRESS);
    IPAddress gateway(192, 168, 137, 1); // Gateway IP address
    IPAddress subnet(255, 255, 255, 0);  // Subnet mask
    WiFi.config(staticIP, gateway, subnet);

    // Connect to Wi-Fi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("WiFi connected");

    // Start the web server
    server.begin();
    Serial.println("Server started");

    // Print the IP address
    Serial.println(WiFi.localIP());
}

void loop() {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    // Wait until the client sends some data
    while (!client.available()) {
        delay(1);
    }

    // Read the HTTP request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Prepare the HTTP response
    String powerData = getPowerData();
    String webpage = generateWebPage(powerData);

    // Send the HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(webpage);

    // Close the connection
    delay(10);
    client.stop();
}

String getPowerData() {
    // Read the data from the module
    float power = pzem.power();
    float pf = pzem.pf();

    // Check if the data is valid
    if (!isnan(power) && !isnan(pf)) {
        // Calculate time difference since last reading
        unsigned long currentTime = millis();
        unsigned long timeDiff = currentTime - lastReadingTime;
        lastReadingTime = currentTime;

        // Convert time difference to hours
        float timeHours = timeDiff / (1000.0 * 3600.0); // milliseconds to hours

        // Accumulate power consumed in watt-hours (Wh)
        float powerConsumed = power * timeHours;
        totalPowerConsumed += powerConsumed;

        // Convert total power consumed to kilowatt-hours (kWh)
        float totalPowerConsumed_kWh = totalPowerConsumed / 1000.0; // Wh to kWh

        // Build the power data string
        String powerData = "<div>Power: " + String(power) + " W</div>";
        powerData += "<div>Power Factor: " + String(pf) + "</div>";
        powerData += "<div>Total Power Consumed: " + String(totalPowerConsumed_kWh, 2) + " kWh</div>";
        return powerData;
    } else {
        return "Error reading data from module";
    }
}

String generateWebPage(String powerData) {
    String webpage = "<!DOCTYPE html><html><head><title>SMART ENERGY METER</title>";
    webpage += "<style>body{font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 0; background-color: #f2f2f2;}";
    webpage += ".container{max-width: 800px; margin: 50px auto; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);}";
    webpage += "h1{text-align: center; color: #333;}";
    webpage += "p{text-align: center; color: #666; font-size: 18px;}";
    webpage += ".data{margin-top: 20px; padding: 20px; background-color: #f9f9f9; border-radius: 10px;}";
    webpage += ".data-title{text-align: center; font-size: 24px; font-weight: bold;}";
    webpage += ".data-item{text-align: center; font-size: 20px;}";
    webpage += ".btn-reset{display: block; margin: 20px auto; padding: 10px 20px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer;}";
    webpage += ".btn-reset:hover{background-color: #45a049;}";
    webpage += ".log-sheet{margin-top: 30px; border-collapse: collapse; width: 100%;}";
    webpage += ".log-sheet th, .log-sheet td{border: 1px solid #ddd; padding: 8px; text-align: left;}";
    webpage += ".log-sheet th{background-color: #4CAF50; color: white;}";
    webpage += "</style></head>";
    webpage += "<body>";
    webpage += "<div class='container'>";
    webpage += "<h1>SMART ENERGY METER</h1>";
    webpage += "<div class='data'>";
    webpage += "<div class='data-title'>Power Data</div>";
    webpage += "<div class='data-item'>" + powerData + "</div>";
    webpage += "<button class='btn-reset' onclick='resetValues()'>Reset Values</button>";
    webpage += "</div>";
    webpage += "<h2>Data Log Sheet</h2>";
    webpage += "<table class='log-sheet'>";
    webpage += "<tr><th>Date & Time</th><th>Power (W)</th><th>Power Factor</th><th>Total Power Consumed (kWh)</th></tr>";
    // Add data log rows here
    webpage += "</table>";
    webpage += "</div>";
    webpage += "<script>";
    webpage += "function resetValues() {";
    webpage += "if (confirm('Are you sure you want to reset the values?')) {";
    webpage += "location.reload();"; // Reload the page to reset the values
    webpage += "}";
    webpage += "}";
    webpage += "</script>";
    webpage += "</body></html>";
    return webpage;
}
