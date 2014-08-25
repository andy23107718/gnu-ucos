#include "ucos_ii.h"
#include "appdef.h"
#include "2440addr.h"
#include "2440lib.h"
#include "target.h"

int readADC(INT8U channel)
{
	channel	&= 7;
	rADCCON = (1<<14)|(49<<6)|(channel<<3);              //设置预分频器和A/D通道

	rADCCON|=0x1;                                      //启动A/D转换

	while(rADCCON & 0x1)
		OSTimeDly(1);                                                //确认A/D转换是否开始

	while(!(rADCCON & 0x8000))
		OSTimeDly(1);                                                //等待A/D转换的结束

	return ( (int)rADCDAT0 & 0x3ff );      //读取A/D转换的数据
}


void	read_date(void)
{
	rRTCCON = 1 ;
	if(rBCDYEAR==0x99)
		rYear = 1999;
	else
		rYear    = (2000 + FROM_BCD(rBCDYEAR));
	rMonth  = FROM_BCD(rBCDMON & 0x1f);
	rDayOfWeek = FROM_BCD(rBCDDAY & 0x0f);
	rDay	= FROM_BCD(rBCDDATE&0x3f);
	rHour   = FROM_BCD(rBCDHOUR & 0x3f);
	rMinute     = FROM_BCD(rBCDMIN & 0x7f);
	rSecond     = FROM_BCD(rBCDSEC & 0x7f);
	rRTCCON = 0 ;
}

//参数需要是BCD形式的
void write_date(UINT8 year,UINT8 month,UINT8 day,UINT8 dayofweek,UINT8 hour,UINT8 minute,UINT8 second)
{
	rRTCCON = 1;
	rBCDYEAR = year;            //年
	rBCDMON  = month ;           //月
	rBCDDAY  = dayofweek;            //日
	rBCDDATE = day; 		//星期
	rBCDHOUR = hour;            //小时
	rBCDMIN  = minute;                   //分
	rBCDSEC  = second;            //秒
	rRTCCON = 0;
}
