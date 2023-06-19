#include <SoftwareSerial.h>
#include <DHT.h>
#include "TFMini.h"
#define DHTTYPE DHT11
#define DHTPIN 4
#include <SD.h>
#define SD_ChipSelectPin 10
#include <TMRpcm.h>
#include <SPI.h>
DHT dht(DHTPIN, DHTTYPE);
TFMini tfmini;
TMRpcm tmrpcm;
const int BUTTON = 7;
int BUTTONstate = 0;
SoftwareSerial SerialTFMini(2, 3);          //The only value that matters here is the first one, 2, Rx
int led=8;
void getTFminiData(int *distance, int *strength)
{
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static int rx[9];
  
  if (SerialTFMini.available())
  {
    rx[i] = SerialTFMini.read();
    if (rx[0] != 0x59)
    {
      i = 0;
    }
    else if (i == 1 && rx[1] != 0x59)
    {
      i = 0;
    }
    else if (i == 8)
    {
      for (j = 0; j < 8; j++)
      {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256))
      {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    }
    else
    {
      i++;
    }
  }
}
 

void setup()
{
  Serial.begin(115200);       //Initialize hardware serial port (serial debug port)
  while (!Serial);            // wait for serial port to connect. Needed for native USB port only
  Serial.println ("Initializing...");
  SerialTFMini.begin(TFMINI_BAUDRATE);    //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);            //Initialize the TF Mini sensor
  dht.begin();
  
  tmrpcm.speakerPin = 9; 

  
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
    return;   // don't do anything more if not
  }
  else{  
    Serial.println("SD ok");  
  }
  pinMode(led,OUTPUT);
}

void loop()
{


  int distance = 0;
  int strength = 0;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  getTFminiData(&distance, &strength);

  while (!distance)
  {
    getTFminiData(&distance, &strength);
    if (distance )
    {
      Serial.print("Lidar distance : ");
      Serial.print(distance);
      Serial.print("cm\t");
      Serial.print("strength: ");
      Serial.println(strength);

     if (distance <100){
     
     Serial.println("Be Alert");
     tmrpcm.setVolume(5);
     tmrpcm.play("10.wav");
     digitalWrite(led,HIGH);
     delay(100);
     digitalWrite(led,LOW);
     }

      }
       
    }
  
  BUTTONstate = digitalRead(BUTTON);
switch (BUTTONstate)
  {
case HIGH:
   {
    Serial.print(F(" Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("C "));
    Serial.print(f);
    Serial.print(F("F  Heat index: "));
    
    Serial.print(hic);
    Serial.print(F("C "));
    Serial.print(hif);
    Serial.println(F("F"));
        if ( h>30 , h<70){
      Serial.println("Weather is pleasant to go out");
      tmrpcm.setVolume(5);
     
      tmrpcm.play("3.wav");
      delay(100);
        }
    
    else{
      Serial.println("Weather is not pleasant to go out");
      tmrpcm.setVolume(5);
      tmrpcm.play("4.wav");
    }

    break;
  }
default:
  {
    break;
  }
}
}
