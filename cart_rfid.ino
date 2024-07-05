#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your network credentials
const char* ssid = "Panda";
const char* password = "panda1601";

// Create an instance of the server
ESP8266WebServer server(80);

// Initialize the LCD with the I2C address (commonly 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  
  // Print IP address to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(3, 0);
  lcd.print(WiFi.localIP());//comment this line befor submiting it contains ipaddress
  
  // Define routing
  server.on("/", handleRoot);
  server.on("/submit", HTTP_POST, handleSubmit);
  
  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Enter Text</h1>";
  html += "<form action='/submit' method='POST'>";
  html += "<input type='text' name='text' placeholder='Enter text'><br>";
  html += "<input type='submit' value='Submit'>";
  html += "</form>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

void handleSubmit() {
  if (server.hasArg("text")) {
    String text = server.arg("text");
    Serial.print("Received text: ");
    Serial.println(text);
    
    // Display the text on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(text);
    
    server.send(200, "text/html", "<html><body><h1>Text displayed on LCD!</h1></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: No text received!</h1></body></html>");
  }
}
