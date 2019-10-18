/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xspips.h"		/* SPI device driver */


#define SPI_DEVICE_ID		XPAR_XSPIPS_0_DEVICE_ID

/**************************** Type Definitions ********************************/

/***************** Macros (Inline Functions) Definitions **********************/

/************************** Function Prototypes *******************************/

int SpiPsSelfTestExample(u16 DeviceId);

/************************** Variable Definitions ******************************/

XSpiPs Spi;			/* The instance of the SPI device */


int SpiPsSelfTestExample(u16 DeviceId)
{
	int Status;
	u8 data;
	data = 0xAA;
	XSpiPs_Config *SpiConfig;

	/*
	 * Initialize the SPI device.
	 */
	SpiConfig = XSpiPs_LookupConfig(DeviceId);
	if (NULL == SpiConfig) {
		return XST_FAILURE;
	}

	Status = XSpiPs_CfgInitialize(&Spi, SpiConfig, SpiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to check hardware build.
	 */
	Status = XSpiPs_SelfTest(&Spi);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XSpiPs_SetOptions(&Spi, XSPIPS_MASTER_OPTION |
			   XSPIPS_FORCE_SSELECT_OPTION);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		};

		XSpiPs_SetClkPrescaler(&Spi, XSPIPS_CLK_PRESCALE_64);

		XSpiPs_SetSlaveSelect(&Spi, 0x00);

		XSpiPs_Enable((&Spi));

	return XST_SUCCESS;
}


int main(void)
{
	init_platform();

	int status;
	uint8_t gg = 0xAA;

//	gg=0xAA;

	status = SpiPsSelfTestExample(SPI_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("SPI Selftest Example Failed\r\n");
		return XST_FAILURE;
	}
	while(1){
		status = XSpiPs_PolledTransfer(&Spi, &gg, NULL, 1);
		while (status != XST_SUCCESS){
			//wait

		}
		xil_printf("data sended\r\n");
	//	XSpiPs_Transfer(Spi,gg,NULL,1);
	}
	return XST_SUCCESS;
}
