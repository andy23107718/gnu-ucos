#include "ucos_ii.h"
#include "s3c2440s.h"
#include "app_cfg.h"
#include "2440lib.h"
#include "target.h"
#include "2440addr.h"
#include "appdef.h"
#include "printf.h"
#include "data.h"

OS_STK  MainTaskStk[MainTaskStkLengh];
OS_STK	Task0Stk [Task0StkLengh];       // Define the Task0 stack
OS_STK	Task1Stk [Task1StkLengh];       // Define the Task1 stack
OS_STK	Task2Stk [Task2StkLengh];       // Define the Task1 stack

int 	rYear, rMonth,rDay,rDayOfWeek,rHour,rMinute,rSecond;
char 	banner[]="******Welcome To UCOS******";
uint32	count=10;



int main(void);

int main()
{
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

void nand_test(void)
{
	UINT8	buff[2048];
	int	i;

	for(i = 0;i < 2048;i ++)
		buff[i]	= i & 0xff;

	nand_init_ll();

	printf("erase result is %x\r\n",nand_eraseblock(8));

	printf("write result is %x\r\n",nand_writepage(515,buff,2048));

	for(i = 0;i < 2048;i ++)
		buff[i]	= 0x0;
	printf("read result is %x\r\n",nand_read_3(515 * 2048,buff,2048));

	for(i = 0;i < 4;i ++){
		printf("%2x ",buff[i]);
	}

	nand_ramdom_write(514,2048 - 33,0xf4);
	printf("result is :%x \r\n",nand_ramdom_read(514,2048 - 33));

	nand_readID(buff);

	printf("nand ID is :");
	for(i = 0;i < 5;i ++)
		printf("%x ",buff[i]);		
	printf("\r\n");
}

void Task0(void *pdata)
{
	UINT32	pwm_freq;
#if 0
	nand_test();
#endif
	printf("%s\n",banner);

	rGPBCON = (rGPBCON & ~0x3) | 0x2;

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
	UINT32	cir_color[] = {0xffffff,0x0,0xffff00,0xff00ff,0xffff,0xff0000,0xff00};
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
