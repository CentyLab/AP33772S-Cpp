## Info
AP33772**S** is a USB PD3.1 Sink controller that communicate via I2C. This is an upgrade IC from the previous version AP33772 IC. With this library, can you use the IC with any Arduino compatable board as it is based on the Wire.h library. This library currently does not support interrupt from the AP33772**S**.

The code has been written for RP2040 micro-controller and ultilizing timer1. Adaptation to other micro-controler will require update in I2C Read/Write and timer function to maintain AVS operation. A repeat AVS request in less than 750ms is needed, else the charger will issue a hard reset.

![AVSprofileChange](examples/AVSprofileChange.gif?raw=true "AVS")

Main feature of the library:
+ Fix voltage request
+ PPS voltage/current request
+ AVS voltage request
+ Voltage reading
+ Current reading
+ NTC temperature reading
+ Output back-to-back NMOS control
+ Set/read different safety values

## Using Wire1
The RP2040 has two different I2C physical driver, `Wire` (I2C0) and `Wire1`(I2C1). The default channel is selected to be `Wire` in AP33772S Constructor but one can overwrite it. PicoPD Pro connect to AP33772S using I2C0 at pin 0 and pin 1.

```
AP33772S usbpd(Wire1);
```

## Requesting PPS Voltage/Current

Example profile from UGREEN 140W charger

![UGREEN140WProfile](examples/ProfileDisplayUGREEN140W.png?raw=true "Title")

When setting any power mode like fixed PDO, PPS, or AVS, the code require you to also input the maximum current expected.

The example code set the PPS voltage with the max current the charge/cable can deliver.

Example:
```
AP33772S usbpd; //create object with Wire0
setPPSPDO(usbpd.getPPSIndex(), 4200, 1000); // Requesting CV/CC supply 4.2V max 1A PPS
// setPPSPDO(6, 4200, 1000); // Same as
delay(2000);
setFixPDO(2,3000); // Requesting CV supply 9V max 3A fix PDO
```

## Example code
Drag and drop file from "Compiled for PicoPD" to test out the code without compiling. However, some code does has load switch turn on by default. Ensure your connected device can handle the voltage at VBUS.

Else if you would like to compile your own code, ensure to install [Earlephilhower's Pico Core](https://github.com/earlephilhower/arduino-pico#installation) so that you can call Wire.setSCL() and Wire.setSDA()


