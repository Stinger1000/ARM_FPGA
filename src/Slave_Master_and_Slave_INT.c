#include <stdio.h>
#include "platform.h"
#include "xadcps.h"
#include "xgpiops.h"
#include "xil_types.h"
#include "xspips.h"
#include "Xscugic.h"
#include "Xil_exception.h"

#define XPAR_AXI_XADC_0_DEVICE_ID 0
#define SPI1_DEVICE_ID	XPAR_XSPIPS_1_DEVICE_ID
#define SPI0_DEVICE_ID	XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INTERRUPT_ID XPS_GPIO_INT_ID
#define SPI_INT XPS_SPI0_INT_ID
#define SpiPs_RecvByte(BaseAddress) \
		(u8)XSpiPs_In32((BaseAddress) + XSPIPS_RXD_OFFSET)

#define ledpin1 53
#define ledpin2 52
#define pbsw 0

static XScuGic Intc; /* The Instance of the Interrupt Controller Driver */
static XGpioPs Gpio; /* The driver instance for GPIO Device. */
XGpioPs_Config *Cfg;
XSpiPs_Config* Spi1Cfg;
XSpiPs_Config* Spi0Cfg;
XSpiPs Spi1;
XSpiPs Spi0;
static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId, XSpiPs *Spi1,XSpiPs *Spi0);
static void IntrHandler(void *CallBackRef, int Bank, u32 Status);
static void IntrHandler1(void* CallBackRef, int Bank, u32 Status);
u8 data =0xAA;
u8 ReadBuffer[1];
u8 *BufferPtr;

void SpiSlaveRead(int ByteCount)
{
	int Count;
	u32 StatusReg;
	StatusReg = XSpiPs_ReadReg(Spi0.Config.BaseAddress,
					XSPIPS_SR_OFFSET);
	do{
		StatusReg = XSpiPs_ReadReg(Spi0.Config.BaseAddress,
					XSPIPS_SR_OFFSET);
	}while(!(StatusReg & XSPIPS_IXR_RXNEMPTY_MASK));
	for(Count = 0; Count < ByteCount; Count++){
		ReadBuffer[Count] = SpiPs_RecvByte(
				Spi0.Config.BaseAddress);
	}
}

int main()
{
init_platform();
int Status;

//GPIO Initilization
Cfg = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
Status = XGpioPs_CfgInitialize(&Gpio, Cfg,Cfg->BaseAddr);
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

Spi1Cfg = XSpiPs_LookupConfig(SPI1_DEVICE_ID);
Spi0Cfg = XSpiPs_LookupConfig(SPI0_DEVICE_ID);
XSpiPs_CfgInitialize(&Spi1 ,Spi1Cfg, Spi1Cfg->BaseAddress);
XSpiPs_CfgInitialize(&Spi0,Spi0Cfg,Spi0Cfg->BaseAddress);
XSpiPs_SetOptions(&Spi1, XSPIPS_MASTER_OPTION);
XSpiPs_SetOptions(&Spi0,XSPIPS_CR_CPHA_MASK);
XSpiPs_SetClkPrescaler(&Spi1, XSPIPS_CLK_PRESCALE_128);
XSpiPs_SetSlaveSelect(&Spi1, 0x00);
XSpiPs_SetSlaveSelect(&Spi0,0x00);
SetupInterruptSystem(&Intc, &Gpio, GPIO_INTERRUPT_ID, &Spi1,&Spi0);
memset(ReadBuffer,0x00,sizeof(ReadBuffer));
//XSpiPs_SetRXWatermark((&Spi0),1);
while(1){
};
cleanup_platform();
return 0;
}


static void SetupInterruptSystem(XScuGic *GicInstancePtr, XGpioPs *Gpio, u16 GpioIntrId, XSpiPs *Spi1,XSpiPs *Spi0)
{
XScuGic_Config *IntcConfig; /* Instance of the interrupt controller */
Xil_ExceptionInit();
IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
XScuGic_CfgInitialize(GicInstancePtr, IntcConfig,
IntcConfig->CpuBaseAddress);
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
(Xil_ExceptionHandler)XScuGic_InterruptHandler,
GicInstancePtr);
XScuGic_Connect(GicInstancePtr, GpioIntrId,
(Xil_ExceptionHandler)XGpioPs_IntrHandler,
(void *)Gpio);

XScuGic_Connect(GicInstancePtr,SPI_INT,
(XInterruptHandler)XSpiPs_InterruptHandler,
(void *)Spi0);

XGpioPs_SetIntrTypePin(Gpio, pbsw, XGPIOPS_IRQ_TYPE_EDGE_RISING);
XGpioPs_SetCallbackHandler(Gpio, (void *)Gpio, IntrHandler);
XSpiPs_SetStatusHandler(Spi0,(void *)Spi0, IntrHandler1);
XGpioPs_IntrEnablePin(Gpio, pbsw);
XScuGic_Enable(GicInstancePtr, GpioIntrId);
XScuGic_Enable(GicInstancePtr, SPI_INT);
XSpiPs_WriteReg(Spi0->Config.BaseAddress, XSPIPS_IER_OFFSET, XSPIPS_IXR_RXNEMPTY_MASK);
XSpiPs_WriteReg(Spi0->Config.BaseAddress,XSPIPS_CR_OFFSET, 0);
XSpiPs_WriteReg(Spi0->Config.BaseAddress,XSPIPS_ER_OFFSET, 1U);
Xil_ExceptionEnable();
}
static void IntrHandler(void *CallBackRef, int Bank, u32 Status)
{
XGpioPs_WritePin(&Gpio, ledpin1, 1);
XGpioPs_WritePin(&Gpio, ledpin2, 1);
xil_printf("OutByte=%x\r\n",data);
XSpiPs_PolledTransfer(&Spi1, &data, NULL, 1);
while (1){
   		if (XGpioPs_ReadPin(&Gpio,pbsw)==0){break;}}
XGpioPs_WritePin(&Gpio, ledpin1, 0);
XGpioPs_WritePin(&Gpio, ledpin2, 0);
}

static void IntrHandler1(void *CallBackRef, int Bank, u32 Status)
{
//XSpiPs_WriteReg(Spi0.Config.BaseAddress,XSPIPS_ER_OFFSET, 1U);
XSpiPs_WriteReg(Spi0.Config.BaseAddress,XSPIPS_SR_OFFSET, 1U);
ReadBuffer[0] = SpiPs_RecvByte(Spi0.Config.BaseAddress);
BufferPtr=ReadBuffer;
xil_printf("InByte=%x \r\n",BufferPtr[0]);
XSpiPs_ReadReg(Spi0.Config.BaseAddress,XSPIPS_SR_OFFSET);
XSpiPs_WriteReg(Spi0.Config.BaseAddress,XSPIPS_ER_OFFSET, 1U);
}

