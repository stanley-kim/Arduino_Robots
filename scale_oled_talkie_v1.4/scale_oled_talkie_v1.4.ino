/*
 Example using the SparkFun HX711 breakout board with a scale
 By: Nathan Seidle
 SparkFun Electronics
 Date: November 19th, 2014
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

 This example demonstrates basic scale output. See the calibration sketch to get the calibration_factor for your
 specific load cell setup.

 This example code uses bogde's excellent library:"https://github.com/bogde/HX711"
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE

 The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
 based load cell which should allow a user to measure everything from a few grams to tens of tons.
 Arduino pin 2 -> HX711 CLK
 3 -> DAT
 5V -> VCC
 GND -> GND

 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power should be fine.

*/
#include "Talkie.h"
#include "Vocab_US_Large.h"
#include "Vocab_Special.h"

#include "HX711.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#define calibration_factor -7050.0 //This value is obtained using the SparkFun_HX711_Calibration sketch
//#define calibration_factor -6950.0 //This value is obtained trial and error
#define calibration_factor -457.0 //This value is obtained trial and error


#define DOUT  7
#define CLK  6

HX711 scale;



#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#define TALK 3
Talkie voice;
const uint8_t spHEAVY[] PROGMEM = {0x08,0xC8,0x46,0x13,0x03,0xAB,0xAE,0x5E,0xCC,0xB3,0x6B,0xAF,0xB6,0x99,0x10,0xAB,0xBC,0xB3,0xEA,0xE6,0x52,0x34,0xE2,0xF6,0xAA,0x8A,0x73,0x55,0xD7,0x35,0xA6,0xF1,0xDA,0x55,0xC3,0x56,0x85,0x76,0xC8,0x10,0x63,0xB3,0x33,0xB2,0x16,0x54,0xC4,0x6D,0xF1,0xCA,0x7A,0x50,0x96,0xC8,0x25,0xA3,0xE8,0x41,0xD1,0x32,0x1F,0xCF,0xA2,0x56,0xFF,0x0F};
const uint8_t spLIGHT[] PROGMEM = {0x69,0x18,0x44,0xD2,0x83,0xB2,0x95,0x69,0x63,0x1A,0x17,0x49,0xD7,0xA6,0x85,0x78,0x5D,0xD5,0xFB,0x1A,0x33,0xDF,0x76,0x97,0x2D,0x6B,0x68,0x6C,0x4A,0x53,0xF7,0xAC,0xA9,0x89,0x71,0x2D,0xDD,0x33,0xA6,0x26,0x9B,0xAD,0x6D,0x77,0x5B,0x7A,0x70,0xF4,0xC8,0x45,0xE1,0x69,0x8E,0xA9,0x6B,0x6E,0x89,0x62,0x78,0x45,0x8D,0x7C,0x8C,0xC0,0x29,0x4A,0x55,0xA8,0x06,0x06,0x01,0x0C,0x30,0xA4,0x9B,0x02,0x36,0x57,0x43,0xE0,0x14,0xA5,0x2A,0x54,0xE3,0xFF,0x01};


void setup() {
  digitalWrite(TALK, HIGH);  //no sound
  Serial.begin(9600);
  Serial.println("HX711 scale demo");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch

  for( int i=0; i<2 ; i++)  {
    scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
    Serial.print(i);
    Serial.print("th Reading: ");
    Serial.print((int)scale.get_units(), 1); //scale.get_units() returns a float
    Serial.println(" g");
  }
}
int prev_weight = 0;
const int diff = 10;
void loop() {
   int weight = (int)scale.get_units();  //gram 
  Serial.print("Reading: ");
  Serial.print(weight, 1); //scale.get_units() returns a float
 
  Serial.print(" g");
  //Serial.print(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();

  display.clearDisplay();

  //display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setCursor(0,0);             // Start at top-left corner

  display.setTextSize(4);             // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.print( weight );

  display.println("g"); 


  display.setTextSize(2);   
  if( weight >= 98 && weight < 149 && (  (weight - prev_weight) > diff || (prev_weight - weight) > diff  )  )  {
    voice.say(spLIGHT);
    display.println( "Light" );
  }
  else if( weight >= 149 && weight < 200 && (  (weight - prev_weight) > diff || (prev_weight - weight) > diff  )  )  {
    voice.say(spHEAVY);
    display.println( "Heavy" );
  }
  else if( weight >= 1000 && (  (weight - prev_weight) > diff || (prev_weight - weight) > diff  )  )  {
    voice.say(sp2_TWO);
    voice.say(spHEAVY);
    display.println( "Too Heavy" );
  }
  else if( weight >= 2000 && (  (weight - prev_weight) > diff || (prev_weight - weight) > diff  )  )  {
    voice.say(sp2_TWO);
    voice.say(sp2_TWO);
    voice.say(spHEAVY);
    display.println( "TooToo Heavy" );
  }
  digitalWrite(TALK, HIGH);
  display.display();  
  
  prev_weight = weight;
  delay(1000);
  
}
