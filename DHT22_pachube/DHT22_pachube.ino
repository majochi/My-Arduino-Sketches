#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>
#include <private.h> // defines PACHUBE_API_KEY

dht DHT;

byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0xE9, 0x8C };

long lastReadingTime = 0;

EthernetClient client;

long interval = 10000;

boolean lastConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("starting...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("DHCP failed, halting...");
    while (true);
  }
  
  Serial.print("my IP: ");
  Serial.println(Ethernet.localIP());

  DHT.pin = 2;

  delay(250);
}

void loop() {
  
  if (millis() - lastReadingTime > interval && !client.connected())  {
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
      case -1: Serial.println("sensor checksum error"); return;
      case -2: Serial.println("sensor timeout error"); return;
      default: Serial.println("unknown sensor error"); return;

    }

    char tempStr[8];
    char humiStr[8];
    dtostrf(DHT.temperature, 3, 1, tempStr);
    dtostrf(DHT.humidity, 3, 1, humiStr);
    String data = String(String("1,") + String(tempStr) + String("\n2,")
        + String(humiStr)); 
    Serial.println(data);
    
    if (client.connect("api.pachube.com", 80)) {
      Serial.println("connected");
      client.println("PUT /v2/feeds/43055.csv HTTP/1.1");
      client.print("X-PachubeApiKey: ");
      client.println(PACHUBE_API_KEY);
      client.print("Content-Length: ");
      client.println(data.length());
      client.println("Host: api.pachube.com");
      client.println("Connection: close");
      client.println();
      client.print(data);
      Serial.println("data sent:");
      Serial.println(data);
    } else {
      Serial.println("error connecting to server");
    }

    lastReadingTime = millis();
  }

  if (client.available()) {
      char c;
      c = client.read();
      Serial.print(c);
  }
  
  if (!client.connected() && lastConnected) {
    client.stop();
    Serial.println("\ndisconnected");
  }
  
  lastConnected = client.connected();
}

