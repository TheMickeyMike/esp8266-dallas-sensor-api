#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "......";
const char* password = "......";

WiFiServer server(80);


void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(2, OUTPUT);
  digitalWrite(2, 0);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  sensors.begin();
  Serial.println("Dallas Temperature IC Control Library Loaded");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  digitalWrite(2, 0);
  
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  float temperature;
  
  if (req.indexOf("/temperature") != -1)
    temperature = read_temperature();
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  
  client.flush();


  String s = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"temperature\": ";
  s += temperature;
  s += "}";
  
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");
}

float read_temperature() {
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");
  Serial.print("Temperature for the device 1 (index 0) is: ");
  float temperature = sensors.getTempCByIndex(0);
  Serial.println(temperature);
  return temperature;
}

