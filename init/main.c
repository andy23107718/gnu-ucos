#include "ucos_ii.h"
#include "s3c2440s.h"
#include "app_cfg.h"
#include "2440lib.h"
#include "target.h"
#include "2440addr.h"
#include "appdef.h"

OS_STK  MainTaskStk[MainTaskStkLengh];
OS_STK	Task0Stk [Task0StkLengh];       // Define the Task0 stack
OS_STK	Task1Stk [Task1StkLengh];       // Define the Task1 stack
OS_STK	Task2Stk [Task2StkLengh];       // Define the Task1 stack

int 	rYear, rMonth,rDay,rDayOfWeek,rHour,rMinute,rSecond;
char 	banner[]="******Welcome To UCOS******";
uint32	count=10;

int main(void);

void write_rec(byte data)
{
	while( (rUTRSTAT0 & 0x04) != 0x04 );

	rUTXH0 = data;  
}

void write_string(char *pdata)
{
	for( ;*pdata; pdata ++)
		write_rec((byte)(*pdata));
}

int main()
{
	TargetInit();
	OSInit ();
	OSTimeSet(0);
	write_string("main");
	OSTaskCreate (MainTask,(void *)0, &MainTaskStk[MainTaskStkLengh - 1], MainTaskPrio);
	OSStart ();

	return 0;
}

void MainTask(void *pdata) //Main Task create taks0 and task1
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
	OS_CPU_SR  cpu_sr;
#endif
	OS_ENTER_CRITICAL();
	Timer0Init();                 				//initial timer0 for ucos time tick
	TickISRInit();   					//initial interrupt prio or enable or disable
	OS_EXIT_CRITICAL();

	write_string("mainTask");
	OSTaskCreate (Task0,(void *)0, &Task0Stk[Task0StkLengh - 1], Task0Prio);
	OSTaskCreate (Task1,(void *)0, &Task1Stk[Task1StkLengh - 1], Task1Prio);
	OSTaskCreate (Task2,(void *)0, &Task2Stk[Task2StkLengh - 1], Task2Prio);
	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

void Task0(void *pdata)
{
	while (1)
	{
		write_string("Task0");
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

void Task1(void *pdata)
{
	uint16	TestCnt=0;
	uint16 Version;
	Version=OSVersion();

	while (1)
	{
		TestCnt++;
		write_string("task1");
		if(TestCnt%2)
			rGPBDAT = 0x0000;
		else
			rGPBDAT = 0x07fe;
		OSTimeDly(OS_TICKS_PER_SEC);
	}
}

void Task2(void *pdata)
{
	int i=0;

	while(1)
	{
		i++;
		if(i>99)i=0;

		if(rBCDYEAR==0x99)
			rYear = 1999;
		else
			rYear    = (2000 + rBCDYEAR);
		rMonth   = FROM_BCD(rBCDMON & 0x1f);
		rDay		= FROM_BCD(rBCDDAY & 0x03f);
		rDayOfWeek = rBCDDATE-1;
		rHour    = FROM_BCD(rBCDHOUR & 0x3f);
		rMinute     = FROM_BCD(rBCDMIN & 0x7f);
		rSecond     = FROM_BCD(rBCDSEC & 0x7f);
		OSTimeDly( 5 );
		OSTimeDly(OS_TICKS_PER_SEC/5);
	}
}
