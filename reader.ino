#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

ESP8266WebServer server(80);
const char* ssid = "Ranjith samsung";
const char* password = "ranjith11";
LiquidCrystal_I2C lcd(0x27, 16, 2);
int p1 = 0, p2 = 0, p3 = 0, p4 = 0;
float c1 = 0.0, c2 = 0.0, c3 = 0.0, c4 = 0.0;
int count_prod = 0;
float total = 0.0;

void setup() {
  pinMode(D3, INPUT_PULLUP);
  pinMode(D4, OUTPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Wire.begin(D2, D1);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   WELCOME TO       ");
  lcd.setCursor(0, 1);
  lcd.print("   SMART CART       ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 0);
    lcd.print("WiFi connecting...          ");
  }
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected          ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(1500);
  server.on("/", []() {
    String page = "<html><head><title>E Cart using IoT</title></head><style type=\"text/css\">";
    page += "table{border-collapse: collapse;}th {background-color:  #3498db ;color: white;}table,td {border: 4px solid black;font-size: x-large;";
    page += "text-align:center;border-style: groove;border-color: rgb(255,0,0);}</style><body><center>";
    page += "<h1>Smart Shopping Cart using IoT</h1><br><br><table style=\"width: 1200px;height: 450px;\"><tr>";
    page += "<th>ITEMS</th><th>QUANTITY</th><th>COST</th></tr><tr><td>Biscuit</td><td>"+String(p1)+"</td><td>"+String(c1)+"</td></tr>";
    page += "<tr><td>Soap</td><td>"+String(p2)+"</td><td>"+String(c2)+"</td></tr><tr><td>Rice(1KG)</td><td>"+String(p3)+"</td><td>"+String(c3)+"</td>";
    page += "</tr><tr><td>Tea(50g)</td><td>"+String(p4)+"</td><td>"+String(c4)+"</td></tr><tr><th>Grand Total</th><th>"+String(count_prod)+"</th><th>"+String(total)+"</th>";
    page += "</tr></table><br><input type=\"button\" name=\"Pay Now\" value=\"Pay Now\" style=\"width: 200px;height: 50px\"></center></body></html>";
    page += "<meta http-equiv=\"refresh\" content=\"2\">";
    server.send(200, "text/html", page);
  });
  server.begin();
}

void loop() {
  server.handleClient();
  int a = digitalRead(D3);
  int count = 0;
  char input[12];
  while (Serial.available() && count < 12) {
    input[count] = Serial.read();
    count++;
    delay(5);
  }
  input[count] = '\0'; // Null terminate the input array
  if ((strncmp(input, "002A573E3959", 12) == 0) && (a == 1)) { // Check if RFID tag matches and button is pressed
    lcd.setCursor(0, 0);
    lcd.print("Biscuit Added       ");
    lcd.setCursor(0, 1);
    lcd.print("Price(Rs):35.00      ");
    p1++;
    digitalWrite(D4, HIGH);
    delay(2000);
    total = total + 35.00;
    count_prod++;
    digitalWrite(D4, LOW);
    lcd.clear();
  } else if ((strncmp(input, "002A573E3959", 12) == 0) && (a == 0)) { // Check if RFID tag matches and button is released
    if (p1 > 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Biscuit Removed!!!        ");
      digitalWrite(D4, HIGH);
      delay(2000);
      p1--;
      total = total - 35.00;
      count_prod--;
      lcd.clear();
      digitalWrite(D4, LOW);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Not in cart!!!        ");
      digitalWrite(D4, HIGH);
      delay(2000);
      digitalWrite(D4, LOW);
      lcd.clear();
    }
  }
  c1 = p1 * 35.00;
  c2 = p2 * 38.00;
  c3 = p3 * 55.00;
  c4 = p4 * 45.00;
}
