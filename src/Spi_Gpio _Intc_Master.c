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
#define GPIO_INTERRUPT_ID XPS_GPIO_INT_ID

#define ledpin1 53
#define ledpin2 52
#define pbsw 0

static XScuGic Intc; /* The Instance of the Interrupt Controller Driver */
static XGpioPs Gpio; /* The driver instance for GPIO Device. */
XGpioPs_Config *Cfg;
XSpiPs_Config* SpiCfg;
XSpiPs Spi;
static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId, XSpiPs *Spi);
static void IntrHandler(void *CallBackRef, int Bank, u32 Status);
u8 data =0xAA;

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

SpiCfg = XSpiPs_LookupConfig(SPI_DEVICE_ID);
XSpiPs_CfgInitialize(&Spi ,SpiCfg, SpiCfg->BaseAddress);
XSpiPs_SetOptions(&Spi, XSPIPS_MASTER_OPTION);
XSpiPs_SetClkPrescaler(&Spi, XSPIPS_CLK_PRESCALE_128);
XSpiPs_SetSlaveSelect(&Spi, 0x00);
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
(void *)Gpio);

//Enable interrupts for all the pins in bank 0.
XGpioPs_SetIntrTypePin(Gpio, pbsw, XGPIOPS_IRQ_TYPE_EDGE_RISING);

//Set the handler for gpio interrupts.
XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, IntrHandler);


//Enable the GPIO interrupts of Bank 0.
XGpioPs_IntrEnablePin(Gpio, pbsw);

//Enable the interrupt for the GPIO device.
XScuGic_Enable(GicInstancePtr, GpioIntrId);

// Enable interrupts in the Processor.
Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);
}

static void IntrHandler(void *CallBackRef, int Bank, u32 Status)
{
//int udelay;
printf("****button pressed****\n\r");
XGpioPs_WritePin(&Gpio, ledpin1, 1);
XGpioPs_WritePin(&Gpio, ledpin2, 1);
XSpiPs_PolledTransfer(&Spi, &data, NULL, 1);
xil_printf("CBuffer=%x\r\n",data);
while (1){
   		if (XGpioPs_ReadPin(&Gpio,pbsw)==0){break;}}
XGpioPs_WritePin(&Gpio, ledpin1, 0);
XGpioPs_WritePin(&Gpio, ledpin2, 0);
XGpioPs_IntrClear(&Gpio, pbsw, 1);
}
