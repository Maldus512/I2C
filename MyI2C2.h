/* 
 * File:   MyI2C2.h
 * Author: Sabbiolino
 *
 * Created on 1 maggio 2016, 18.21
 */


#ifndef MYI2C2_H
#define	MYI2C2_H

#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"
#include "HardwareProfile.h"

static inline __attribute__((always_inline)) void disableInt() {
        IEC0bits.T1IE = 0;
        IEC0bits.T2IE = 0;
}

static inline __attribute__((always_inline)) void enableInt() {
        IEC0bits.T1IE = 1;
        IEC0bits.T2IE = 1;
}


static inline __attribute__((always_inline)) void MyStartI2C2()
{
    I2C2CONbits.SEN = 1; //StartI2C2();
    while (I2C2CONbits.SEN);
    delay_us(2);
}

static inline __attribute__((always_inline)) void MyStopI2C2()
{
    I2C2CONbits.PEN = 1; //StopI2C2();
    while (I2C2CONbits.PEN);
}

static inline __attribute__((always_inline)) void MyRestartI2C2()
{
    I2C2CONbits.RSEN = 1; //RestartI2C2();
    while (I2C2CONbits.RSEN);
    //delay_us(2);
}

static inline __attribute__((always_inline)) void MyIdleI2C2()
{
    while (I2C2CONbits.SEN || I2C2CONbits.RSEN || I2C2CONbits.PEN || I2C2CONbits.RCEN || I2C2CONbits.ACKEN || I2C2STATbits.TRSTAT);
}

static inline __attribute__((always_inline)) void MyAckI2C2()
{
    I2C2CONbits.ACKDT = 0; //Set for ACk
    I2C2CONbits.ACKEN = 1;
    while (I2C2CONbits.ACKEN); //wait for ACK to complete
}

void Init_I2C();

void I2CWriteReg(unsigned char reg, unsigned char data, unsigned char addr);
unsigned char I2CReadReg(unsigned char reg, unsigned char addr);
unsigned char I2CWriteRegN(unsigned char addr,unsigned char reg, unsigned char *Data, unsigned char N );
unsigned char I2CReadRegN(unsigned char addr,unsigned char reg, unsigned char *Data, unsigned char N );

unsigned int EEAckPolling(unsigned char control);


/*********************************************************************
 * Function:        HDByteWriteI2C()
 *
 * Input:		ControlByte, HighAddr, LowAddr, Data.
 *
 * Output:		None.
 *
 * Overview:		perform a high density byte write of data byte, Data.
 *				starting at the address formed from HighAdd and LowAdd
 *
 * Note:			None
 ********************************************************************/
static inline __attribute__((always_inline)) int HDByteWriteI2C(unsigned char ControlByte,
        unsigned char HighAdd, unsigned char LowAdd, unsigned char data) {
    #if RAM_TYPE == MEM_24XX16
        return byteWrite_24XX16(ControlByte, HighAdd, LowAdd, data);
#elif RAM_TYPE == MEM_24XX1025
        return byteWrite_24XX1025(ControlByte, HighAdd, LowAdd, data);
#endif
}


/*********************************************************************
 * Function:        HDByteReadI2C()
 *
 * Input:		Control Byte, HighAdd, LowAdd, *Data, Length.
 *
 * Output:		None.
 *
 * Overview:		Performs a low density read of Length bytes and stores in *Data array
 *				starting at Address formed from HighAdd and LowAdd.
 *
 * Note:			None
 ********************************************************************/
static inline __attribute__((always_inline)) unsigned int HDByteReadI2C(unsigned char ControlByte,
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *Data) {
    #if RAM_TYPE == MEM_24XX16
        return byteRead_24XX16(ControlByte, HighAdd, LowAdd, data);
#elif RAM_TYPE == MEM_24XX1025
        return byteRead_24XX1025(ControlByte, HighAdd, LowAdd, Data);
#endif
}



/*********************************************************************
 * Function:        HDSequentialWriteI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *wrptr.
 *
 * Output:		Error code:
 *                  0 : everything fine
 *                  1 : too many bytes to write
 *
 * Overview:		Write a page of data from array pointed to be wrptr
 *				starting at address from HighAdd and LowAdd
 *
 * Note:			This function performs all necessary checks and
 *              elaborations on addresses and pages.
 ********************************************************************/
static inline __attribute__((always_inline)) unsigned int HDSequentialWriteI2C(unsigned char ControlByte,
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, unsigned int Length) {
    #if RAM_TYPE == MEM_24XX16
        return sequentialWrite_24XX16(ControlByte, HighAdd, LowAdd, wrptr, Length);
#elif RAM_TYPE == MEM_24XX1025
        return sequentialWrite_24XX1025(ControlByte, HighAdd, LowAdd, wrptr, Length);
#endif
}



/*********************************************************************
 * Function:        HDSequentialReadI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *rdptr, length.
 *
 * Output:		None.
 *
 * Overview:		Performs a sequential read of length bytes starting at address
 *				and places data in array pointed to by *rdptr
 *
 * Note:			None
 ********************************************************************/
static inline __attribute__((always_inline)) unsigned int HDSequentialReadI2C(unsigned char ControlByte,
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length) {
    #if RAM_TYPE == MEM_24XX16
        return sequentialRead_24XX16(ControlByte, HighAdd, LowAdd, wrptr, length);
#elif RAM_TYPE == MEM_24XX1025
        return sequentialRead_24XX1025(ControlByte, HighAdd, LowAdd, rdptr, length);
#endif
}


#endif	/* MYI2C2_H */

