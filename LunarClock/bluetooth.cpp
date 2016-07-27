/* 
   - GND
   - Pin 2 to HC-05 TXD
   - Pin 3 to HC-05 RXD
   - GND to HC-05 KEY
   - VCC to HC-05 VCC 
*/

#include "pins.h"
#include "bluetooth.h"
#include <Arduino.h>

SoftwareSerial BTSerial(HC_05_TXD_ARDUINO_RXD, HC_05_RXD_ARDUINO_TXD); // RX | TX

void bluetooth_setup(){
  
  BTSerial.begin(9600);  // HC-05 default speed 
  
}

  







