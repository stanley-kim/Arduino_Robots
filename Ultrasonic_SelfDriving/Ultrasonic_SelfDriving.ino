#include <Servo.h>
//servo motor
Servo myServo;
const int S_pin=10;
const int Init_degree = 90;
const int neck_t = 100;
//dc motor 
const int AIA=3;
const int AIB=5;
const int BIA=6;  //left
const int BIB=11; //left
const int Max_speed1 = 160;
const int Max_speed2 = 200;
const int delay_t = 350;
const int bal=27;
//ultrasonic sensor
const int trigpin=13;
const int echopin=12;
//distance from sensor
unsigned int F_Dist, L_Dist, R_Dist, L45_Dist, R45_Dist; 
const int Dist_Min = 20;
const int Half_Width_Min = 10;

int alert_count = 0;
const int alert_count_threshold = 2;
int brake_look_around = 0;
const int brake_look_around_period = 15;

void setup() {
  // put your setup code here, to run once:
  myServo.attach(S_pin);
  myServo.write(Init_degree);
  delay(neck_t);
  
  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);

  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  digitalWrite(trigpin, LOW);

  Serial.begin(9600);
}

void loop() {
  look_forward();
  if( F_Dist <= Dist_Min) 
    alert_count++;
  else
    alert_count = 0;  

  if( alert_count > alert_count_threshold) {
    Serial.print("alert_count");
    Serial.println(alert_count_threshold);
    stop();
    look_around();
    switch( selectDirection() ) {
      case 'l' :
        left(delay_t);
        break;
      case 'r' :
        right(delay_t);
        break;
      case 'b' :
      default : 
        backward(2*delay_t);
        break;
    }
    alert_count = 0;
  }
  else {
    brake_look_around++;  
    if( brake_look_around > brake_look_around_period ) {
      Serial.print("brake_look_around>");
      Serial.println(brake_look_around_period);
      stop();
      look_around();
      switch( selectDirection() ) {
        case 'f' :
          forward();
          break;
        case 'l' :
          left(delay_t);
          break;
        case 'r' :
          right(delay_t);
          break;
        case 'b' :
        default :
          backward(2*delay_t);
          break;
      }      
      brake_look_around = 0;
    }
    else 
      forward();
  }
} //end of loop
void forward() {
  analogWrite(AIA, Max_speed1-bal);
  analogWrite(AIB, 0);
  analogWrite(BIA, Max_speed1);
  analogWrite(BIB, 0);  
}
void backward(int t) {
  analogWrite(AIA, 0);
  analogWrite(AIB, Max_speed1);
  analogWrite(BIA, 0);
  analogWrite(BIB, Max_speed1);
  delay(t);    
}
void right(int t) {
  analogWrite(AIA, 0);
  analogWrite(AIB, Max_speed2);
  analogWrite(BIA, Max_speed2);
  analogWrite(BIB, 0);
  delay(t);
}
void left(int t) {
  analogWrite(AIA, Max_speed2);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, Max_speed2);
  delay(t);
}
void stop() {
  analogWrite(AIA, 0);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, 0);
}
unsigned int U_sonic() {
  unsigned long Dist_cm;
  digitalWrite(trigpin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigpin, LOW);
  Dist_cm= pulseIn(echopin, HIGH);  //recognize 45degree obstacle as 22m distance;;
//  Dist_cm= pulseIn(echopin, HIGH, 100000UL);    //1700cm=17m timeout, now 45degree obstacle as 0m, but not always ㅠ   
  Serial.print(Dist_cm);   
  Dist_cm= Dist_cm * 0.017;
  Serial.print(" (cm: ");
  Serial.println(Dist_cm); 
  return round(Dist_cm);
}
void look_forward() {
  myServo.write(90);
  delay(neck_t);
  F_Dist = U_sonic();
  Serial.print(">>F_Dist:"); Serial.println(F_Dist);  
}
void look_around() {
  myServo.write(5);
  delay(neck_t);
  R_Dist = U_sonic();
  Serial.print("<R_Dist:"); Serial.println( R_Dist);
  myServo.write(45);
  delay(neck_t);
  R45_Dist = U_sonic();
  Serial.print("<R45_Dist:"); Serial.println( R45_Dist);
  myServo.write(90);
  delay(neck_t);
  F_Dist = U_sonic();
  Serial.print("<F_Dist:"); Serial.println(F_Dist);
  myServo.write(135);
  delay(neck_t);
  L45_Dist = U_sonic();
  Serial.print("<L45_Dist:"); Serial.println( L45_Dist);
  myServo.write(175);
  delay(neck_t);
  L_Dist = U_sonic();
  Serial.print("<L_Dist:"); Serial.println( L_Dist);
}  //end of look_around
char selectDirection() {
    if ( F_Dist >= Dist_Min && R_Dist >= Half_Width_Min && R45_Dist >= Half_Width_Min && L45_Dist >= Half_Width_Min && L_Dist >= Half_Width_Min )  {
      Serial.println("select forward");      
      return 'f';
    }
    else if(  L_Dist >= Dist_Min && L45_Dist >= Dist_Min )  {
      Serial.println("select L_turn");      
      return 'l';     
    }
    else if(   R45_Dist >= Dist_Min && R_Dist >= Dist_Min )  {
      Serial.println("select R_turn");      
      return 'r';     
    }
    else   {
      Serial.println("select backward");      
      return 'b';     
    }
}
