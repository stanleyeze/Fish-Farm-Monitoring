#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
SoftwareSerial s(D6,D5);
#include <ArduinoJson.h>

#define LED_BUILTIN 3

String apiWritekey = "ZJI1BA5Z4135QHXW"; // replace with your THINGSPEAK WRITEAPI key here
const char* ssid ="SASKTEL344"; //"SASKTEL531";//"uofrGuest"; // your wifi SSID name
const char* password = "3359mEQ535zv";//"6uKW7XPu5vZW" ;// wifi pasword 6uKW7XPu5vZW

String apiKey= "62d0f28aaf7c8189a8d23a5cae9b8799"; 
//the city you want the weather for 
String location= "saskatoon,CA"; 
int status = WL_IDLE_STATUS; 
char server2[] = "api.openweathermap.org"; 
 
const char* server = "api.thingspeak.com";
float resolution=3.3/1023;// 3.3 is the supply volt  & 1023 is max analog read value
WiFiClient client;


 
void setup() {

  Serial.begin(115200);
  WiFi.disconnect();
  delay(10);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
 
  Serial.println("");
  Serial.print("NodeMcu connected to wifi...");
  Serial.println(ssid);
  Serial.println();
  
  // Initialize Serial port
  Serial.begin(9600);
  s.begin(9600);
  while (!Serial) continue;
 
}
void getWeather() { 
 Serial.println("\nStarting connection to server..."); 
 // if you get a connection, report back via serial: 
 if (client.connect(server2, 80)) { 
   Serial.println("connected to server"); 
   Serial.println(""); 
   Serial.println("Downloading forcast data..."); 
   // Make a HTTP request: 
   client.print("GET /data/2.5/forecast?"); 
   client.print("q="+location); 
   client.print("&appid="+apiKey); 
   client.print("&cnt=3"); 
   client.println("&units=metric"); 
   client.println("Host: api.openweathermap.org"); 
  // client.println("Connection: close"); 
   client.println(); 
 } else { 
   Serial.println("unable to connect"); 
 } 
 delay(1000); 
 String line = ""; 
   line = client.readStringUntil('\n'); 
   //Serial.println(line); 
   Serial.println("parsingValues"); 
   //create a json buffer where to store the json data 
   StaticJsonBuffer<5000> jsonBuffer; 
   JsonObject& root = jsonBuffer.parseObject(line); 
//     Serial.print(root);
   if (!root.success()) { 
     Serial.println("parseObject() failed"); 
     return; 
   } 
 //get the data from the json tree 
 String nextWeatherTime0 = root["list"][0]["dt_txt"]; 
 String nextWeather0 = root["list"][0]["weather"][0]["main"]; 
 String nextWeatherTime1 = root["list"][1]["dt_txt"]; 
 String nextWeather1 = root["list"][1]["weather"][0]["main"]; 
 String nextWeatherTime2 = root["list"][2]["dt_txt"]; 
 String nextWeather2 = root["list"][2]["weather"][0]["main"]; 

 
 // Print values. 
Serial.println("");
Serial.println("**** MORNING WEATHER FORCAST FOR DATE: "+nextWeatherTime0+" ****");
 Serial.println("WEATHER: "+nextWeather0); 
 Serial.println(""); 

 Serial.println("**** AFTERNOON WEATHER FORCAST FOR DATE: "+nextWeatherTime1+" ****");
 Serial.println("WEATHER: "+nextWeather1); 
 Serial.println(""); 

 Serial.println("**** EVENING WEATHER FORCAST FOR DATE: "+nextWeatherTime2+" ****");
 Serial.println("WEATHER: "+nextWeather2); 
 Serial.println(""); 
}

void loop() {
 StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
  if (root == JsonObject::invalid())
    return;
  Serial.println("JSON received and parsed");
  //root.prettyPrintTo(Serial);
  Serial.print("Temperature Value ");
  float data1=root["data1"];
  Serial.println(data1);
  Serial.print("   Humidity Value ");
  float data2=root["data2"];
  Serial.println(data2);

   Serial.print("   pH Value ");
  float data3=root["data3"];
  Serial.println(data3);

  Serial.print(" Error Message:  ");
  String data4=root["data4"];
  Serial.println(data4);
  Serial.println("---------------------xxxxx--------------------");

  if (client.connect(server,80))
  {  
    String tsData = apiWritekey;
           tsData +="&field1=";
           tsData += String(data1);
           tsData +="&field2=";
           tsData += String(data2);
           tsData +="&field3=";
           tsData += String(data3);
           tsData += "\r\n\r\n";
 
     client.print("POST /update HTTP/1.1\n");
     client.print("Host: api.thingspeak.com\n");
     client.print("Connection: close\n");
     client.print("X-THINGSPEAKAPIKEY: "+apiWritekey+"\n");
     client.print("Content-Type: application/x-www-form-urlencoded\n");
     client.print("Content-Length: ");
     client.print(tsData.length());
     client.print("\n\n");  // the 2 carriage returns indicate closing of Header fields & starting of data
     client.print(tsData);
     delay(9600);

     Serial.println("uploaded to Thingspeak server....");
  }
 getWeather();
  client.stop();
}
