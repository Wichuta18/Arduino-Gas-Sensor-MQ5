#include <ESP8266WiFi.h> 
#include <stdio.h>
#define Red D5
const char* ssid     = "Silvia";     // Set SSID
const char* password = "qwertyui"; // Set password
const char* thing = "GasZyGas";   // Set Thing name
const char* host = "dweet.io";        // Set host 
int count = 0;
const int buzzer = D8;

void setup() {

    Serial.begin(9600);
    pinMode(buzzer, OUTPUT);  
    pinMode(Red, OUTPUT);  
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");                // Print WiFi status
    }
    Serial.println("");
    Serial.println("WiFi connected");   // Print connect status
    Serial.println("IP address: ");     
    Serial.println(WiFi.localIP());     // Print IP address
}
   

void loop() {

   //GAS
    float sensor_volt;
    float RS_air; //  Get the value of RS via in a clear air
    float R0;  // Get the value of R0 via in H2
    float sensorValue;

        /*--- Get a average data by testing 100 times ---*/
    for(int x = 0 ; x < 100 ; x++)
    {
        sensorValue = sensorValue + analogRead(A0);
    }
    sensorValue = sensorValue/100.0;
        /*-----------------------------------------------*/

    sensor_volt = sensorValue/1024*5.0;
    RS_air = (5.0-sensor_volt)/sensor_volt; // omit *RL
    R0 = RS_air/6.5; // The ratio of RS/R0 is 6.5 in a clear air from Graph (Found using WebPlotDigitizer)

    Serial.print("sensor_volt = ");
    Serial.print(sensor_volt);
    Serial.println("V");

    Serial.print("R0 = ");
    Serial.println(R0);
    delay(1000);

    if(R0 <= 0.23){
      digitalWrite(Red,1);
      tone(buzzer, 1000); // Send 1KHz sound signal...
      delay(1000);        // ...for 1 sec
      noTone(buzzer);     // Stop sound...
      delay(1000); 
    }else{
      digitalWrite(Red,0);
      delay(1000); 
    }
  
    delay(1000);
    ++count;                            // Count value
    WiFiClient client;
    const int httpPort = 80;            // Set HTTP port
    if (!client.connect(host, httpPort))// Test connection 
    {
      Serial.println("connection failed"); // Print connection fail message
      return;
    }
    String url =  String("/dweet/for/") + thing + "?"; // Set message
    url += "count=";
    url += count;
    url += "&value=";
    url += R0;
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: keep-alive\r\n" +
                 "Cache-Control: max-age=0\r\n\r\n");
    Serial.println("URL : ");         
    Serial.println(url);              // Print URL 
    delay(1000);
    
    while (client.available()) 
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
    Serial.println();
    Serial.println("closing connection"); // Print closing status

 
}



