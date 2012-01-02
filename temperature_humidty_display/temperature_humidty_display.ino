// temperature humidty display unit
// DHT22 + DFRobot LCD Keypad Shield

#include <LCD4Bit_mod.h>
#include <dht.h>

LCD4Bit_mod lcd = LCD4Bit_mod(2);
dht DHT;
char *TEMPERATURE_LABEL = "Temp.: ";
char *HUMIDITY_LABEL = "Luftf.: ";
char *HUMIDITY_SUFFIX = "%";
const int STAT_LED_PIN = 13;
const int DISPLAY_WIDTH = 16;
int displayed_error = 0;

void setup() {
  init_lcd();
  DHT.pin = 12;
  pinMode(STAT_LED_PIN, OUTPUT);
  init_serial();
}

void loop() {
  int result = DHT.readDefinedPin();
  switch (result) {
    case 0:
      output_values(DHT.temperature, DHT.humidity);
      Serial.print("T: ");
      Serial.print(DHT.temperature, 1);
      Serial.write(0xb0);
      Serial.print(" f: ");
      Serial.print(DHT.humidity, 1);
      Serial.println("%");
      break;
    case -1: output_error("sensor checksum error"); break;
    case -2: output_error("sensor timeout error"); break;
    default: output_error("unknown sensor error"); break;
  }
  delay(1000);
}

void init_lcd() {
  lcd.init();
  lcd.clear();
}

void init_serial() {
  Serial.begin(115200);
  Serial.println("temperature humidity printer");
}

void output_values(double temperature, double humidity) {
  if (displayed_error) {
    lcd.clear();
    displayed_error = 0;
    digitalWrite(STAT_LED_PIN, LOW);
  }
  char temp[10];
  dtostrf(temperature, 3, 1, temp);
  char humi[10];
  dtostrf(humidity, 3, 1, humi);
  lcd.cursorTo(0,0);
  lcd.printIn(TEMPERATURE_LABEL);
  lcd.printIn(temp);
  lcd.print(223);
  lcd.cursorTo(2, 0);
  lcd.printIn(HUMIDITY_LABEL);
  lcd.printIn(humi);
  lcd.printIn(HUMIDITY_SUFFIX);
  delay(1000);
  int scroll = strlen(HUMIDITY_LABEL) + strlen(humi) + strlen(HUMIDITY_SUFFIX)
      - DISPLAY_WIDTH;
  lcd.leftScroll(scroll, 500);
}

void output_error(char *output) {
  digitalWrite(STAT_LED_PIN, HIGH);
  displayed_error = 1;
  lcd.clear();
  lcd.printIn(output);
  delay(1000);
  lcd.leftScroll((int) strlen(output) - DISPLAY_WIDTH, 300);
}

