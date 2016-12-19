#include "HardwareProfile.h"

#include "MyI2C2.h"
#include "EEPROM/24XX1025.h"
#include "EEPROM/24XX16.h"


void Init_I2C()
{
    I2C2STAT = 0x0000;
    I2C2RCV = 0x0000;
    I2C2TRN = 0x0000;
    IFS3bits.MI2C2IF = 0;
    IEC3bits.MI2C2IE = 0;
    CloseI2C2();
    OpenI2C2(I2C2_ON & I2C2_IDLE_CON & I2C2_CLK_HLD &
            I2C2_IPMI_DIS & I2C2_7BIT_ADD & I2C2_SLW_EN &
            I2C2_SM_DIS & I2C2_GCALL_DIS & I2C2_STR_DIS &
            I2C2_NACK & I2C2_ACK_DIS & I2C2_RCV_DIS &
            I2C2_STOP_DIS & I2C2_RESTART_DIS & I2C2_START_DIS, (unsigned int) I2C_BRG);
    
    IdleI2C2();
}

void I2CWriteReg(unsigned char reg, unsigned char data, unsigned char addr) {
    disableInt();
    MyStartI2C2();

    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        MyIdleI2C2();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        MasterWriteI2C2(data);
        MyIdleI2C2();
        
    } while(I2C2STATbits.ACKSTAT);
    
    MyStopI2C2();
    enableInt();
}

unsigned char I2CReadReg(unsigned char reg, unsigned char addr) {
    unsigned char valore;
    disableInt();
    MyStartI2C2();

    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        
        MasterWriteI2C2(reg);
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT);

    MyRestartI2C2();
    MasterWriteI2C2(addr | 1);
    MyIdleI2C2();
    valore = MasterReadI2C2();

    MyStopI2C2();
    enableInt();

    return valore;
}

unsigned char
I2CWriteRegN(unsigned char addr, unsigned char reg, unsigned char *Data, unsigned char N)
{
    unsigned char *ptr;
    unsigned char len;
    disableInt();
    MyStartI2C2();
    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        ptr = Data;
        len = N;
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        MyIdleI2C2();
        
        while (len--) {
            MasterWriteI2C2(*ptr++);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT) {
                break;
            }
        }
    }
    while (I2C2STATbits.ACKSTAT);

    MyStopI2C2();
    MyIdleI2C2();
    enableInt();
    return 1;
}

unsigned char I2CReadRegN(unsigned char addr, unsigned char reg, unsigned char *Data, unsigned char N)
{
    unsigned char *ptr;
    
    ptr = Data;
    disableInt();
    MyStartI2C2();
    MasterWriteI2C2(addr);
    MyIdleI2C2();
    
    do
    {
        if (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2();
            MasterWriteI2C2(addr);
            MyIdleI2C2();
            if (I2C2STATbits.ACKSTAT)
            {
                continue;
            }
        }
        MasterWriteI2C2(reg);
        MyIdleI2C2();
        
        if (I2C2STATbits.ACKSTAT)
        {
            continue;
        }
        MyRestartI2C2();
        MasterWriteI2C2(addr | 1);//repeat the device address with R/W = 1 (read operation)
        MyIdleI2C2();
    }
    while (I2C2STATbits.ACKSTAT);
    
    MastergetsI2C2(N, ptr, 50000);
    MyStopI2C2(); //Send the Stop condition
    enableInt();
    return 1;
}





/*********************************************************************
 * Function:        EEAckPolling()
 *
 * Input:		Control byte.
 *
 * Output:		error state.
 *
 * Overview:		polls the bus for an Acknowledge from device
 *
 * Note:			None
 ********************************************************************/
unsigned int EEAckPolling(unsigned char control)
{
    MyIdleI2C2(); //wait for bus Idle
    MyStartI2C2(); //Generate Start condition
    
    if (I2C1STATbits.BCL)
    {
        return (-1); //Bus collision, return
    }
    else
    {
        if (MasterWriteI2C2(control))
        {
            Nop();
            Nop();
            Nop();
            //return (-3); //error return
        }
        MyIdleI2C2(); //wait for bus idle
        
        if (I2C1STATbits.BCL || I2C2STATbits.IWCOL)
        {
            return (-1); //error return
        }
        
//        while (!I2C2STATbits.ACKSTAT) {
        
        while (I2C2STATbits.ACKSTAT)
        {
            MyRestartI2C2(); //generate restart
            if (I2C1STATbits.BCL)
            {
                return (-1); //error return
            }
            
            if (MasterWriteI2C2(control)) {
                Nop();
                Nop();
                Nop();
                //return (-3);
            }
            
            MyIdleI2C2();
        }
    }
    MyStopI2C2(); //send stop condition
    
    if (I2C1STATbits.BCL)
    {
        return (-1);
    }
    return (0);
}



/*********************************************************************
 * Function:        HDPageWriteI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *wrptr.
 *
 * Output:		None.
 *
 * Overview:		Write a page of data from array pointed to be wrptr
 *				starting at address from HighAdd and LowAdd
 *
 * Note:			Address must start on a page boundary and the number
 *              of bytes to write must not overflow the page. No
 *              controls are carried on in thos function
 ********************************************************************/

static inline __attribute__((always_inline)) void HDPageWriteI2C(unsigned char ControlByte,
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *wrptr, int Length) {
    #if RAM_TYPE == MEM_24XX16
        pageWrite_24XX16(ControlByte, HighAdd, LowAdd, wrptr, Length);
#elif RAM_TYPE == MEM_24XX1025
        pageWrite_24XX1025(ControlByte, HighAdd, LowAdd, wrptr, Length);
#endif
}


/*********************************************************************
 * Function:        HDBlockReadI2C()
 *
 * Input:		ControlByte, HighAdd, LowAdd, *rdptr, length.
 *
 * Output:		None.
 *
 * Overview:		Performs a sequential read of length bytes starting at address
 *				and places data in array pointed to by *rdptr
 *
 * Note:			No check on address and length are issued
 ********************************************************************/

static inline __attribute__((always_inline)) void HDBlockReadI2C(unsigned char ControlByte,
        unsigned char HighAdd, unsigned char LowAdd, unsigned char *rdptr, unsigned int length) {
    #if RAM_TYPE == MEM_24XX16
        blockRead_24XX16(ControlByte, HighAdd, LowAdd, rdptr, length);
    #elif RAM_TYPE == MEM_24XX1025
        blockRead_24XX1025(ControlByte, HighAdd, LowAdd, rdptr, length);
#endif
}