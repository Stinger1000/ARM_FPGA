#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xspips.h"
#include "xgpiops.h"
#include "xil_printf.h"


#define SPI_DEVICE_ID	XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID 	XPAR_XGPIOPS_0_DEVICE_ID
#define SpiPs_RecvByte(BaseAddress) \
		(u8)XSpiPs_In32((BaseAddress) + XSPIPS_RXD_OFFSET)

XSpiPs Spi;
XGpioPs Gpio;

//uint8_t data0 = 0xAA, data1 = 0xFF;
u32 Input_Pin, Output_Pin_1, Output_Pin_2;
u8 Buffer[100];
u8 ReadBuffer[100];
u8 *BufferPtr;

void SpiSlaveRead(int ByteCount)
{
	int Count;
	u32 StatusReg;

	StatusReg = XSpiPs_ReadReg(Spi.Config.BaseAddress,
					XSPIPS_SR_OFFSET);

	/*
	 * Polling the Rx Buffer for Data
	 */
	do{
		StatusReg = XSpiPs_ReadReg(Spi.Config.BaseAddress,
					XSPIPS_SR_OFFSET);
	}while(!(StatusReg & XSPIPS_IXR_RXNEMPTY_MASK));

	/*
	 * Reading the Rx Buffer
	 */
	for(Count = 0; Count < ByteCount; Count++){
		ReadBuffer[Count] = SpiPs_RecvByte(
				Spi.Config.BaseAddress);
	}

}

int main()
{
    init_platform();
    XSpiPs_Config* SpiCfg;
    SpiCfg = XSpiPs_LookupConfig(SPI_DEVICE_ID);
    XSpiPs_CfgInitialize(&Spi ,SpiCfg, SpiCfg->BaseAddress);
    XSpiPs_SetOptions(&Spi,(XSPIPS_CR_CPHA_MASK) | \
			(XSPIPS_CR_CPOL_MASK));
    while(1)
    {
    memset(ReadBuffer, 0x00, sizeof(ReadBuffer));
    XSpiPs_SetRXWatermark((&Spi),100);
    XSpiPs_Enable((&Spi));
    SpiSlaveRead(100);
    BufferPtr = ReadBuffer;
    XSpiPs_Disable((&Spi));
    for (int i=0;i<100;i++){
    xil_printf("Element=%d \r\n",BufferPtr[i]);
    }
    }
    cleanup_platform();
    return 0;
}
