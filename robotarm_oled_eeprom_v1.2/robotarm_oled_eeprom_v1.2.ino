#include <Servo.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>

#define PIN_SHOULDER_X 11
#define PIN_SHOULDER_Y 10
#define PIN_ELBOW      9
#define PIN_CLAW       6

#define PIN_ANALOG_X A0
#define PIN_ANALOG_Y A1
#define PIN_BUTTON_A 2
#define PIN_BUTTON_B 3
#define PIN_BUTTON_C 4
#define PIN_BUTTON_D 5
#define PIN_BUTTON_K 8

#define INDEX_SERVO_ARRAY_SHOULDER_X 3  
#define INDEX_SERVO_ARRAY_SHOULDER_Y 1
#define INDEX_SERVO_ARRAY_ELBOW      2
#define INDEX_SERVO_ARRAY_CLAW       0
#define NumberOfServo  4
Servo s[NumberOfServo];

//degree[0]는 집게. 집게를 서로 일직선이 되게 한 상태에서 각도를 0도로 주고 연결. 초기값은 16도가 좋은
//degree[1]은 어깨. 초기값은 90도가 좋음 그 상태에서 수직으로 연결
//degree[2]는 팔꿈치. 0도가 바닥과 수직이 되게 하고 연결 초기값이 180도가 좋음  그 상태에서 수평으로 연결
//degree[3]는 어깨. 0도가 바닥판의 Y써있는 쪽을 수직으로 바라보게 꼽고, 초기값은 90도가 좋음(정면바라보도록)
int degree[NumberOfServo];
const int initial_degree[NumberOfServo] = {0, 90, 180, 90};
const int Minimum_Degree[NumberOfServo] = { 0, 90, 0, 0 };
const int Maximum_Degree[NumberOfServo] = {28, 180, 180, 180};
const int step = 4;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
    
  s[INDEX_SERVO_ARRAY_CLAW].attach(PIN_CLAW);
  s[INDEX_SERVO_ARRAY_SHOULDER_Y].attach(PIN_SHOULDER_Y);
  s[INDEX_SERVO_ARRAY_ELBOW].attach(PIN_ELBOW);
  s[INDEX_SERVO_ARRAY_SHOULDER_X].attach(PIN_SHOULDER_X);

  nv2motor();
  func_auto_lift();

  pinMode(PIN_BUTTON_A, INPUT);
  digitalWrite(PIN_BUTTON_A, HIGH);

  pinMode(PIN_BUTTON_B, INPUT);
  digitalWrite(PIN_BUTTON_B, HIGH);

  pinMode(PIN_BUTTON_C, INPUT);
  digitalWrite(PIN_BUTTON_C, HIGH);

  pinMode(PIN_BUTTON_D, INPUT);
  digitalWrite(PIN_BUTTON_D, HIGH);

  pinMode(PIN_BUTTON_K, INPUT);
  digitalWrite(PIN_BUTTON_K, HIGH);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  
  delay(2000);  
}

void loop() {
  // put your main code here, to run repeatedly:
  //const int step = 4;
  int dec_sig[4],inc_sig[4];
  int auto_lift;
  dec_sig[INDEX_SERVO_ARRAY_SHOULDER_X] = analogRead(PIN_ANALOG_X);
  dec_sig[INDEX_SERVO_ARRAY_SHOULDER_Y] = analogRead(PIN_ANALOG_Y);
  dec_sig[INDEX_SERVO_ARRAY_ELBOW]=digitalRead(PIN_BUTTON_A);
  inc_sig[INDEX_SERVO_ARRAY_ELBOW]=digitalRead(PIN_BUTTON_C);
  dec_sig[INDEX_SERVO_ARRAY_CLAW]=digitalRead(PIN_BUTTON_D);
  inc_sig[INDEX_SERVO_ARRAY_CLAW]=digitalRead(PIN_BUTTON_B);
  auto_lift = digitalRead(PIN_BUTTON_K);
  
  if( dec_sig[INDEX_SERVO_ARRAY_SHOULDER_X] < 300 && degree[INDEX_SERVO_ARRAY_SHOULDER_X] < Maximum_Degree[INDEX_SERVO_ARRAY_SHOULDER_X])
    degree[INDEX_SERVO_ARRAY_SHOULDER_X]+=step;
  else if( dec_sig[INDEX_SERVO_ARRAY_SHOULDER_X] > 700 && degree[INDEX_SERVO_ARRAY_SHOULDER_X] > Minimum_Degree[INDEX_SERVO_ARRAY_SHOULDER_X])
    degree[INDEX_SERVO_ARRAY_SHOULDER_X]-=step;
  if( dec_sig[INDEX_SERVO_ARRAY_SHOULDER_Y] < 300 && degree[INDEX_SERVO_ARRAY_SHOULDER_Y] > Minimum_Degree[INDEX_SERVO_ARRAY_SHOULDER_Y])
    degree[INDEX_SERVO_ARRAY_SHOULDER_Y]-=step;
  else if( dec_sig[INDEX_SERVO_ARRAY_SHOULDER_Y] > 700 && degree[INDEX_SERVO_ARRAY_SHOULDER_Y] < Maximum_Degree[INDEX_SERVO_ARRAY_SHOULDER_Y])
    degree[INDEX_SERVO_ARRAY_SHOULDER_Y]+=step;
  if( dec_sig[INDEX_SERVO_ARRAY_ELBOW] == LOW && degree[INDEX_SERVO_ARRAY_ELBOW] > Minimum_Degree[INDEX_SERVO_ARRAY_ELBOW])
    degree[INDEX_SERVO_ARRAY_ELBOW]-=step;
  else if( inc_sig[INDEX_SERVO_ARRAY_ELBOW] == LOW && degree[INDEX_SERVO_ARRAY_ELBOW] < Maximum_Degree[INDEX_SERVO_ARRAY_ELBOW])
    degree[INDEX_SERVO_ARRAY_ELBOW]+=step;
  if( dec_sig[INDEX_SERVO_ARRAY_CLAW] == LOW && degree[INDEX_SERVO_ARRAY_CLAW] > Minimum_Degree[INDEX_SERVO_ARRAY_CLAW]   )
    degree[INDEX_SERVO_ARRAY_CLAW]-=2;
  else if( inc_sig[INDEX_SERVO_ARRAY_CLAW] == LOW && degree[INDEX_SERVO_ARRAY_CLAW] < Maximum_Degree[INDEX_SERVO_ARRAY_CLAW])
    degree[INDEX_SERVO_ARRAY_CLAW]+=2;
    
  for(int i = 0; i< NumberOfServo ; i++) {
    if( degree[i] < 0)
      degree[i] = 0;
    if( degree[i] > 180)
      degree[i] = 180;
  }   
   
  if( auto_lift == LOW )
    func_auto_lift();  

  for(int i = 0; i< NumberOfServo ; i++) {
    s[i].write(degree[i]);
    EEPROM.update(i, degree[i]);   
    delay(50);
  }   

  display.clearDisplay();

  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(WHITE);

  display.print("SHOU_X:");
  display.println(degree[INDEX_SERVO_ARRAY_SHOULDER_X]);
  display.print("SHOU_Y:");
  display.println(degree[INDEX_SERVO_ARRAY_SHOULDER_Y]);  
  display.print("ELBOW:");
  display.println(degree[INDEX_SERVO_ARRAY_ELBOW]);  
  display.print("CLAW:");
  display.println(degree[INDEX_SERVO_ARRAY_CLAW]);  
  byte nv_degree[NumberOfServo];
  for(int i = 0; i< NumberOfServo ; i++) {
    nv_degree[i] = EEPROM.read(i);
#ifdef DISPLAY_NV 
    display.println( (String)nv_degree[i]+"___" );
#endif  
  }
  display.display();
  Serial.print("SHOU_X: ");
  Serial.print(degree[INDEX_SERVO_ARRAY_SHOULDER_X]);
  Serial.print(" SHOU_Y: ");
  Serial.print(degree[INDEX_SERVO_ARRAY_SHOULDER_Y]);  
  Serial.print(" ELBOW: ");
  Serial.print(degree[INDEX_SERVO_ARRAY_ELBOW]);  
  Serial.print(" CLAW: ");
  Serial.print(degree[INDEX_SERVO_ARRAY_CLAW]);  
  Serial.println();

  Serial.print("NVOU_X: ");
  Serial.print(nv_degree[INDEX_SERVO_ARRAY_SHOULDER_X]);
  Serial.print(" NVOU_Y: ");
  Serial.print(nv_degree[INDEX_SERVO_ARRAY_SHOULDER_Y]);  
  Serial.print(" NVBOW: ");
  Serial.print(nv_degree[INDEX_SERVO_ARRAY_ELBOW]);  
  Serial.print(" NVAW: ");
  Serial.print(nv_degree[INDEX_SERVO_ARRAY_CLAW]);  
  Serial.println();
}

void nv2motor() {
  byte nv_degree[NumberOfServo];
  for(int i = 0; i<NumberOfServo; i++) {
    nv_degree[i] = EEPROM.read(i);
    if( nv_degree[i] > 180 || nv_degree[i] < 0 ) 
      EEPROM.update(i, initial_degree[i]);
    degree[i] = nv_degree[i];   
    s[i].write(degree[i]);
    delay(50);  
  }  
}

void func_auto_lift() {
      while(1) {
        if( degree[INDEX_SERVO_ARRAY_SHOULDER_Y] > Minimum_Degree[INDEX_SERVO_ARRAY_SHOULDER_Y] ) {
          degree[INDEX_SERVO_ARRAY_SHOULDER_Y]-=step;
          s[INDEX_SERVO_ARRAY_SHOULDER_Y].write(degree[INDEX_SERVO_ARRAY_SHOULDER_Y]);
          delay(50);
        }
        if ( degree[INDEX_SERVO_ARRAY_ELBOW] < Maximum_Degree[INDEX_SERVO_ARRAY_ELBOW] ) {
          degree[INDEX_SERVO_ARRAY_ELBOW]+=step;
          s[INDEX_SERVO_ARRAY_ELBOW].write( degree[INDEX_SERVO_ARRAY_ELBOW] );
          delay(50);
        }
        if( degree[INDEX_SERVO_ARRAY_SHOULDER_Y] <= Minimum_Degree[INDEX_SERVO_ARRAY_SHOULDER_Y]  && 
                 degree[INDEX_SERVO_ARRAY_ELBOW] >= Maximum_Degree[INDEX_SERVO_ARRAY_ELBOW] )   {
             degree[INDEX_SERVO_ARRAY_SHOULDER_Y] = Minimum_Degree[INDEX_SERVO_ARRAY_SHOULDER_Y];     
             degree[INDEX_SERVO_ARRAY_ELBOW] = Maximum_Degree[INDEX_SERVO_ARRAY_ELBOW];
             EEPROM.update(INDEX_SERVO_ARRAY_SHOULDER_Y, degree[INDEX_SERVO_ARRAY_SHOULDER_Y]);   
             EEPROM.update(INDEX_SERVO_ARRAY_ELBOW, degree[INDEX_SERVO_ARRAY_ELBOW]);   
             Serial.println("auto lift");
             break;
        }
      }
}
