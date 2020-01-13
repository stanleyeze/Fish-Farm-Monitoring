// include the libraries
#include "DHT.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SensorPin 0          // the pH meter Analog output is connected with the Arduino’s Analog
#define DHTPIN 8
#define DHTTYPE DHT11 
#define LED_BUILTIN 3
const int buzzer = 9;
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

DHT dht (DHTPIN,DHTTYPE);
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
String err= "NAN";

SoftwareSerial s(5,6);

void setup() {
  /*
lcd.init();// initialize the lcd 
lcd.backlight();
lcd.setCursor(0,0);
lcd.print(t);
lcd.setCursor(1,1);
lcd.print(h);
*/
//get lcd to display on the screen
lcd.begin();
lcd.backlight();
//print on boot lcd
lcd.print("Hello World Stanley");

//select port 9600
s.begin(9600);
dht.begin();
Serial.begin(9600);
pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
digitalWrite(LED_BUILTIN, HIGH);
}
 
void loop() {
float ph = phvalue();// reads the ph value
float t = dht.readTemperature();// creats dht11 object
float h = dht.readHumidity();
Serial.print("The tem is : ");
Serial.println(t);
Serial.print("The Humidity is : ");
Serial.println(h);
Serial.println("The pH level is : ");
Serial.println(ph);

err="NAN";
/*if(ph > 9 || ph < 5 || t > 30 || t < 15 || h < 45){
   //digitalWrite(LED_BUILTIN, HIGH); 
   tone(buzzer, 1000);
   err="HHHHHH";
   delay(9600);
  }else{
    //digitalWrite(LED_BUILTIN, LOW);
    noTone(buzzer); 
    }
    */
  
//conditions to notify user
if(ph > 9){
  //digitalWrite(LED_BUILTIN, HIGH); 
   tone(buzzer, 1000);
   lcd.begin();
   lcd.backlight();
   lcd.setCursor(0,2);
   lcd.print("The pH value is high");
   delay(9600);
  } else if(ph < 5){
    //digitalWrite(LED_BUILTIN, HIGH); 
   tone(buzzer, 1000);
      lcd.begin();
   lcd.backlight();
   lcd.setCursor(0,2);
   lcd.print("The pH value is low");
   delay(9600);
    }else if(t > 30){
      //digitalWrite(LED_BUILTIN, HIGH); 
   tone(buzzer, 10000);
      lcd.begin();
   lcd.backlight();
   lcd.setCursor(0,2);
   lcd.print("The Temp is high");
   delay(9600);
      } else if(h < 45){
      //digitalWrite(LED_BUILTIN, HIGH); 
   tone(buzzer, 10000);
      lcd.begin();
   lcd.backlight();
   lcd.setCursor(0,2);
   lcd.print("The Hum is low");
   delay(9600);
      }   
    else{
      /////
      }
  
  
//create json buffer for transfering data to nodemcu
StaticJsonBuffer<50> jsonBuffer;
 JsonObject& root = jsonBuffer.createObject();
  root["data1"] = t;// reads temperature value
  root["data2"] = h;  //reads humidity value  
  root["data3"] = ph;// reads ph value
  root["data4"] = err;// reads ph value

 //also display sensor info on screen
lcd.begin();
lcd.backlight();
lcd.setCursor(0,0);
lcd.print("Temp Value is =");
lcd.setCursor(15,0);
lcd.print(t);
lcd.setCursor(17,0);
lcd.print("*C ");

lcd.setCursor(0,1);
lcd.print("Hum Value is=");
lcd.setCursor(13,1);
lcd.print(h);
lcd.setCursor(15,1);
lcd.print("%  ");

lcd.setCursor(0,2);
lcd.print("pH Value is=");
lcd.setCursor(12,2);
lcd.print(ph);
lcd.setCursor(16,2);
lcd.print("ph ");

if(s.available()>0)
{
  root.printTo(s);
  
  }
  delay(9600);
}

float phvalue(){
    for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue; 
  return phValue;
  }
