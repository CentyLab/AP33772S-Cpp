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
  usbpd.setOutput(1);
  if(usbpd.getPPSIndex() > 0)
  {
    for(int i = 3600; i <= 20000; i=i+100)
      {
        usbpd.setPPSPDO(usbpd.getPPSIndex(), i, 3000);
        delay(600);
      }
  }
}

