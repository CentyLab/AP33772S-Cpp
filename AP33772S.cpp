/* Structs and Resgister list ported from "AP33772 I2C Command Tester" by Joseph Liang
 * Created 11 April 2022
 * Added class and class functions by VicentN for PicoPD evaluation board
 * Created 8 August 2023
 * Updated 6 Oct 2024 - Expose more internal class variable and include better PPS functions
 */

#include "AP33772S.h"

/**
 * @brief Class constuctor
 * @param &wire reference of Wire class. Pass in Wire or Wire1
 */
AP33772::AP33772(TwoWire &wire)
{
    _i2cPort = &wire;
}

/**
 * @brief Check if power supply is good and fetch the PDO profile
 */
void AP33772::begin()
{
    delay(5);
    i2c_read(AP33772S_ADDRESS, CMD_STATUS, 1); // CMD: Read Status into read buff
    i2c_read(AP33772S_ADDRESS, CMD_SRCPDO, 26);

    for (int i = 0; i < 26; i += 2) {
        // Store the bytes in the array of structs
        int pdoIndex = (i / 2);  // Calculate the PDO index
        SRC_SPRandEPRpdoArray[pdoIndex].byte0 = readBuf[i];
        SRC_SPRandEPRpdoArray[pdoIndex].byte1 = readBuf[i + 1];
        displayPDOInfo(pdoIndex);
    }
}

