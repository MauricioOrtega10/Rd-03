#include <Arduino.h>
#include "RD-03.h"

#define RX_PIN 16 // ESP32 RX pin
#define TX_PIN 17 // ESP32 TX pin

//RD-03 class constructor.
RD03 rd03_1(RX_PIN, TX_PIN, &Serial1);

unsigned long currentMillis;
unsigned long previousMillis = 0;
unsigned long previousMillis_print = 0;
const long interval_data_refresh = 100;
const long interval_data_refresh_print = 100;

void setup() 
{
  Serial.begin(115200);

  //Initialize the RD-03 module with predetermined values.
  rd03_1.begin();
}

void loop() 
{
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval_data_refresh) 
  {
    previousMillis = currentMillis;
    //Reads and processes the values sent by serial from the RD-03 sensor.
    //Note: Perform data reading at least every 100ms.
    rd03_1.read(); 
  }

  if (currentMillis - previousMillis_print >= interval_data_refresh_print) 
  {
    previousMillis_print = currentMillis;
    //Obtains the distance values and whether someone has been detected according to the values set in the begin() function.
    Serial.print(rd03_1.getSomeone());
    Serial.print("\t");
    Serial.println(rd03_1.getDistance());
  }
}

