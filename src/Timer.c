
#include <stdio.h>
#include <stdlib.h>
#include "xparameters.h"
#include "xstatus.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xttcps.h"
#include "xscugic.h"
#include "xil_printf.h"


#define TTC_TICK_DEVICE_ID	XPAR_XTTCPS_0_DEVICE_ID
#define TTC_TICK_INTR_ID	XPAR_XTTCPS_0_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define	TICK_TIMER_FREQ_HZ	1
#define TICKS_PER_CHANGE_PERIOD	TICK_TIMER_FREQ_HZ


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
			xil_printf("Time:\n\r");
		}
	}
}
