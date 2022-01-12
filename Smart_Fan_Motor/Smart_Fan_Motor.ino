//thermo meter + lcd display + conditional fan
#include "DHT.h"
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial BT_Serial(7,8);

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//LiquidCrystal_I2C lcd(0x3F, 16, 2);
LiquidCrystal_I2C lcd(0x27, 16, 2);

int BIA = 9;
int BIB = 10;
int motor_speed = 150;

float threshold_temperature = 24.0;

void setup() {
   // put your setup code here, to run once:
  Serial.begin(9600);
  BT_Serial.begin(9600);
  dht.begin();

  lcd.init();
  lcd.backlight();

  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  String value;
  while( BT_Serial.available() ) {
    delay(10);
    char c=BT_Serial.read();
    value = value + c;
  }
  if(value.length() > 0 ) {
      if(value == "forward") threshold_temperature = threshold_temperature + 1.0;
      else if(value == "reverse") threshold_temperature = threshold_temperature - 1.0;
      else if(value == "right")   motor_speed += 10;
      else if(value == "left")    motor_speed -= 10;
      value ="";
  }
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
  humi += "% ";
  humi += (String)motor_speed;
  humi += "   ";

  String temp = "temp: ";
  temp += (String)temperature;
  temp+="C";
  if(temperature > threshold_temperature) 
    temp += ">";
  else if (temperature < threshold_temperature)
    temp += "<";
  else
    temp += "=";
  temp += (String)((int)threshold_temperature);  

  lcd.setCursor(0,0);
  lcd.print(humi);

  lcd.setCursor(0,1);
  lcd.print(temp);

  if( temperature > threshold_temperature )  {
    analogWrite(BIA, motor_speed);
    analogWrite(BIB, 0);
  }
  else  {
    analogWrite(BIA, 0);
    analogWrite(BIB, 0);
  }

  delay(2000);
}
