  


/*
This code is to get multiple temp and humidity  data using Esp32 and NCD long-range-iot-wireless-temperature-humidity-sensor.
Also to create  AWS email alert on multiple data of sensor.

Sensor data structure can be found here https://ncd.io/long-range-iot-wireless-temperature-humidity-sensor-product-manual/

Sensor can be found here https://store.ncd.io/product/industrial-long-range-wireless-temperature-humidity-sensor/

*/

/* Include Libraries */
#include<WiFi.h>

#include <AWS_IOT.h>

#include <Wire.h>

#include <PubSubClient.h>

#include <HardwareSerial.h> 

float getVPP();

//# define WIFI_SSID "dlink-8E7B"      //your wifi ssid
//# define WIFI_PASSWD "123@argus@123"  //your wifi password


# define WIFI_SSID "ACT101651126426"      //your wifi ssid
# define WIFI_PASSWD "65125656"  //your wifi password


//# define CLIENT_ID "TestThing"   // thing unique ID, can be any unique id
# define CLIENT_ID "TestIoThings"   // thing unique ID, can be any unique id
//$aws/things/TestIoThings/shadow/hello
//# define MQTT_TOPIC "esp32/sub"   //topic for the MQTT data
# define MQTT_TOPIC "$aws/things/TestIoThings/shadow/hello"   //topic for the MQTT data

//# define AWS_HOST "a25hzq1rajjanb-ats.iot.ap-south-1.amazonaws.com"     // your host for uploading data to AWS
# define AWS_HOST "a1jdqnbvy5dt1m-ats.iot.us-west-2.amazonaws.com"     // your host for uploading data to AWS
uint8_t data[29];
const int sensorIn = 34;
int mVperAmp = 185;
int i;
double voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
double AmpsRMS1 = 0;
int pubStatus;
//char AmpsRMS[40]="hello argus";
char payload[40];
AWS_IOT aws;       // AWS_IOT instance


/*  Main Functions */
void setup() {

  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);

  /*Serial1.begin(115200, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 19200 bps, 8 bits no parity 1 stop bit​*/
  Serial.begin(9600);
  //Serial.println("ncd.io IoT ESP32 Temperature and Humidity sensor");

  Serial.print("\n  Initializing WIFI: Connecting to ");
  Serial.print("  ");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  //Initializing connetction to AWS
  Serial.println("\n  Connected.\n  Done");
  Serial.println("\n  Initializing connetction to AWS....");
  Serial.print(aws.connect(AWS_HOST, CLIENT_ID));
//  if (aws.connect(AWS_HOST, CLIENT_ID) == 0) {           // connects to host and returns 0 upon success
//    Serial.println("  Connected to AWS\n  Done.");
//  } else {
//    Serial.println("  Connection failed!");
//  }
  Serial.println("  End Done.\n\nStart Done.\n");
}




    
    
void loop(){

/*
   // String temp_humidity = "This is message";     //create string AmpsRMS for publishing
    //String temp_humidity = "Hello Argus";

    temp_humidity.toCharArray(AmpsRMS, 40);
    //temp_humidity.toCharArray("Hello Argus", 40);

    Serial.println("Publishing:- ");
    Serial.println(AmpsRMS);
*/
//  if (Serial1.available()) {
//    data[0] = Serial1.read();
//    delay(sensorIn);
//    if (data[0] == 0x7E) {
//      while (!Serial1.available());
//      for (i = 1; i < 29; i++) {
//        data[i] = Serial1.read();
//        delay(1);
//      }
//      if (data[15] == 0x7F)         // to check if the recive data is correct
//      {
//        if (data[22] == 1)
//        //{
         float voltage = getVPP();
         float VRMS = (voltage/2.0) *0.707;
         float AmpsRMS = (VRMS * 1000)/mVperAmp;
         float AmpsRMS1 = AmpsRMS - 0.35;
        //}
         String Current_sensor = "Ampere: ";
         Current_sensor += String(AmpsRMS);
         Current_sensor += " Amps";
         Current_sensor.toCharArray(payload,40);
         Serial.println("Publishing:- ");
         Serial.println(payload);
         pubStatus = aws.publish(MQTT_TOPIC, payload);
         if (pubStatus == 0) { // publishes AmpsRMS and returns 0 upon success
         Serial.println("Success\n");
    }  
    else if(pubStatus == -30)
    {
       pubStatus = aws.publish(MQTT_TOPIC, payload);
      if(pubStatus == 0) {
        Serial.println("Hurray, Now success\n");
        }
    }
    
    else {
      Serial.println(pubStatus);
      Serial.println("Sorry Failed\n");
    }
  

  delay(2000);

}
float getVPP()
{
  float result,result1;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 4096;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
        //Serial.print(readValue);
        //Serial.println(" readValue");
       
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       
        //Serial.print(maxValue);
 //Serial.println(" maxValue");
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       
        //Serial.print(minValue);
        //Serial.println(" minValue");
       }
   }

  
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/4096.0;
     //Serial.print(result);
        //Serial.println(" result Value"); 
   
   result1 = (maxValue - minValue);
    //Serial.print(result1);
        //Serial.println(" result1 Value"); 
   return result;
 }
