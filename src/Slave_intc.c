#include <stdio.h>
#include "platform.h"
#include "xadcps.h"
#include "xgpiops.h"
#include "xil_types.h"
#include "xspips.h"
#include "Xscugic.h"
#include "Xil_exception.h"

#define XPAR_AXI_XADC_0_DEVICE_ID 0
#define SPI_DEVICE_ID	XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INTERRUPT_ID XPS_SPI0_INT_ID

#define SpiPs_RecvByte(BaseAddress) \
		(u8)XSpiPs_In32((BaseAddress) + XSPIPS_RXD_OFFSET)

#define ledpin1 53
#define ledpin2 52
#define pbsw 0

u8 Buffer[100];
u8 ReadBuffer[100];
u8 *BufferPtr;

static XScuGic Intc; /* The Instance of the Interrupt Controller Driver */
static XGpioPs Gpio; /* The driver instance for GPIO Device. */
XGpioPs_Config *Cfg;
XSpiPs_Config* SpiCfg;
XSpiPs Spi;
static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId, XSpiPs *Spi);
static void IntrHandler(void *CallBackRef, int Bank, u32 Status);
static void SpiSlaveRead(int ByteCount);
u8 data =0xAA;

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
int Status;
XGpioPs_Config *GPIOConfigPtr;

//GPIO Initilization
GPIOConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
Status = XGpioPs_CfgInitialize(&Gpio, GPIOConfigPtr,GPIOConfigPtr->BaseAddr);
if (Status != XST_SUCCESS) {
print("GPIO INIT FAILED\n\r");
return XST_FAILURE;
}

//set direction and enable output
XGpioPs_SetDirectionPin(&Gpio, ledpin1, 1);
XGpioPs_SetOutputEnablePin(&Gpio, ledpin1, 1);
XGpioPs_SetDirectionPin(&Gpio, ledpin2, 1);
XGpioPs_SetOutputEnablePin(&Gpio, ledpin2, 1);

//set direction input pin
XGpioPs_SetDirectionPin(&Gpio, pbsw, 0x0);
Cfg = XGpioPs_LookupConfig(GPIO_DEVICE_ID) ;
XGpioPs_CfgInitialize(&Gpio, Cfg, Cfg->BaseAddr);

XSpiPs_SetOptions(&Spi,(XSPIPS_CR_CPHA_MASK) | \
		(XSPIPS_CR_CPOL_MASK));

SpiCfg = XSpiPs_LookupConfig(SPI_DEVICE_ID);
XSpiPs_CfgInitialize(&Spi ,SpiCfg, SpiCfg->BaseAddress);
XSpiPs_SetOptions(&Spi,(XSPIPS_CR_CPHA_MASK) | \
		(XSPIPS_CR_CPOL_MASK));
memset(ReadBuffer, 0x00, sizeof(ReadBuffer));
XSpiPs_SetRXWatermark((&Spi),100);
XSpiPs_Enable((&Spi));
SetupInterruptSystem(&Intc, &Gpio, GPIO_INTERRUPT_ID, &Spi);

while(1){
	for(int i=1;i<5;i++){
		int k = 1;
	}
}

cleanup_platform();
return 0;
}


static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId, XSpiPs *Spi)
{


XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */

Xil_ExceptionInit();

/*
* Initialize the interrupt controller driver so that it is ready to
* use.
*/
IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);

XScuGic_CfgInitialize(GicInstancePtr, IntcConfig,
IntcConfig->CpuBaseAddress);

/*
* Connect the interrupt controller interrupt handler to the hardware
* interrupt handling logic in the processor.
*/
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
(Xil_ExceptionHandler)XScuGic_InterruptHandler,
GicInstancePtr);

/*
* Connect the device driver handler that will be called when an
* interrupt for the device occurs, the handler defined above performs
* the specific interrupt processing for the device.
*/
XScuGic_Connect(GicInstancePtr, GpioIntrId,
(Xil_ExceptionHandler)XGpioPs_IntrHandler,
(void *)Spi);


//Enable the interrupt for the GPIO device.
XScuGic_Enable(GicInstancePtr, GpioIntrId);


// Enable interrupts in the Processor.
Xil_ExceptionEnable();
}

static void IntrHandler(void *CallBackRef, int Bank, u32 Status)
{
	SpiSlaveRead(100);
	BufferPtr = ReadBuffer;
	for (int i=0;i<100;i++){
	    xil_printf("Element=%d \r\n",BufferPtr[i]);
	    }
	XSpiPs_Disable((&Spi));
	XSpiPs_Enable((&Spi));
}
