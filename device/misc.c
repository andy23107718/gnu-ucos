#include "ucos_ii.h"
#include "appdef.h"
#include "2440addr.h"
#include "2440lib.h"

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
