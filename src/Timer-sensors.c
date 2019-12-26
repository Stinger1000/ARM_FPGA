
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xiicps.h"
#include "xil_printf.h"


#define TTC_TICK_DEVICE_ID	XPAR_XTTCPS_0_DEVICE_ID
#define TTC_TICK_INTR_ID	XPAR_XTTCPS_0_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define	TICK_TIMER_FREQ_HZ	1
#define TICKS_PER_CHANGE_PERIOD	TICK_TIMER_FREQ_HZ

#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID

#define IIC_SLAVE_ADDR		0x1D
#define IIC_SCLK_RATE		100000

#define TEST_BUFFER_SIZE	2

        u8 x=0;
        u8 y=0;
        u8 z=0;
        u8 Ans=0;
XIicPs Iic;		/**< Instance of the IIC Device */

u8 SendBuffer[TEST_BUFFER_SIZE];    /**< Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];    /**< Buffer for Receiving Data */

typedef struct {
	u32 OutputHz;
	XInterval Interval;
	u8 Prescaler;
	u16 Options;
} TmrCntrSetup;


static int SetupTicker(void);
static int SetupTimer(int DeviceID);
static int SetupInterruptSystem(u16 IntcDeviceID, XScuGic *IntcInstancePtr);
static void TickHandler(void *CallBackRef);

static XTtcPs TtcPsInst;
static TmrCntrSetup SettingsTable=
	{TICK_TIMER_FREQ_HZ, 0, 0, 0};
XScuGic InterruptController;

static u32 TickCount;

int main(void)
{
	xil_printf("Starting Timer RTC Example\n\r");
	SetupInterruptSystem(INTC_DEVICE_ID, &InterruptController);
	SetupTicker();
		XIicPs_Config *Config;
	Config = XIicPs_LookupConfig(IIC_DEVICE_ID);
	XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	XIicPs_SelfTest(&Iic);
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	SendBuffer[0]=0x20; //adr
	SendBuffer[1]=0x60; //value

	XIicPs_MasterSendPolled(&Iic, SendBuffer, 2, IIC_SLAVE_ADDR);
	XIicPs_MasterSendPolled(&Iic, SendBuffer, 1, IIC_SLAVE_ADDR);
	XIicPs_MasterRecvPolled(&Iic, RecvBuffer, TEST_BUFFER_SIZE, IIC_SLAVE_ADDR);
    XIicPs_BusIsBusy(&Iic);

    SendBuffer[0]=0x21; //adr
	SendBuffer[1]=0x00; //value
	XIicPs_MasterSendPolled(&Iic, SendBuffer, 2, IIC_SLAVE_ADDR);
	XIicPs_MasterSendPolled(&Iic, SendBuffer, 1, IIC_SLAVE_ADDR);
	XIicPs_MasterRecvPolled(&Iic, RecvBuffer, TEST_BUFFER_SIZE, IIC_SLAVE_ADDR);
    XIicPs_BusIsBusy(&Iic);

        SendBuffer[0]=0x23; //adr
    	SendBuffer[1]=0x01; //value
    	XIicPs_MasterSendPolled(&Iic, SendBuffer, 2, IIC_SLAVE_ADDR);
    	XIicPs_MasterSendPolled(&Iic, SendBuffer, 1, IIC_SLAVE_ADDR);
    	XIicPs_MasterRecvPolled(&Iic, RecvBuffer, TEST_BUFFER_SIZE, IIC_SLAVE_ADDR);
        XIicPs_BusIsBusy(&Iic);

        SendBuffer[0]=0x37; //adr
    	SendBuffer[1]=0x00; //value
    	XIicPs_MasterSendPolled(&Iic, SendBuffer, 2, IIC_SLAVE_ADDR);
    	XIicPs_MasterSendPolled(&Iic, SendBuffer, 1, IIC_SLAVE_ADDR);
    	XIicPs_MasterRecvPolled(&Iic, RecvBuffer, TEST_BUFFER_SIZE, IIC_SLAVE_ADDR);
        XIicPs_BusIsBusy(&Iic);

        XIicPs_BusIsBusy(&Iic);
	while(1){};
	XTtcPs_Stop(&TtcPsInst);
	xil_printf("Successfully ran ttcps rtc Example\r\n");
}
int SetupTicker(void)
{

	TmrCntrSetup *TimerSetup;
	XTtcPs *TtcPsTick;
	TimerSetup = &SettingsTable;
	TimerSetup->Options = (XTTCPS_OPTION_INTERVAL_MODE |
					      XTTCPS_OPTION_WAVE_DISABLE);
	SetupTimer(TTC_TICK_DEVICE_ID);
	TtcPsTick = &TtcPsInst;
	XScuGic_Connect(&InterruptController, TTC_TICK_INTR_ID,
		(Xil_InterruptHandler)TickHandler, (void *)TtcPsTick);
	XScuGic_Enable(&InterruptController, TTC_TICK_INTR_ID);
	XTtcPs_EnableInterrupts(TtcPsTick, XTTCPS_IXR_INTERVAL_MASK);
	XTtcPs_Start(TtcPsTick);
	return 0;
}
int SetupTimer(int DeviceID)
{
	XTtcPs_Config *Config;
	XTtcPs *Timer;
	TmrCntrSetup *TimerSetup;
	TimerSetup = &SettingsTable;
	Timer = &TtcPsInst;
	Config = XTtcPs_LookupConfig(DeviceID);
	XTtcPs_CfgInitialize(Timer, Config, Config->BaseAddress);
	XTtcPs_SetOptions(Timer, TimerSetup->Options);
	XTtcPs_CalcIntervalFromFreq(Timer, TimerSetup->OutputHz,
		&(TimerSetup->Interval), &(TimerSetup->Prescaler));
	XTtcPs_SetInterval(Timer, TimerSetup->Interval);
	XTtcPs_SetPrescaler(Timer, TimerSetup->Prescaler);
	return 0;
}

static int SetupInterruptSystem(u16 IntcDeviceID,
				    XScuGic *IntcInstancePtr)
{
	XScuGic_Config *IntcConfig;
	IntcConfig = XScuGic_LookupConfig(IntcDeviceID);
	XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler) XScuGic_InterruptHandler,
				IntcInstancePtr);
	Xil_ExceptionEnable();
	return 0;
}

static void TickHandler(void *CallBackRef)
{
	u32 StatusEvent;
	StatusEvent = XTtcPs_GetInterruptStatus((XTtcPs *)CallBackRef);
	XTtcPs_ClearInterruptStatus((XTtcPs *)CallBackRef, StatusEvent);
	if (0 != (XTTCPS_IXR_INTERVAL_MASK & StatusEvent)) {
		TickCount++;
		if (TICKS_PER_CHANGE_PERIOD == TickCount) {
			    TickCount = 0;
			    SendBuffer[0]=0x26; //adr
                XIicPs_MasterSendPolled(&Iic, SendBuffer, 1, IIC_SLAVE_ADDR);
                XIicPs_MasterRecvPolled(&Iic, RecvBuffer, 1, IIC_SLAVE_ADDR);
                XIicPs_BusIsBusy(&Iic);
                xil_printf("temp= %i\r",((RecvBuffer[0])+25));
	            SendBuffer[0]=0x29; //adr
	            XIicPs_MasterSendPolled(&Iic, &SendBuffer[0], 1, IIC_SLAVE_ADDR);
	            XIicPs_MasterRecvPolled(&Iic, RecvBuffer, 1, IIC_SLAVE_ADDR);
                XIicPs_BusIsBusy(&Iic);
                x = RecvBuffer[0];
	            SendBuffer[0]=0x2B; //adr
	            XIicPs_MasterSendPolled(&Iic, &SendBuffer[0], 1, IIC_SLAVE_ADDR);
	            XIicPs_MasterRecvPolled(&Iic, RecvBuffer, 1, IIC_SLAVE_ADDR);
                XIicPs_BusIsBusy(&Iic);
                y = RecvBuffer[0];
	            SendBuffer[0]=0x2D; //adr
	            XIicPs_MasterSendPolled(&Iic, &SendBuffer[0], 1, IIC_SLAVE_ADDR);
                XIicPs_MasterRecvPolled(&Iic, RecvBuffer, 1, IIC_SLAVE_ADDR);
                XIicPs_BusIsBusy(&Iic);
                z = RecvBuffer[0];
                xil_printf("x=%i y=%i z=%i \r",x,y,z);

		}
	}
}
