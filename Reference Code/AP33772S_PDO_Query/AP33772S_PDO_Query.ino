#include <Wire.h>

// Define the slave address
#define SLAVE_ADDRESS 0x52
#define MAX_PDO_ENTRIES 13  // Define the maximum number of PDO entries you expect

// Declare I2C read and write buffers
#define READ_BUFF_LENGTH     128
#define WRITE_BUFF_LENGTH    6
byte readBuf[READ_BUFF_LENGTH] = {0};
byte writeBuf[WRITE_BUFF_LENGTH] = {0};

// Declare variables to keep track of the pin state
int lastState = LOW;  // Last state of the GPIO 13 pin
int currentState;     // Current state of the GPIO 13 pin

bool isI2CConnected = false;  // Global flag for I2C connection status


typedef struct {
  union {
    struct {
      unsigned int voltage_max: 8;   // Bits 7:0, VOLTAGE_MAX field
      unsigned int peak_current: 2;  // Bits 9:8, PEAK_CURRENT field
      unsigned int current_max: 4;   // Bits 13:10, CURRENT_MAX field
      unsigned int type: 1;          // Bit 14, TYPE field
      unsigned int detect: 1;        // Bit 15, DETECT field
    } fixed;
    struct {
      unsigned int voltage_max: 8;   // Bits 7:0, VOLTAGE_MAX field
      unsigned int voltage_min: 2;   // Bits 9:8, VOLTAGE_MIN field
      unsigned int current_max: 4;   // Bits 13:10, CURRENT_MAX field
      unsigned int type: 1;          // Bit 14, TYPE field
      unsigned int detect: 1;        // Bit 15, DETECT field
    } pps;
  struct {
      unsigned int voltage_max: 8;   // Bits 7:0, VOLTAGE_MAX field
      unsigned int voltage_min: 2;   // Bits 9:8, VOLTAGE_MIN field
      unsigned int current_max: 4;   // Bits 13:10, CURRENT_MAX field
      unsigned int type: 1;          // Bit 14, TYPE field
      unsigned int detect: 1;        // Bit 15, DETECT field
    } avs;
  struct {
      byte byte0;
      byte byte1;
  };
  };
  unsigned long data;
} SRC_SPRandEPR_PDO_Fields;

// Declare an array of SRC_SPRandEPR_PDO_Fields
SRC_SPRandEPR_PDO_Fields SRC_SPRandEPRpdoArray[MAX_PDO_ENTRIES];  



void setup() {
  pinMode(13, INPUT);  // Set GPIO 13 as an input pin
  // put your setup code here, to run once:
  Wire.begin();        // Initialize I2C communication
  Wire.setWireTimeout(3000);  // Set timeout to 3000 microseconds
  Serial.begin(115200);  // Initialize Serial communication at 9600 baud rate
}

void loop() {
  // put your main code here, to run repeatedly:
  handleI2CStartupSettings();
  readSerialData();
  delay(1);
}

void handleI2CStartupSettings(){
  i2c_read(SLAVE_ADDRESS, 0x01, 1, readBuf);  // Read 10 bytes from the defined slave address, starting at register 0x00
    if (readBuf[0] != 0x00) {
        Serial.print("STATUS Read bytes in HEX: ");
        printBytesInHex(readBuf, 1);
    }
}

void initReadBuf()
{
  for(byte i=0 ; i<READ_BUFF_LENGTH ; i++)
  {
    readBuf[i] = 0;
    
  }
}

// I2C read function
void i2c_read(byte slvAddr, byte cmdAddr, byte len, byte *readBuf) {
  initReadBuf();
  Wire.beginTransmission(slvAddr);
  Wire.write(cmdAddr);
  byte error = Wire.endTransmission();
  if (error == 0) {  // Check if the transmission was successful
    isI2CConnected = true;  // Set the global flag to true
      Wire.requestFrom(slvAddr, len);
      byte i = 0;
      while (Wire.available() && i < len) {  // Check if there is data to read
        readBuf[i] = Wire.read();
        i++;
      }
    } else {
      // Handle the error (you can add your own error handling code here)
      isI2CConnected = false;  // Set the global flag to true
      disableI2CAndPullLow();
       while (1){
          // Read the current state of GPIO 13
          currentState = digitalRead(13);
          // Check if the state has changed from LOW to HIGH
          if (lastState == LOW && currentState == HIGH) {
            enableI2C();  // Re-enable I2C
            break;
          }
       }
    }
}


void initWriteBuf()
{
  for(byte i=0 ; i<WRITE_BUFF_LENGTH ; i++)
  {
    writeBuf[i] = 0;
  }
}
// I2C write function
void i2c_write(byte slvAddr, byte cmdAddr, byte writeBuf[], byte len) {
  Wire.beginTransmission(slvAddr);
  Wire.write(cmdAddr);
  for (byte i = 0; i < len; i++) {
    Wire.write(writeBuf[i]);
  }
  Wire.endTransmission();
  initWriteBuf();
}

void enableI2C() {
  Wire.begin();  // Initialize I2C communication
  pinMode(SDA, INPUT_PULLUP);  // Enable internal pull-up resistor for SDA
  pinMode(SCL, INPUT_PULLUP);  // Enable internal pull-up resistor for SCL
}

void disableI2CAndPullLow() {
  Wire.end();  // Disable I2C to free up SDA and SCL pins
  pinMode(SDA, OUTPUT);  // Set SDA as output
  pinMode(SCL, OUTPUT);  // Set SCL as output
  digitalWrite(SDA, LOW);  // Pull SDA low
  digitalWrite(SCL, LOW);  // Pull SCL low
}

void readSerialData() {
  // Check if data is available to read from the serial port
  if (Serial.available() > 0) {
    char receivedChar = Serial.read();  // Read the incoming character
    if (receivedChar == 'p') {
      if (isI2CConnected) {
        getPDSourcePowerCapabilities();
      }
    }
    
  }
}

void getPDSourcePowerCapabilities() {
  delay(5);
  i2c_read(SLAVE_ADDRESS, 0x01, 1, readBuf);  // Read 10 bytes from the defined slave address, starting at register 0x00
  
  // Print the read bytes in hexadecimal format using the function
  Serial.print("STATUS Read bytes in HEX: ");
  printBytesInHex(readBuf, 1);
  delay(5);
  Serial.println("Get PD Source Power Capabilities:");
  //validPDOIndexBuf[13] = {0};  // Initialize the buffer for valid PDO indices to zero
  //validIndex = 0;  // Initialize the index for valid PDOs to zero
  
  i2c_read(SLAVE_ADDRESS, 0x20, 26, readBuf);  // Read the SRCPDO value
  
  for (int i = 0; i < 26; i += 2) {
    // Store the bytes in the array of structs
    int pdoIndex = (i / 2);  // Calculate the PDO index
    SRC_SPRandEPRpdoArray[pdoIndex].byte0 = readBuf[i];
    SRC_SPRandEPRpdoArray[pdoIndex].byte1 = readBuf[i + 1];
    displayPDOInfo(pdoIndex);
  }
}

void displayPDOInfo(int pdoIndex) {
  // Determine if it's SPR or EPR based on pdoIndex
  bool isEPR = (pdoIndex >= 7 && pdoIndex <= 12);  // 1-6 for SPR, 7-12 for EPR
  // Check if both bytes are zero
  if (SRC_SPRandEPRpdoArray[pdoIndex].byte0 == 0 && SRC_SPRandEPRpdoArray[pdoIndex].byte1 == 0) {
    return;  // If both bytes are zero, exit the function
  }
  
  // Print the PDO type and index
  Serial.print(pdoIndex <= 6 ? " SRC_SPR_PDO" : " SRC_EPR_PDO");
  Serial.print(pdoIndex+1);
  Serial.print(": ");
  
  // Now, the individual fields can be accessed through the union in the struct
  if (SRC_SPRandEPRpdoArray[pdoIndex].fixed.type == 0) {  // Fixed PDO
    // Print parsed values
    Serial.print("Fixed PDO: ");
    Serial.print(SRC_SPRandEPRpdoArray[pdoIndex].fixed.voltage_max * (isEPR ? 200 : 100)); // Voltage in 200mV units for EPR, 100mV for SPR
    Serial.print("mV ");
    displayCurrentRange(SRC_SPRandEPRpdoArray[pdoIndex].fixed.current_max);  // Assuming displayCurrentRange function is available
  } else {  // PPS or AVS PDO
    // Print parsed values
    Serial.print(isEPR ? "AVS PDO: " : "PPS PDO: ");
    if (isEPR) {
      displayEPRVoltageMin(SRC_SPRandEPRpdoArray[pdoIndex].avs.voltage_min);  // Assuming displayVoltageMin function is available
    } else {
      displaySPRVoltageMin(SRC_SPRandEPRpdoArray[pdoIndex].pps.voltage_min);  // Assuming displayVoltageMin function is available
    }
    Serial.print(SRC_SPRandEPRpdoArray[pdoIndex].fixed.voltage_max * (isEPR ? 200 : 100)); // Maximum Voltage in 200mV units for EPR, 100mV for SPR
    Serial.print("mV ");
    displayCurrentRange(SRC_SPRandEPRpdoArray[pdoIndex].fixed.current_max);  // Assuming displayCurrentRange function is available
  }
  Serial.println();
}

void displaySPRVoltageMin(unsigned int current_max) {
  switch (current_max) {
    case 0:
      Serial.print("Reserved");
      break;
    case 1:
      Serial.print("3300mV~");
      break;
    case 2:
      Serial.print("3300mV < VOLTAGE_MIN ≤ 5000mV ");
      break;
    case 3:
      Serial.print("others");
      break;
    default:
      Serial.print("Invalid value");
      break;
  }
}


void displayEPRVoltageMin(unsigned int current_max) {
  switch (current_max) {
    case 0:
      Serial.print("Reserved");
      break;
    case 1:
      Serial.print("15000mV~");
      break;
    case 2:
      Serial.print("15000mV < VOLTAGE_MIN ≤ 20000mV ");
      break;
    case 3:
      Serial.print("others");
      break;
    default:
      Serial.print("Invalid value");
      break;
  }
}

void displayCurrentRange(unsigned int current_max) {
  switch (current_max) {
    case 0:
      Serial.print("0.00A ~ 1.24A (Less than)");
      break;
    case 1:
      Serial.print("1.25A ~ 1.49A");
      break;
    case 2:
      Serial.print("1.50A ~ 1.74A");
      break;
    case 3:
      Serial.print("1.75A ~ 1.99A");
      break;
    case 4:
      Serial.print("2.00A ~ 2.24A");
      break;
    case 5:
      Serial.print("2.25A ~ 2.49A");
      break;
    case 6:
      Serial.print("2.50A ~ 2.74A");
      break;
    case 7:
      Serial.print("2.75A ~ 2.99A");
      break;
    case 8:
      Serial.print("3.00A ~ 3.24A");
      break;
    case 9:
      Serial.print("3.25A ~ 3.49A");
      break;
    case 10:
      Serial.print("3.50A ~ 3.74A");
      break;
    case 11:
      Serial.print("3.75A ~ 3.99A");
      break;
    case 12:
      Serial.print("4.00A ~ 4.24A");
      break;
    case 13:
      Serial.print("4.25A ~ 4.49A");
      break;
    case 14:
      Serial.print("4.50A ~ 4.99A");
      break;
    case 15:
      Serial.print("5.00A ~ (More than)");
      break;
    default:
      Serial.print("Invalid value");
      break;
  }
}
// Function to print an array of bytes in hexadecimal format
void printBytesInHex(byte *buf, int length) {
  
  for (int i = 0; i < length; i++) {
    if (buf[i] < 16) {
      Serial.print("0");  // Add a leading zero for single-digit hex values
    }
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
