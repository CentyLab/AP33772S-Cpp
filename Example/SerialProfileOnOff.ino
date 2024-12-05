#include <Arduino.h>
#include <AP33772S.h>
#include "hardware/timer.h"

// put function declarations here:
AP33772S usbpd;


void setup() {
  // put your setup code here, to run once:
  Wire.setSCL(1);
  Wire.setSDA(0);
  Wire.begin();

  Serial.begin(115200);
  delay(1000); //Ensure everything got enough time to bootup
  usbpd.begin();
}

void loop() {
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();  // Read the incoming character
    if (receivedChar == 'p') 	  // Print out profile
      usbpd.displayProfiles();	  
    else if (receivedChar == 'o') // On
      usbpd.setOutput(1);
    else if(receivedChar == 'f') // Off
      usbpd.setOutput(0);
    else if(receivedChar == 't') // Print temperature
      Serial.println(usbpd.readTemp());
  }
}

