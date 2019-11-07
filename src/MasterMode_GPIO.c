#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xspips.h"
#include "xgpiops.h"
#include "xil_printf.h"


#define SPI_DEVICE_ID	XPAR_XSPIPS_0_DEVICE_ID
#define GPIO_DEVICE_ID 	XPAR_XGPIOPS_0_DEVICE_ID

XSpiPs Spi;
XGpioPs Gpio;

//uint8_t data0 = 0xAA, data1 = 0xFF;
u32 Input_Pin, Output_Pin_1, Output_Pin_2;
u8 data =0xAA;
int main()
{
    init_platform();

    XGpioPs_Config *Cfg;
    Cfg = XGpioPs_LookupConfig(GPIO_DEVICE_ID) ;
    XGpioPs_CfgInitialize(&Gpio, Cfg, Cfg->BaseAddr);

    Input_Pin = 0;
    Output_Pin_1 = 53;
    Output_Pin_2 = 52;
    XGpioPs_SetDirectionPin(&Gpio, Input_Pin, 0x0);
    XGpioPs_SetDirectionPin(&Gpio, Output_Pin_1, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, Output_Pin_2, 1);
    XGpioPs_SetDirectionPin(&Gpio, Output_Pin_2, 1);
    XGpioPs_SetOutputEnablePin(&Gpio, Output_Pin_1, 1);


    XSpiPs_Config* SpiCfg;
    SpiCfg = XSpiPs_LookupConfig(SPI_DEVICE_ID);
    XSpiPs_CfgInitialize(&Spi ,SpiCfg, SpiCfg->BaseAddress);
    XSpiPs_SetOptions(&Spi, XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION);
    XSpiPs_SetClkPrescaler(&Spi , XSPIPS_CLK_PRESCALE_64);


    while(1)
       {
    	if(XGpioPs_ReadPin(&Gpio,Input_Pin)==1){
           	XGpioPs_WritePin(&Gpio, Output_Pin_1, 0x1);
           	XGpioPs_WritePin(&Gpio, Output_Pin_2, 0x0);
            XSpiPs_SetSlaveSelect(&Spi, 0x00);
           	XSpiPs_PolledTransfer(&Spi, &data, NULL, 1);
           	xil_printf("CBuffer=%d\r\n",data);
           	while (1){
           		if (XGpioPs_ReadPin(&Gpio,Input_Pin)==0){break;}
           	}
       }
    cleanup_platform();
    return 0;
}
}
