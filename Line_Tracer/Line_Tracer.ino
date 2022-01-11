//line tracer codes v6.0 higher speed for curve
int AIA = 5;
int AIB = 6;
int BIA = 9;
int BIB = 10;
int Left_Sensor = 3;
int Right_Sensor = 4;


void setup() {
  // put your setup code here, to run once:
  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);
  pinMode(Right_Sensor, INPUT);
  pinMode(Left_Sensor, INPUT);  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int R, L;
  R= digitalRead(Right_Sensor);
  L= digitalRead(Left_Sensor);
  Serial.print(L);
  Serial.println(R);
  
  if(R ==0 && L==0)  {
    Serial.println("Forward");
    forward(140);
  }
  else if(R ==1 && L==0)   {
    Serial.println("Right");
    R_turn(140);
  }
  else if(R ==0 && L==1)  {
    Serial.println("Right");
    L_turn(140);
   }
  else if(R ==1 && L==1)  {
    Serial.println("Brake");
    brake();
    ///delay(4000);
  }      
}


void forward(int X) {
  analogWrite(AIA, X);
  analogWrite(AIB, 0);
  analogWrite(BIA, X);
  analogWrite(BIB, 0);
  delay(5);
}

void R_turn(int Y) {
  analogWrite(AIA, 0);
  analogWrite(AIB, Y);
  analogWrite(BIA, Y);
  analogWrite(BIB, 0);
  delay(5);
}

void L_turn(int Z) {
  analogWrite(AIA, Z);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, Z);
  delay(5);
}

void brake() {
  analogWrite(AIA, 0);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, 0);
  delay(5);
}
