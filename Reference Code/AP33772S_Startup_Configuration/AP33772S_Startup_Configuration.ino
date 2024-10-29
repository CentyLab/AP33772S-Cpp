#include <Wire.h>

// Define the slave address
#define SLAVE_ADDRESS 0x52


// Declare I2C read and write buffers
#define READ_BUFF_LENGTH     128
#define WRITE_BUFF_LENGTH    6
byte readBuf[READ_BUFF_LENGTH] = {0};
byte writeBuf[WRITE_BUFF_LENGTH] = {0};

// Declare variables to keep track of the pin state
int lastState = LOW;  // Last state of the GPIO 13 pin
int currentState;     // Current state of the GPIO 13 pin

bool isI2CConnected = false;  // Global flag for I2C connection status

void interruptINT() {
  // Read the current state of GPIO 13
  currentState = digitalRead(13);
  // Check if the state has changed from LOW to HIGH
  if (lastState == LOW && currentState == HIGH) {
    // Trigger GPIO3 interrupt function
    // Example I2C read and write operations
    i2c_read(SLAVE_ADDRESS, 0x01, 1, readBuf);  //Read the voltage value
  
    // Print the read bytes in hexadecimal format using the function
    Serial.print("STATUS Read bytes in HEX: ");
    printBytesInHex(readBuf, 1);
    // Check if the read byte indicates that booting is complete
    if (readBuf[0] == 0x01) {
      Serial.println("Startup Settings");
      writeBuf[0] = 0x0F; 
      i2c_write(SLAVE_ADDRESS, 0x02, writeBuf, 1);  // MASK 0x02: Interrupt enable mask
      writeBuf[0] = 0xFC; 
      i2c_write(SLAVE_ADDRESS, 0x04, writeBuf, 1);  // CONFIG 0x04: System configuration options
      writeBuf[0] = 0x03; 
      i2c_write(SLAVE_ADDRESS, 0x05, writeBuf, 1);  // PDCONFIG 0x05: PD mode configuration options
      writeBuf[0] = 0x19; 
      i2c_write(SLAVE_ADDRESS, 0x016, writeBuf, 1); // VSELMIN 0x16: The Minimum Selection Voltage, LSB 200mV
      writeBuf[0] = 0x01; 
      i2c_write(SLAVE_ADDRESS, 0x017, writeBuf, 1); // UVPTHR 0x17: UVP threshold, percentage(%) of VREQ;
      writeBuf[0] = 0x19; 
      i2c_write(SLAVE_ADDRESS, 0x018, writeBuf, 1); // OVPTHR 0x18: OVP threshold, offset from VREQ. LSB 80mV
      writeBuf[0] = 0x00; 
      i2c_write(SLAVE_ADDRESS, 0x019, writeBuf, 1); // OCPTHR 0x19: OCP threshold, LSB 50mA
      writeBuf[0] = 0x70; 
      i2c_write(SLAVE_ADDRESS, 0x01A, writeBuf, 1); // OTPTHR 0x1A: OTP threshold, Unit: °C
      writeBuf[0] = 0x70; 
      i2c_write(SLAVE_ADDRESS, 0x01B, writeBuf, 1); // DRTHR 0x1B: De-rating threshold, Unit: °C
      writeBuf[0] = 0x06; 
      i2c_write(SLAVE_ADDRESS, 0x01C, writeBuf, 1); // VDCTHR 0x1C: VDC threshold, percentage(%) difference between VREQ and VOLTAGE
    }
  }
  // Update the last state for the next iteration
  lastState = currentState;
}


void setup() {
  pinMode(12, OUTPUT);  // 設定GPIO 12為OUTPUT
  pinMode(13, INPUT);  // Set GPIO 13 as an input pin
  // put your setup code here, to run once:
  Wire.begin();        // Initialize I2C communication
  Wire.setWireTimeout(3000);  // Set timeout to 3000 microseconds
  disableI2CAndPullLow();
  Serial.begin(115200);  // Initialize Serial communication at 9600 baud rate
}

void loop() {
  // put your main code here, to run repeatedly:
  //handleI2CStartupSettings();
  interruptINT();
  delay(1);
}

void handleI2CStartupSettings(){
  i2c_read(SLAVE_ADDRESS, 0x01, 1, readBuf);  // Read 10 bytes from the defined slave address, starting at register 0x00
    if (readBuf[0] != 0x00) {
        switch (readBuf[0]) {
          case 0x01:
        
            Serial.print("STATUS Read bytes in HEX: ");
            printBytesInHex(readBuf, 1);
            
            Serial.println("Startup Settings");
            writeBuf[0] = 0x03; 
            i2c_write(SLAVE_ADDRESS, 0x02, writeBuf, 1);  // MASK 0x02: Interrupt enable mask
            writeBuf[0] = 0xFC; 
            i2c_write(SLAVE_ADDRESS, 0x04, writeBuf, 1);  // CONFIG 0x04: System configuration options
            writeBuf[0] = 0x03; 
            i2c_write(SLAVE_ADDRESS, 0x05, writeBuf, 1);  // PDCONFIG 0x05: PD mode configuration options
            writeBuf[0] = 0x00; 
            i2c_write(SLAVE_ADDRESS, 0x016, writeBuf, 1); // VSELMIN 0x16: The Minimum Selection Voltage, LSB 200mV
            writeBuf[0] = 0x01; 
            i2c_write(SLAVE_ADDRESS, 0x017, writeBuf, 1); // UVPTHR 0x17: UVP threshold, percentage(%) of VREQ
            writeBuf[0] = 0x19; 
            i2c_write(SLAVE_ADDRESS, 0x018, writeBuf, 1); // OVPTHR 0x18: OVP threshold, offset from VREQ. LSB 80mV
            writeBuf[0] = 0x00; 
            i2c_write(SLAVE_ADDRESS, 0x019, writeBuf, 1); // OCPTHR 0x19: OCP threshold, LSB 50mA
            writeBuf[0] = 0x78; 
            i2c_write(SLAVE_ADDRESS, 0x01A, writeBuf, 1); // OTPTHR 0x1A: OTP threshold, Unit: °C
            writeBuf[0] = 0x78; 
            i2c_write(SLAVE_ADDRESS, 0x01B, writeBuf, 1); // DRTHR 0x1B: De-rating threshold, Unit: °C
            writeBuf[0] = 0x06; 
            i2c_write(SLAVE_ADDRESS, 0x01C, writeBuf, 1); // VDCTHR 0x1C: VDC threshold, percentage(%) difference between VREQ and VOLTAGE
            break;
          default:
            Serial.print("STATUS Read bytes in HEX: ");
            printBytesInHex(readBuf, 1);
            break;
        }
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
