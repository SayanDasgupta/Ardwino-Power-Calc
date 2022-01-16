#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
SoftwareSerial sim800l(0, 1);

/*
 * For Impulse Rate : 
 * 6400 -> 562.5
 * 3200 -> 1125
 * 1600 -> 2250
 */

//variables
unsigned int sum, prevTime, count;
const float impulseRate = 562.5;
bool flag;

void setup() {
  // functional Setup
  lcd.begin(16,2);
  pinMode(A0,INPUT);
  pinMode(6,INPUT_PULLUP);
  pinMode(7, OUTPUT);

  sim800l.begin(9600);   //Module baude rate, this is on max, it depends on the version
  Serial.begin(9600);   
  delay(1000);
  
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

  //EVENT : Button Press
  if (digitalRead(6) == LOW) {            //And if it's pressed
    Serial.println("Button pressed");   //Shows this message on the serial monitor
    delay(200);                         //Small delay to avoid detecting the button press many times
    SendSMS(getWattConsumed());                          //And this function is called
  }
  if (sim800l.available()){            //Displays on the serial monitor if there's a communication from the module
    Serial.write(sim800l.read()); 
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

void SendSMS(float val){
  Serial.println("Sending SMS...");               //Show this message on serial monitor
  sim800l.print("AT+CMGF=1\r");                   //Set the module to SMS mode
  delay(100);
  sim800l.print("AT+CMGS=\"+919080659745\"\r");  //Your phone number don't forget to include your country code, example +212123456789"
  delay(500);
  sim800l.print("Power Consumed : "+String(val)); //This is the text to send to the phone number, don't make it too long or you have to modify the SoftwareSerial buffer
  Serial.println("Power Consumed : "+String(val));
  delay(500);
  sim800l.print((char)26);// (required according to the datasheet)
  delay(500);
  sim800l.println();
  Serial.println("Text Sent.");
  delay(500);
}
