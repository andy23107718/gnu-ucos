#include "ucos_ii.h"
#include "s3c2440s.h"
#include "app_cfg.h"
#include "2440lib.h"
#include "target.h"
#include "2440addr.h"
#include "appdef.h"
#include "printf.h"

OS_STK  MainTaskStk[MainTaskStkLengh];
OS_STK	Task0Stk [Task0StkLengh];       // Define the Task0 stack
OS_STK	Task1Stk [Task1StkLengh];       // Define the Task1 stack
OS_STK	Task2Stk [Task2StkLengh];       // Define the Task1 stack

int main(void);

int main()
{
	count=10;
	os_banner = "Welcome To UCOS******";

	TargetInit();
	OSInit ();
	OSTimeSet(0);
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
	UINT32	pwm_freq;
#if 0
	nand_test();
#endif
	printf("%s\n",os_banner);

	//rGPBCON = (rGPBCON & ~0x3) | 0x2;

	pwm_freq = 3906;

	///Timer0 work freq 781250
	rTCNTB0 = 7812;
	rTCMPB0 = pwm_freq;

	rTCON &= ~0x1F;       
	rTCON |= 0xf;              	//死区无效，自动装载，电平反转，手动更新，定时器开启
	rTCON &= ~0x2 ;  		//手动更新位清零，PWM开始工作

	while (1)
	{
		for ( ; pwm_freq <7800; )
		{

			pwm_freq +=10;
			rTCMPB0 = pwm_freq ;          //重新赋值
			OSTimeDly(OS_TICKS_PER_SEC/4);
		}
		for( ; pwm_freq >50 ; )
		{
			pwm_freq -= 10;
			rTCMPB0 = pwm_freq;
			OSTimeDly(OS_TICKS_PER_SEC/4);
		}
		printf("Task0\r\n");
	}
}

void Task1(void *pdata)
{
	uint16	TestCnt=0;
	uint16 Version;

	UINT32	bak_color[] = {0x0,0xffffff,0xff,0xff00,0xff0000,0xffff,0xff00ff};
	//UINT32	cir_color[] = {0xffffff,0x0,0xffff00,0xff00ff,0xffff,0xff0000,0xff00};
	UINT8	i;
	Version=OSVersion();

	lcd_init();

	rLCDCON1|=1;                   //LCD开启

	paint_bmp(0,0,LCD_WIDTH,LCD_HEIGHT,bmp_data);

	while (1)
	{
		for(i = 0;i < sizeof(bak_color)/sizeof(UINT32);i ++) {
			TestCnt++;
			printf("task1\r\n");
			if(TestCnt%2)
				rGPBDAT = 0x0000;
			else
				rGPBDAT = 0x07fe;

			/*Brush_Background(bak_color[i]);
			Draw_Circular(cir_color[i]);*/
			OSTimeDly(OS_TICKS_PER_SEC);
		}
	}
}

void Task2(void *pdata)
{
	int i=0;

	/*for(i = 0;i < 3000;i ++){
		printf("%x\r\n",readADC(2));
		OSTimeDly(OS_TICKS_PER_SEC);
	}*/
	write_date(0x14,0x8,0x25,0x1,0x21,0x40,0x33);
	read_date();
	printf("year:%4d,month:%2d,day:%2d,week:%2d,hour:%2d,minute:%2d,second:%2d\r\n",
			rYear, rMonth,rDay,rDayOfWeek,rHour,rMinute,rSecond);
	while(1)
	{
		i++;
		if(i>99)i=0;

		OSTimeDly( 5 );
		OSTimeDly(OS_TICKS_PER_SEC/5);
	}
}
