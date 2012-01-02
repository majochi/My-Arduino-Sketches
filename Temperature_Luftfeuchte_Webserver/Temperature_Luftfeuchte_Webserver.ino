#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include <WebServer.h>

dht DHT;

byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0xE9, 0x8C };

WebServer webserver("/", 80);
P(header) =
  "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
  "<!DOCTYPE html PUBLIC \"-//OPENWAVE//DTD XHTML Mobile 1.0//EN\"\n"
  "\"http://www.openwave.com/dtd/xhtml-mobile10.dtd\">\n"
  "<html><head><title>Arduino UNO DHT22 Wetterserver</title>\n"
  "<style type=\"text/css\">\n"
  "body {\n"
  "  background: #bbb;\n"
  "  font-family: sans-serif;\n"
  "}\n"
  "</style>"
  "</head>\n"
  "<body><h1>Wetterduino</h1><p>\n";
P(footer) =
  "</p></body></html>";


long lastReadingTime = 0;

void temperatureHumidityCmd(WebServer &server, WebServer::ConnectionType type,
                            char *, bool) {
  server.httpSuccess();
  if (type == WebServer::HEAD) return;
  
  server.printP(header);
  server.print("Temperatur: ");
  server.print(DHT.temperature, 1);
  server.print("°</br>");
  server.print(" Luftfeuchte: ");
  server.print(DHT.humidity, 1);
  server.println("%");
  server.printP(footer); 
}

void setup() {
  Serial.begin(115200);
  Serial.println("starting...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed");
    while (true);
  }
  
  Serial.print("my IP: ");
  Serial.println(Ethernet.localIP());

  DHT.pin = 2;

  webserver.setDefaultCommand(&temperatureHumidityCmd);
  webserver.begin();

  delay(250);
}

void loop() {
  char buff[64];
  int len = 64;
  
  if (millis() - lastReadingTime > 1000)  {
    int result = DHT.readDefinedPin();
    switch (result) {
      case 0:
        Serial.print("T: ");
        Serial.print(DHT.temperature, 1);
        Serial.write(0xb0);
        Serial.print(" f: ");
        Serial.print(DHT.humidity, 1);
        Serial.println("%");
        break;
      case -1: Serial.println("sensor checksum error"); break;
      case -2: Serial.println("sensor timeout error"); break;
      default: Serial.println("unknown sensor error"); break;

    }
    lastReadingTime = millis();
  }
  webserver.processConnection(buff, &len);
}

