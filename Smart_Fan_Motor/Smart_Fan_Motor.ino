#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
   // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();

  lcd.init();
  lcd.backlight();
}

void loop() {
  // put your main code here, to run repeatedly:
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

if(isnan(humidity) || isnan(temperature) )  {
  Serial.println("Failed to read from HDT");
  return ;
}
Serial.print( (int)temperature);
Serial.print( "*c, ");
Serial.print( (int)humidity);
Serial.println("%");

String humi = "Humi: ";
humi += (String)humidity;
humi += "%";

String temp = "temp: ";
temp += (String)temperature;
temp+="C";

lcd.setCursor(0,0);
lcd.print(humi);

lcd.setCursor(0,1);
lcd.print(temp);

delay(1500);
}
