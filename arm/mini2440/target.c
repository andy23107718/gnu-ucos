/* 初始化mini2440目标板 
   1. 设定系统运行时钟频率
   2. 端口初始化
   3. MMU初始化
   4. 串口初始化
   5. LED指示灯初始化
 */
#include "2440lib.h"
#include "2440addr.h"
#include "s3c2440s.h"
#include "ucos_ii.h"
#include "target.h"

void TargetInit(void)
{
	int i;
	byte  key;
	uint32		mpll_val=0;

	i = 2 ;	//use 400M!

	switch ( i ) {
		case 0:	//200
			key = 12;
			mpll_val = (92<<12)|(4<<4)|(1);
			break;
		case 1:	//300
			key = 14;
			mpll_val = (67<<12)|(1<<4)|(1);
			break;
		case 2:	//400
			key = 14;
			mpll_val = (92<<12)|(1<<4)|(1);
			break;
		case 3:	//440!!!
			key = 14;
			mpll_val = (102<<12)|(1<<4)|(1);
			break;
		default:
			key = 14;
			mpll_val = (92<<12)|(1<<4)|(1);
			break;
	}

	//init FCLK=400M, so change MPLL first
	//	ChangeMPllValue((mpll_val>>12)&0xff, (mpll_val>>4)&0x3f, mpll_val&3);
	//	ChangeClockDivider(key, 12);    

	//MMU_EnableICache();
	//MMU_EnableDCache();

	MMU_DisableICache();
	MMU_DisableDCache();
	// Port_Init();
	MMU_Init();

	//Delay(0);
	//    Uart_Init(0,115200);
	//Uart_Select(0);
	/*若使用printf语句，将使目标代码增加很多*/
	// Uart_Printf("hello,qq2440, printf\n");
	rGPBDAT = 0x02<<5;
	//   rGPBDAT = 0x07ff;
	//  Delay(0);
	init_key();
}

void Rtc_Init(void)
{
	int wYear, wMonth,wDay,wDayOfWeek,wHour,wMinute,wSecond;
	wYear = 2008;
	wMonth = 9;
	wDay = 5;
	wDayOfWeek = 5;
	wHour= 9;
	wMinute = 41;
	wSecond = 30;
	rRTCCON = 1 ;		//RTC read and write enable
	rBCDYEAR = (unsigned char)TO_BCD(wYear%100);	//年
	rBCDMON  = (unsigned char)TO_BCD(wMonth);		//月
	rBCDDAY	 = (unsigned char)TO_BCD(wDay);			//日	
	rBCDDATE = wDayOfWeek+1;				//星期
	rBCDHOUR = (unsigned char)TO_BCD(wHour);		//小时
	rBCDMIN  = (unsigned char)TO_BCD(wMinute);		//分
	rBCDSEC  = (unsigned char)TO_BCD(wSecond);		//秒

	rRTCCON &= ~1 ;		//RTC read and write disable
}
/*********************************************************************************************************
  Initial Timer0 use for ucos time tick
 ********************************************************************************************************/
void Timer0Init(void)
{
	// 定时器设置
	rTCON &= ~(0xf<<8);					// clear manual update bit, stop Timer0

	rTCFG0 	&= 0xffffff00;					// set Timer 0&1 prescaler 0
	rTCFG0 |= 15;							//prescaler = 15+1

	rTCFG1 	&= 0xfffffff0;					// set Timer 0 MUX 1/4
	rTCFG1  |= 0x00000001;					// set Timer 0 MUX 1/4

	rTCNTB1 = (PCLK / (4 *15* OS_TICKS_PER_SEC)) - 1;

	rTCON = (rTCON & (~(0xf<<8))) |(0x02<<8);              	// updata 		
	rTCON = (rTCON & (~(0xf<<8))) |(0x09<<8); 		// star
}

/*********************************************************************************************************
  system IsrInit
 ********************************************************************************************************/
void TickISRInit(void)
{
	// 设置中断控制器
	rPRIORITY = 0x00000000;		// 使用默认的固定的优先级
	rINTMOD = 0x00000000;		// 所有中断均为IRQ中断

	pIRQ_TIMER1 = (uint32)OSTickISR;
	rINTMSK &= ~(1<<11);			// 打开TIMER1中断允许
	CLR_IF();
}


void init_key(void)
{
	rGPFCON	&= ~0x33f;
	rGPFCON	|= 0x22a;

	rINTMSK &= ~( (1<<0)|(1<<1)|(1<<2)|(1<<4));		/// enable eint
	rEINTMASK	&= ~(1<<4);				/// enable eint4

	rEXTINT0	&= ~0x70777;
	rEXTINT0	|= 0x20222;

	pIRQ_EINT0	= (uint32)key_int0;
	pIRQ_EINT1	= (uint32)key_int1;
	pIRQ_EINT2	= (uint32)key_int3;
	pIRQ_EINT4_7	= (uint32)eint4_7;
}

void key_int0(void)
{
	
}

void key_int1(void)
{
	
}

void key_int3(void)
{
	
}

void eint4_7(void)
{
	UINT32 eintpnd;
	UINT8	index;

	eintpnd	= rEINTPEND;

	for(index = 4;index < 24;index ++){
		if(eintpnd & (1<<index)){
			if(index == 4){
				//rTCON &= ~0x8;          //禁止定时器自动重载，即关闭定时器
				rGPBCON = (rGPBCON & ~0x3) | 0x1;
				rGPBDAT &= ~0x1;	
			}
			rEINTPEND |= 1<<index;
			break;
		}
	}
}
