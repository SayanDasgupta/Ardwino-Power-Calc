#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

//variables
unsigned int sum, prevTime, count;
const float impulseRate = 562.5;
bool flag;

void setup() {
  // functional Setup
  lcd.begin(16,2);
  pinMode(A0,INPUT);
  pinMode(0,INPUT_PULLUP);
  pinMode(7, OUTPUT);
  
  //innitialize
  sum = 0;
  prevTime = millis();
  count = 0;
  flag = false;
}

void loop() {
  // Monitor LDR
  lcd.setCursor(0,0);
  lcd.print("Pulse : ");
  lcd.setCursor(0,1);
  lcd.print("Power : "+String(getWattConsumed()));
  
  // conditionals (Events)
  if(analogRead(A0)>500 && flag==false){
    digitalWrite(7, HIGH);
    lcd.setCursor(10, 0);
    pulse();
    lcd.print(count);
    flag=true;
  } else if(analogRead(A0)<500 && flag==true){
    digitalWrite(7, LOW);
    flag=false;
  }
  
}

// Custom Functions 

void pulse(){
  unsigned int currTime = millis();
  count++;
  sum+=(currTime-prevTime);
  prevTime = currTime;
}

float getAvgBlinkTime(){
  return (count==0)?0.0:((sum*1.0)/count);
}

float getWattConsumed(){
  float avgBlink = getAvgBlinkTime();
  return (avgBlink==0.0)?0.0:(impulseRate/avgBlink);
}
