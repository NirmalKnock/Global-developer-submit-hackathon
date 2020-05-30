#include <SoftwareSerial.h>
#include<Wire.h>
SoftwareSerial gprsSerial(2,3);//RX,TX of arduino

#include <String.h>
#include <DHT.h> 
#define DHTPIN 4
int voltage=8;
int piroutput;
int pirsensor=9;

DHT dht(DHTPIN, DHT11);
 
void setup()
{
  gprsSerial.begin(9600); // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate 
  dht.begin();
  pinMode(pirsensor,INPUT);
  pinMode(voltage,OUTPUT);
 
  Serial.print("Humidity:");
  Serial.print("Temperature:");
}
 
void loop(){
  
  
  
  digitalWrite(voltage,HIGH);
   // reading temp& humid
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 
  delay(100); 
  Serial.println(h);
  Serial.println(t);
  
     
    
    
   //gprs check valid connection
  if (gprsSerial.available())
    Serial.write(gprsSerial.read());

  gprsSerial.println("AT");
  delay(1000);

  gprsSerial.println("AT+CPIN?");
  delay(1000);

  gprsSerial.println("AT+CREG?");
  delay(1000);

  gprsSerial.println("AT+CGATT?");
  delay(1000);

  gprsSerial.println("AT+CIPSHUT");
  delay(1000);

  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);

  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();//calling a fuctin
  //upload temp,humd
  String str="GET https://api.thingspeak.com/update?api_key=XK4FD10YPUDXUF3M&field1=" + String(t) +"&field2="+String(h);
  Serial.println(str);
  gprsSerial.println(str);//begin send data to remote server
  
  delay(4000);
  ShowSerialData();

  gprsSerial.println((char)26);//sending
  delay(5000);//waitting for reply, 
  gprsSerial.println();
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
 
 
} 

void ShowSerialData()
{
  while(gprsSerial.available()!=0)
  Serial.write(gprsSerial.read());
  delay(5000); 
  
}