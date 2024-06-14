#include "rfid.hpp"
#include "help_tools.hpp"
#include <cstring>



#define DELAY_MS(ms)            vTaskDelay((ms)/portTICK_PERIOD_MS)
#define SET_RESET_PIN(state)    set_pin(RFID_RST_PIN, (state))




int RFID::waitResponse()
{
    if(this->serial == NULL)
        return STATUS_BUS_ERR;
    for(int i=100; i>0; --i){
        DELAY_MS(10);
        if(this->serial->available()){
            return STATUS_OK;
        }
    }
    return STATUS_TIMEOUT;
}



int RFID::write_bytes_reg(const uint8_t *src, const size_t size)
{
    if(this->serial == NULL)
        return STATUS_BUS_ERR;
    return this->serial->write(src, size) <= 0 
        ? STATUS_BUS_ERR 
        : STATUS_OK;
}

int RFID::write_byte_reg(const uint8_t b)
{
    if(this->serial == NULL)
        return STATUS_BUS_ERR;
    return this->serial->write(b) <= 0 
        ? STATUS_BUS_ERR 
        : STATUS_OK;
}

int RFID::PCD_WriteRegister(uint8_t reg, uint8_t value)
{
    CHECK_RES_RET_ERR(STATUS_OK, write_byte_reg(reg));
    waitResponse();
    this->serial->read();
    return write_byte_reg(value);
}

int RFID::read_reg(uint8_t *b)
{
    if(this->serial == NULL)
        return STATUS_INTERNAL_ERROR;
    waitResponse();
    *b = this->serial->read();
    return STATUS_OK;
}

int RFID::PCD_WriteRegister(uint8_t reg, uint8_t *values, const size_t count)
{
    for (int index=0; index<count; index++){
    	CHECK_RES_RET_ERR(STATUS_OK, this->PCD_WriteRegister(reg, values[index]));
    }
    return STATUS_OK;
}

int RFID::PCD_ReadRegister(uint8_t reg, uint8_t *src)
{
    for(int i=3; i>0; --i){
        CHECK_RES_RET_ERR(STATUS_OK, write_byte_reg(reg | 0x80));
        waitResponse();
        if((STATUS_OK == read_reg(src))){
            return STATUS_OK;
        }
    }
    return STATUS_BUS_ERR;
} 

int RFID::PCD_ReadRegister(uint8_t reg,  uint8_t *values, const size_t count, uint8_t rxAlign)
{
    uint8_t value;
    uint8_t index = 0;
    if (rxAlign) {
		uint8_t mask = 0xFF << rxAlign;
		CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(reg, &value));
		values[0] = (values[0] & ~mask) | (value & mask);
		++index;
	}

	while (index < count) {
		CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(reg, &values[index]));
		++index;
	}
    return STATUS_OK;
}

int RFID::PCD_SetRegisterBits(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(reg, &tmp));
    return PCD_WriteRegister(reg, tmp | mask); 
} 

int RFID::PCD_ClrRegisterBits(uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(reg, &tmp));
    return PCD_WriteRegister(reg, tmp & (~mask));
}

int RFID::PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result)
{
    CHECK_RES_RET_ERR(STATUS_OK,PCD_WriteRegister(CommandReg, PCD_Idle));      // Stop any active command.
    CHECK_RES_RET_ERR(STATUS_OK,PCD_WriteRegister(DivIrqReg, 0x04));           // Clear the CRCIRq interrupt request bit
    CHECK_RES_RET_ERR(STATUS_OK,PCD_SetRegisterBits(FIFOLevelReg, 0x80));      // FlushBuffer = 1, FIFO initialization
    CHECK_RES_RET_ERR(STATUS_OK,PCD_WriteRegister(FIFODataReg, data, length)); // Write data to the FIFO
    CHECK_RES_RET_ERR(STATUS_OK,PCD_WriteRegister(CommandReg, PCD_CalcCRC));   // Start the calculation
    int i = 500;
    uint8_t byte;
    do{
        if (i== 0) return STATUS_TIMEOUT;
        PCD_ReadRegister(DivIrqReg, &byte);
        i -= 1;
    }while(!(byte & 0x04));

    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(CommandReg, PCD_Idle));
    CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(CRCResultRegL, &result[0]));
    return PCD_ReadRegister(CRCResultRegH, &result[1]);
}

void RFID::set_driver(HardwareSerial *serial)
{
    this->serial = serial;
}



int RFID::PCD_Init()
{
	SET_RESET_PIN(false);
	DELAY_MS(10);
    SET_RESET_PIN(true);
    uint8_t count = 0;
    uint8_t byte;
    waitResponse();
    this->serial->flush_input();
DELAY_MS(150);
    
	PCD_WriteRegister(TxModeReg, 0x00);
	PCD_WriteRegister(RxModeReg, 0x00);
	PCD_WriteRegister(ModWidthReg, 0x26);

    // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
    // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(TModeReg, 0x80));     
    // TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25us.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(TPrescalerReg, 0xA9)); 
    // Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(TReloadRegH, 0x03));   
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(TReloadRegL, 0xE8));
    // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(TxASKReg, 0x40));  
    // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)    
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(ModeReg, 0x3D));       
    // Set Rx Gain to max
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(RFCfgReg, RxGain_max)); 
    // Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
    PCD_AntennaOn();
    return STATUS_OK;
}

RFID::~RFID()
{
	SET_RESET_PIN(false);
}

int RFID::PCD_Reset()
{
  CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(CommandReg, PCD_SoftReset)); 
  DELAY_MS(50);
  return STATUS_OK;
}

int RFID::PCD_AntennaOn()
{
    uint8_t value;
    CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(TxControlReg, &value));
    if ((value & 0x03) != 0x03){
        return PCD_WriteRegister(TxControlReg, value | 0x03);
    }
    return STATUS_OK;
} 


int RFID::PCD_TransceiveData(uint8_t *sendData,
                                    uint8_t sendLen,
                                    uint8_t *backData,
                                    uint8_t *backLen,
                                    uint8_t *validBits,
                                    uint8_t rxAlign,
                                    bool checkCRC)
{
    const uint8_t waitIRq = 0x30;   
    return PCD_CommunicateWithPICC(PCD_Transceive, 
                                    waitIRq, 
                                    sendData, 
                                    sendLen, 
                                    backData,
                                    backLen, 
                                    validBits, 
                                    rxAlign,
                                    checkCRC);
} 



int RFID::PCD_CommunicateWithPICC(uint8_t command,
                                         uint8_t waitIRq,
                                         uint8_t *sendData,
                                         uint8_t sendLen,
                                         uint8_t *backData,
                                         uint8_t *backLen,
                                         uint8_t *validBits,
                                         uint8_t rxAlign,
                                         bool    checkCRC)
{
    uint8_t _validBits = 0;
    uint8_t txLastBits = validBits ? *validBits : 0;
    // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
    uint8_t bitFraming = (rxAlign << 4) + txLastBits;
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(CommandReg, PCD_Idle));            // Stop any active command.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(ComIrqReg, 0x7F));                 // Clear all seven interrupt request bits
    CHECK_RES_RET_ERR(STATUS_OK, PCD_SetRegisterBits(FIFOLevelReg, 0x80));            // FlushBuffer = 1, FIFO initialization
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(FIFODataReg, sendData, sendLen));  // Write sendData to the FIFO
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(BitFramingReg, bitFraming));       // Bit adjustments
    CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(CommandReg, command));             // Execute the command
    if (command == PCD_Transceive){
        PCD_SetRegisterBits(BitFramingReg, 0x80);      // StartSend=1, transmission of data starts
    }

    uint8_t byte;
	for (uint16_t i=2000; i > 0; i--) {
		PCD_ReadRegister(ComIrqReg, &byte);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (byte & waitIRq) {					// One of the interrupts that signal success has been set.
			break;
		}
		if (byte & 0x01) {						// Timer interrupt - nothing received in 25ms
			return STATUS_TIMEOUT;
		}
	}

    // Stop now if any errors except collisions were detected.
    uint8_t errorRegValue;
    CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(ErrorReg, &errorRegValue));
    if (errorRegValue & 0x13) return STATUS_ERROR;

  // If the caller wants data back, get it from the RFID.
    if (backData && backLen){
        // Number of bytes in the FIFO
        uint8_t byte_num; 
        CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(FIFOLevelReg, &byte_num));           
        if (byte_num > *backLen)return STATUS_NO_ROOM;
        // Number of bytes returned
        *backLen = byte_num;
        // Get received data from FIFO                       
        CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(FIFODataReg, backData, byte_num,  rxAlign));  
        // RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
        CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(ControlReg, &_validBits));
        _validBits &= 0x07; 
        if (validBits){
            *validBits = _validBits;
        }
    }

    if (errorRegValue & 0x08)return STATUS_COLLISION;

    if (backData && backLen && checkCRC){
        // In this case a MIFARE Classic NAK is not OK.
        if ((*backLen == 1) && (_validBits == 4)) return STATUS_MIFARE_NACK;

        // We need at least the CRC_A value and all 8 bits of the last byte must be received.
        if ((*backLen < 2) || (_validBits != 0))return STATUS_CRC_WRONG;

        // Verify CRC_A - do our own calculation and store the control in controlBuffer.
        uint8_t controlBuffer[2];
        CHECK_RES_RET_ERR(STATUS_OK,PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]));
        if ((backData[*backLen - 2] != controlBuffer[0]) 
            || (backData[*backLen - 1] != controlBuffer[1])) return STATUS_CRC_WRONG;
    }
    return STATUS_OK;
} 

int RFID::PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize)
{
  return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
} 


int RFID::PICC_WakeupA(uint8_t *bufferATQA, uint8_t *bufferSize)
{
  return PICC_REQA_or_WUPA(PICC_CMD_WUPA, bufferATQA, bufferSize);
} 


int RFID::PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize)
{
    uint8_t validBits;

    if (bufferATQA == NULL || *bufferSize < 2) return STATUS_NO_ROOM;

    // ValuesAfterColl=1 => Bits received after collision are cleared.
    PCD_ClrRegisterBits(CollReg, 0x80);

    // For REQA and WUPA we need the short frame format
    // - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
    validBits = 7;
    CHECK_RES_RET_ERR(STATUS_OK, PCD_TransceiveData(&command, 1, bufferATQA, bufferSize, &validBits));
    if ((*bufferSize != 2) || (validBits != 0)) return STATUS_ERROR;
    return STATUS_OK;
} 


int RFID::PICC_Select(Uid *uid, uint8_t validBits)
{
    bool uidComplete;
    bool selectDone;
    bool useCascadeTag;
    uint8_t cascadeLevel = 1;
    uint8_t count;
    uint8_t index;
    uint8_t uidIndex;          // The first index in uid->uidByte[] that is used in the current Cascade Level.
    uint8_t currentLevelKnownBits;   // The number of known UID bits in the current Cascade Level.
    uint8_t buffer[9];         // The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
    uint8_t bufferUsed;        // The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
    uint8_t rxAlign;           // Used in BitFramingReg. Defines the bit position for the first bit received.
    uint8_t txLastBits;        // Used in BitFramingReg. The number of valid bits in the last transmitted byte.
    uint8_t *responseBuffer = nullptr;
    uint8_t responseLength;
    uint8_t result;

  // Description of buffer structure:
  //    Byte 0: SEL         Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3
  //    Byte 1: NVB         Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits.
  //    Byte 2: UID-data or CT    See explanation below. CT means Cascade Tag.
  //    Byte 3: UID-data
  //    Byte 4: UID-data
  //    Byte 5: UID-data
  //    Byte 6: BCC         Block Check Character - XOR of bytes 2-5
  //    Byte 7: CRC_A
  //    Byte 8: CRC_A
  // The BCC and CRC_A is only transmitted if we know all the UID bits of the current Cascade Level.
  //
  // Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
  //    UID size  Cascade level Byte2 Byte3 Byte4 Byte5
  //    ========  ============= ===== ===== ===== =====
  //     4 bytes    1     uid0  uid1  uid2  uid3
  //     7 bytes    1     CT    uid0  uid1  uid2
  //                2     uid3  uid4  uid5  uid6
  //    10 bytes    1     CT    uid0  uid1  uid2
  //                2     CT    uid3  uid4  uid5
  //                3     uid6  uid7  uid8  uid9

  // Sanity checks
    if (validBits > 80) return STATUS_INVALID;


    // Prepare RFID
    // ValuesAfterColl=1 => Bits received after collision are cleared.
    PCD_ClrRegisterBits(CollReg, 0x80);

    // Repeat Cascade Level loop until we have a complete UID.
    uidComplete = false;
    while ( ! uidComplete){
        // Set the Cascade Level in the SEL byte, find out if we need to use the Cascade Tag in byte 2.
        switch (cascadeLevel)
        {
            case 1:
            buffer[0] = PICC_CMD_SEL_CL1;
            uidIndex = 0;
            useCascadeTag = validBits && (uid->size > 4); // When we know that the UID has more than 4 bytes
            break;

            case 2:
            buffer[0] = PICC_CMD_SEL_CL2;
            uidIndex = 3;
            useCascadeTag = validBits && (uid->size > 7); // When we know that the UID has more than 7 bytes
            break;

            case 3:
            buffer[0] = PICC_CMD_SEL_CL3;
            uidIndex = 6;
            useCascadeTag = false;            // Never used in CL3.
            break;

            default:
            return STATUS_INTERNAL_ERROR;
            //break;
        }

        // How many UID bits are known in this Cascade Level?
        if(validBits > (8 * uidIndex)){
            currentLevelKnownBits = validBits - (8 * uidIndex);
        } else {
            currentLevelKnownBits = 0;
        }

        // Copy the known bits from uid->uidByte[] to buffer[]
        index = 2; // destination index in buffer[]
        if (useCascadeTag) {
            buffer[index++] = PICC_CMD_CT;
        }

        uint8_t bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of bytes needed to represent the known bits for this level.
        if(bytesToCopy){
            // Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag
            uint8_t maxBytes = useCascadeTag ? 3 : 4;
            if (bytesToCopy > maxBytes){
                bytesToCopy = maxBytes;
            }

            for (int c = 0; c < bytesToCopy; c++){
                buffer[index++] = uid->uidByte[uidIndex + c];
            }
        }

        // Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
        if (useCascadeTag){
            currentLevelKnownBits += 8;
        }

        // Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
        selectDone = false;
        while ( ! selectDone){
            // Find out how many bits and bytes to send and receive.
            if (currentLevelKnownBits >= 32){ // All UID bits in this Cascade Level are known. This is a SELECT.
            //Serial.print("SELECT: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
            buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole bytes

            // Calulate BCC - Block Check Character
            buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];

            // Calculate CRC_A
            CHECK_RES_RET_ERR(STATUS_OK, PCD_CalculateCRC(buffer, 7, &buffer[7]));

            txLastBits      = 0; // 0 => All 8 bits are valid.
            bufferUsed      = 9;

            // Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
            responseBuffer  = &buffer[6];
            responseLength  = 3;
            } else { // This is an ANTICOLLISION.
                //Serial.print("ANTICOLLISION: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
                txLastBits     = currentLevelKnownBits % 8;
                count          = currentLevelKnownBits / 8;  // Number of whole bytes in the UID part.
                index          = 2 + count;                  // Number of whole bytes: SEL + NVB + UIDs
                buffer[1]      = (index << 4) + txLastBits;  // NVB - Number of Valid Bits
                bufferUsed     = index + (txLastBits ? 1 : 0);

                // Store response in the unused part of buffer
                responseBuffer = &buffer[index];
                responseLength = sizeof(buffer) - index;
            }

            // Set bit adjustments
            rxAlign = txLastBits;                     // Having a seperate variable is overkill. But it makes the next line easier to read.
            CHECK_RES_RET_ERR(STATUS_OK, PCD_WriteRegister(BitFramingReg, (rxAlign << 4) + txLastBits));  // RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

            // Transmit the buffer and receive the response.
            result = PCD_TransceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign);
            if (result == STATUS_COLLISION){ 
                // More than one PICC in the field => collision.
                CHECK_RES_RET_ERR(STATUS_OK, PCD_ReadRegister(CollReg, &result));     // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]
                if (result & 0x20){
                    return STATUS_COLLISION; // Without a valid collision position we cannot continue
                }

                uint8_t collisionPos = result & 0x1F; // Values 0-31, 0 means bit 32.
                if (collisionPos == 0){
                    collisionPos = 32;
                }
                // No progress - should not happen
                if (collisionPos <= currentLevelKnownBits){ 
                    return STATUS_INTERNAL_ERROR;
                }

                // Choose the PICC with the bit set.
                currentLevelKnownBits = collisionPos;
                count = (currentLevelKnownBits - 1) % 8; // The bit to modify
                index = 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
                buffer[index] |= (1 << count);
            } else if (result != STATUS_OK) {
                return result;
            } else {
                // STATUS_OK
                if (currentLevelKnownBits >= 32){
                    // This was a SELECT.
                    selectDone = true; // No more anticollision
                    // We continue below outside the while.
                } else { 
                    // This was an ANTICOLLISION.
                    // We now have all 32 bits of the UID in this Cascade Level
                    currentLevelKnownBits = 32;
                    // Run loop again to do the SELECT.
                }
            }
        } 

        // We do not check the CBB - it was constructed by us above.
        // Copy the found UID bytes from buffer[] to uid->uidByte[]
        index       = (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
        bytesToCopy = (buffer[2] == PICC_CMD_CT) ? 3 : 4;
        for (count = 0; count < bytesToCopy; count++){
            uid->uidByte[uidIndex + count] = buffer[index++];
        }

        // Check response SAK (Select Acknowledge)
        if (responseLength != 3 || txLastBits != 0){   
            // SAK must be exactly 24 bits (1 byte + CRC_A).
            return STATUS_ERROR;
        }

        // Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
        CHECK_RES_RET_ERR(STATUS_OK, PCD_CalculateCRC(responseBuffer, 1, &buffer[2]));

        if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])){
            return STATUS_CRC_WRONG;
        }

        if (responseBuffer[0] & 0x04){ 
            // Cascade bit set - UID not complete yes
            cascadeLevel++;
        } else {
            uidComplete = true;
            uid->sak = responseBuffer[0];
        }
    } 

    // Set correct uid->size
    uid->size = 3 * cascadeLevel + 1;

    return STATUS_OK;
}


int RFID::PICC_HaltA()
{
    int result;
    uint8_t buffer[4];

    // Build command buffer
    buffer[0] = PICC_CMD_HLTA;
    buffer[1] = 0;

    // Calculate CRC_A
    CHECK_RES_RET_ERR(STATUS_OK, PCD_CalculateCRC(buffer, 2, &buffer[2]));

    // Send the command.
    // If the PICC responds with any modulation during a period of 1 ms after the end of the frame containing the
    // HLTA command, this response shall be interpreted as 'not acknowledge'.
    // We interpret that this way: Only STATUS_TIMEOUT is an success.
    result = PCD_TransceiveData(buffer, sizeof(buffer), NULL, 0);
    if (result == STATUS_TIMEOUT){
        result = STATUS_OK;
    } else if (result == STATUS_OK) { 
        result = STATUS_ERROR;
    }

    return result;
}


int RFID::PCD_Authenticate(uint8_t command, uint8_t blockAddr, MIFARE_Key *key, Uid *uid)
{
    uint8_t i, waitIRq = 0x10;    // IdleIRq

    // Build command buffer
    uint8_t sendData[12];
    sendData[0] = command;
    sendData[1] = blockAddr;

    for (i = 0; i < MF_KEY_SIZE; i++){  
        sendData[2+i] = key->keyByte[i];
    }

    for (i = 0; i < 4; i++){ 
        // The first 4 bytes of the UID
        sendData[8+i] = uid->uidByte[i];
    }

    // Start the authentication.
    return PCD_CommunicateWithPICC(PCD_MFAuthent, waitIRq, &sendData[0], sizeof(sendData));
} 


void RFID::PCD_StopCrypto1()
{
  // Clear MFCrypto1On bit
  PCD_ClrRegisterBits(Status2Reg, 0x08);  
} 

/*
 * Reads 16 bytes (+ 2 bytes CRC_A) from the active PICC.
 */
int RFID::MIFARE_Read(uint8_t blockAddr, uint8_t *buffer, uint8_t *bufferSize)
{
    if ((buffer == NULL) || (*bufferSize < 18)) return STATUS_NO_ROOM;

    buffer[0] = PICC_CMD_MF_READ;
    buffer[1] = blockAddr;

    // Calculate CRC_A
    CHECK_RES_RET_ERR(STATUS_OK, PCD_CalculateCRC(buffer, 2, &buffer[2]));

    // Transmit the buffer and receive the response, validate CRC_A.
    return PCD_TransceiveData(buffer, 4, buffer, bufferSize, NULL, 0, true);
}

/*
 * Writes 16 bytes to the active PICC.
 */
int RFID::MIFARE_Write(uint8_t blockAddr, uint8_t *buffer, uint8_t bufferSize)
{
    // Sanity check
    if (buffer == NULL || bufferSize < 16) return STATUS_INVALID;

    // Mifare Classic protocol requires two communications to perform a write.
    // Step 1: Tell the PICC we want to write to block blockAddr.
    uint8_t cmdBuffer[2];
    cmdBuffer[0] = PICC_CMD_MF_WRITE;
    cmdBuffer[1] = blockAddr;
    // Adds CRC_A and checks that the response is MF_ACK.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_MIFARE_Transceive(cmdBuffer, 2));

    // Step 2: Transfer the data
    // Adds CRC_A and checks that the response is MF_ACK.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_MIFARE_Transceive(buffer, bufferSize));

    return STATUS_OK;
} 

/*
 * Writes a 4 byte page to the active MIFARE Ultralight PICC.
 */
int RFID::MIFARE_UltralightWrite(uint8_t page, uint8_t *buffer, uint8_t bufferSize)
{
    if (buffer == NULL || bufferSize < 4)return STATUS_INVALID;
    uint8_t cmdBuffer[6];
    cmdBuffer[0] = PICC_CMD_UL_WRITE;
    cmdBuffer[1] = page;
    memcpy(&cmdBuffer[2], buffer, 4);
    // Perform the write
    // Adds CRC_A and checks that the response is MF_ACK.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_MIFARE_Transceive(cmdBuffer, 6)); 
    return STATUS_OK;
}


int RFID::MIFARE_Decrement(uint8_t blockAddr, uint32_t delta)
{
  return MIFARE_TwoStepHelper(PICC_CMD_MF_DECREMENT, blockAddr, delta);
} 


int RFID::MIFARE_Increment(uint8_t blockAddr, uint32_t delta)
{
  return MIFARE_TwoStepHelper(PICC_CMD_MF_INCREMENT, blockAddr, delta);
} 


int RFID::MIFARE_Restore(uint8_t blockAddr)
{
  return MIFARE_TwoStepHelper(PICC_CMD_MF_RESTORE, blockAddr, 0L);
} 


int RFID::MIFARE_TwoStepHelper(uint8_t command, uint8_t blockAddr, uint32_t data)
{
    uint8_t cmdBuffer[2]; // We only need room for 2 bytes.

    // Step 1: Tell the PICC the command and block address
    cmdBuffer[0] = command;
    cmdBuffer[1] = blockAddr;

    // Adds CRC_A and checks that the response is MF_ACK.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_MIFARE_Transceive(cmdBuffer, 2));

    // Step 2: Transfer the data
    // Adds CRC_A and accept timeout as success.
    CHECK_RES_RET_ERR(STATUS_OK, PCD_MIFARE_Transceive((uint8_t *) &data, 4, true));

    return STATUS_OK;
} // End MIFARE_TwoStepHelper()

/*
 * MIFARE Transfer writes the value stored in the volatile memory into one MIFARE Classic block.
 */
int RFID::MIFARE_Transfer(uint8_t blockAddr)
{
    uint8_t cmdBuffer[2]; // We only need room for 2 bytes.

    // Tell the PICC we want to transfer the result into block blockAddr.
    cmdBuffer[0] = PICC_CMD_MF_TRANSFER;
    cmdBuffer[1] = blockAddr;

    // Adds CRC_A and checks that the response is MF_ACK.
    return PCD_MIFARE_Transceive(cmdBuffer, 2);
} 



int RFID::PCD_MIFARE_Transceive(uint8_t *sendData, uint8_t sendLen, bool acceptTimeout)
{
    int result;
    uint8_t cmdBuffer[18]; // We need room for 16 bytes data and 2 bytes CRC_A.

    // Sanity check
    if (sendData == NULL || sendLen > 16) return STATUS_INVALID;

    // Copy sendData[] to cmdBuffer[] and add CRC_A
    memcpy(cmdBuffer, sendData, sendLen);
    CHECK_RES_RET_ERR(STATUS_OK, PCD_CalculateCRC(cmdBuffer, sendLen, &cmdBuffer[sendLen]));

    sendLen += 2;

    // Transceive the data, store the reply in cmdBuffer[]
    uint8_t waitIRq = 0x30;    // RxIRq and IdleIRq
    uint8_t cmdBufferSize = sizeof(cmdBuffer);
    uint8_t validBits = 0;
    result = PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, cmdBuffer, sendLen, cmdBuffer, &cmdBufferSize, &validBits);
    if (acceptTimeout && result == STATUS_TIMEOUT){
        return STATUS_OK;
    }

    if (result != STATUS_OK){
        return result;
    }

    // The PICC must reply with a 4 bit ACK
    if (cmdBufferSize != 1 || validBits != 4){
        return STATUS_ERROR;
    }

    if (cmdBuffer[0] != MF_ACK){
        return STATUS_MIFARE_NACK;
    }

    return STATUS_OK;
} 


int RFID::PICC_GetType(uint8_t sak)
{
    int retType = PICC_TYPE_UNKNOWN;

    if (sak & 0x04){ 
        // UID not complete
        retType = PICC_TYPE_NOT_COMPLETE;
    } else {
        switch (sak)
        {
            case 0x09: retType = PICC_TYPE_MIFARE_MINI; break;
            case 0x08: retType = PICC_TYPE_MIFARE_1K;   break;
            case 0x18: retType = PICC_TYPE_MIFARE_4K;   break;
            case 0x00: retType = PICC_TYPE_MIFARE_UL;   break;
            case 0x10:
            case 0x11: retType = PICC_TYPE_MIFARE_PLUS; break;
            case 0x01: retType = PICC_TYPE_TNP3XXX;     break;
            default:
            if (sak & 0x20){
                retType = PICC_TYPE_ISO_14443_4;
            }else if (sak & 0x40){
                retType = PICC_TYPE_ISO_18092;
            }
            break;
        }
    }
    return retType;
} 

/*
 * Returns a string pointer to the PICC type name.
 */
char* RFID::PICC_GetTypeName(uint8_t piccType)
{
    if(piccType >= RFID::MAX_TYPENAMES){
        piccType = RFID::MAX_TYPENAMES - 1;
    }
    return((char *) _TypeNamePICC[piccType]);
} 


char* RFID::GetStatusCodeName(uint8_t code)
{
  return((char *) _ErrorMessage[code]);
} 

/*
 * Calculates the bit pattern needed for the specified access bits. In the [C1 C2 C3] tupples C1 is MSB (=4) and C3 is LSB (=1).
 */
void RFID::MIFARE_SetAccessBits(uint8_t *accessBitBuffer,  
                                   uint8_t g0,                
                                   uint8_t g1,                
                                   uint8_t g2,                
                                   uint8_t g3)
{
    uint8_t c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
    uint8_t c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
    uint8_t c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);

    accessBitBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
    accessBitBuffer[1] =          c1 << 4 | (~c3 & 0xF);
    accessBitBuffer[2] =          c3 << 4 | c2;
} 


bool RFID::PICC_IsNewCard(void)
{
    uint8_t bufferATQA[2];
    uint8_t bufferSize = sizeof(bufferATQA);

    uint8_t result = PICC_RequestA(bufferATQA, &bufferSize);
    return ((result == STATUS_OK) || (result == STATUS_COLLISION));
}


bool RFID::PICC_ReadCard(void)
{
    return PICC_Select(&uid) == STATUS_OK;
} 
