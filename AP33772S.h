/* Structs and basic function ported from "I²C Master Sample Code Introduction for AP33772S" by Diodes
 * Added class and class functions by CentyLab for PicoPD Pro evaluation board
 * Created 6 Oct 2024
 */

#include <Arduino.h>
#include <Wire.h>

#ifndef __AP33772S__
#define __AP33772S__

#define MAX_PDO_ENTRIES 13  // Define the maximum number of PDO entries you expect

#define AP33772S_ADDRESS 0x52
#define READ_BUFF_LENGTH 128
#define WRITE_BUFF_LENGTH 6
#define SRCPDO_LENGTH 28

#define CMD_STATUS    0x01
#define CMD_MASK      0x02
#define CMD_OPMODE    0x03
#define CMD_CONFIG    0x04
#define CMD_PDCONFIG  0x05
// Temperature setting register
#define CMD_TR25     0x0C 
#define CMD_TR50     0x0D 
#define CMD_TR75     0x0E
#define CMD_TR100    0x0F

//Power reading related
#define CMD_VOLTAGE   0x11
#define CMD_CURRENT   0x12
#define CMD_TEMP      0x13
#define CMD_VREQ      0x14
#define CMD_IREQ      0x15


#define CMD_VSELMIN   0x16 //Minimum Selection Voltage
#define CMD_UVPTHR    0x17
#define CMD_OVPTHR    0x18
#define CMD_OCPTHR    0x19
#define CMD_OTPTHR    0x1A
#define CMD_DRTHR     0x1B

#define CMD_SRCPDO    0x20

#define CMD_PD_REQMSG 0x31
#define CMD_PD_CMDMSG 0x32
#define CMD_PD_MSGRLT 0x33


//DONE
typedef enum
{
  STARTED_MSK   = 1 << 0,     // 0000 0001
  READY_MSK     = 1 << 1,     // 0000 0010
  NEWPDO_MSK    = 1 << 2,     // 0000 0100
  UVP_MSK       = 1 << 3,     // 0001 0000
  OVP_MSK       = 1 << 4,     // 0010 0000
  OCP_MSK       = 1 << 5,     // 0100 0000
  OTP_MSK       = 1 << 6      // 1000 0000
} AP33772_MASK;

typedef enum
{
  STARTED_MSK   = 1 << 0,     // 0000 0001
  READY_MSK     = 1 << 1,     // 0000 0010
  NEWPDO_MSK    = 1 << 2,     // 0000 0100
  UVP_MSK       = 1 << 3,     // 0001 0000
  OVP_MSK       = 1 << 4,     // 0010 0000
  OCP_MSK       = 1 << 5,     // 0100 0000
  OTP_MSK       = 1 << 6      // 1000 0000
} AP33772_STATUS;

//Not sure if used
typedef struct
{
  union
  {
    struct
    {
      byte newNegoSuccess : 1;
      byte newNegoFail : 1;
      byte negoSuccess : 1;
      byte negoFail : 1;
      byte reserved_1 : 4;
    };
    byte negoEvent;
  };
  union
  {
    struct
    {
      byte ovp : 1;
      byte ocp : 1;
      byte otp : 1;
      byte dr : 1;
      byte reserved_2 : 4;
    };
    byte protectEvent;
  };
} EVENT_FLAG_T;

//DONE
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

//DONE
typedef struct {
  union {
    struct {
      unsigned int VOLTAGE_SEL: 8;  // Bits 7:0, Output Voltage Select
      unsigned int CURRENT_SEL: 4;  // Bits 11:8, Operating Current Select
      unsigned int PDO_INDEX: 4;  // Bits 15:12, Source PDO index select
      
    } REQMSG_Fields;
    struct {
      byte byte0;
      byte byte1;
    };
  unsigned long data;
  };
} RDO_DATA_T;

class AP33772
{
public:
  AP33772(TwoWire &wire = Wire);
  void begin();
  // void setVoltage(int targetVoltage); // Unit in mV
  // void setPDO(uint8_t PDOindex);
  // void setMaxCurrent(int targetMaxCurrent); // Unit in mA
  // void setNTC(int TR25, int TR50, int TR75, int TR100);
  // void setDeratingTemp(int temperature);
  // void setMask(AP33772_MASK flag);
  // void clearMask(AP33772_MASK flag);
  // void writeRDO();
  // int readVoltage();
  // int readCurrent();
  // int getMaxCurrent() const;
  // int readTemp();
  // void printPDO();
  // void reset();

  int getNumPDO();
  int getPPSIndex();
  int getPDOMaxcurrent(uint8_t PDOindex);
  int getPDOVoltage(uint8_t PDOindex);
  int getPPSMinVoltage(uint8_t PPSindex);
  int getPPSMaxVoltage(uint8_t PPSindex);
  int getPPSMaxCurrent(uint8_t PPSindex);
  void setSupplyVoltageCurrent(int targetVoltage, int targetCurrent);
  byte existPPS = 0; // PPS flag for setVoltage()

private:
  void i2c_read(byte slvAddr, byte cmdAddr, byte len);
  void i2c_write(byte slvAddr, byte cmdAddr, byte len);
  TwoWire *_i2cPort;
  byte readBuf[READ_BUFF_LENGTH] = {0};
  byte writeBuf[WRITE_BUFF_LENGTH] = {0};
  byte numPDO = 0;    // source PDO number
  byte indexPDO = 0;  // PDO index, start from index 0
  int reqPpsVolt = 0; // requested PPS voltage, unit:20mV

  int8_t PPSindex = 8;

  AP33772_STATUS_T ap33772_status = {0};
  EVENT_FLAG_T event_flag = {0};
  RDO_DATA_T rdoData = {0};

  SRC_SPRandEPR_PDO_Fields SRC_SPRandEPRpdoArray[MAX_PDO_ENTRIES] = {0}; 
};

#endif