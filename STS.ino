#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = D3; // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4;  // Configurable, see typical pin layout above
MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

const char* ssid = "Panda";
const char* password = "panda1601";

ESP8266WebServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

String trolleyItems[10];
float trolleyPrices[10];
int itemCount = 0;
String tag;

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(3, 0);
  lcd.print(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/add", HTTP_POST, handleAdd);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  
  if (!rfid.PICC_IsNewCardPresent())
    return;
    
  if (rfid.PICC_ReadCardSerial()) {
    tag = "";
    for (byte i = 0; i < 4; i++) {
      tag += String(rfid.uid.uidByte[i], HEX);
    }
    Serial.println("RFID Tag: " + tag);
    
    if (tag == "18185232137" || tag == "2293818595" || tag == "3393818595") {
      addItem("Item" + tag, 10.99); // Assuming same price for all items for simplicity
    } else {
      Serial.println("Access Denied!");
      digitalWrite(D8, HIGH);
      delay(2000);
      digitalWrite(D8, LOW);
    }
    
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Smart Trolley System</h1>";
  html += "<form action='/add' method='POST'>";
  html += "<input type='text' name='item' placeholder='Enter item'><br>";
  html += "<input type='number' step='0.01' name='price' placeholder='Enter price'><br>";
  html += "<input type='submit' value='Add Item'>";
  html += "</form><br>";
  html += "<h2>Current Items:</h2><ul>";
  
  for (int i = 0; i < itemCount; i++) {
    html += "<li>" + trolleyItems[i] + ": $" + String(trolleyPrices[i], 2) + "</li>";
  }
  
  html += "</ul></body></html>";
  
  server.send(200, "text/html", html);
}

void handleAdd() {
  if (server.hasArg("item") && server.hasArg("price")) {
    String item = server.arg("item");
    float price = server.arg("price").toFloat();
    Serial.print("Adding item: ");
    Serial.print(item);
    Serial.print(" with price: $");
    Serial.println(price);
    
    if (itemCount < 10) {
      trolleyItems[itemCount] = item;
      trolleyPrices[itemCount] = price;
      itemCount++;
      updateLCD();
      
      server.send(200, "text/html", "<html><body><h1>Item added!</h1><a href='/'>Go Back</a></body></html>");
    } else {
      server.send(200, "text/html", "<html><body><h1>Trolley is full!</h1><a href='/'>Go Back</a></body></html>");
    }
  } else {
    server.send(400, "text/html", "<html><body><h1>Error: No item or price received!</h1><a href='/'>Go Back</a></body></html>");
  }
}

void addItem(String item, float price) {
  if (itemCount < 10) {
    trolleyItems[itemCount] = item;
    trolleyPrices[itemCount] = price;
    itemCount++;
    Serial.print("Added item: ");
    Serial.print(item);
    Serial.print(" with price: $");
    Serial.println(price);
    updateLCD();
  } else {
    Serial.println("Trolley is full!");
  }
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Items:");
  
  for (int i = 0; i < itemCount; i++) {
    if (i < 2) {
      lcd.setCursor(0, i + 1);
      lcd.print(trolleyItems[i]);
      lcd.print(" $");
      lcd.print(trolleyPrices[i], 2);
    }
  }
}
