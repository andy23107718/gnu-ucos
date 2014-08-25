#include "ucos_ii.h"
#include "appdef.h"
#include "2440addr.h"
#include "2440lib.h"

unsigned char devAddr=0xa0;      //从设备AT24C02A的地址

void 	i2c_test(void)
{
	UINT8 buf[32],i;

	for(i = 0;i < 8;i ++)
		buf[i]	= i;
	wr24c02a(0x23,buf,4);
	
	for(i = 0;i < 8;i ++)
		buf[i]	= 0;
	rd24c02a(0x23,buf,4);

	printf("%d %d %d %d\n",buf[0],buf[1],buf[2],buf[3]);	
}


//AT24C02A页写，当sizeofdate为1时，是字节写
//输入参数依次为设备内存地址、IIC数据缓存数组和要写入的数据个数
void __attribute__((optimize("O0"))) wr24c02a(UINT8 wordAddr,UINT8 *buffer,UINT32 sizeofdate )
{
	int i;
	i2cflag =1;                               //应答标志

	rIICDS = devAddr;
	rIICSTAT = 0xf0;                 //主设备发送模式
	rIICCON &= ~0x10;            //清中断标志

	while(i2cflag == 1)                   //等待从设备应答，
		OSTimeDly(2);                   //一旦进入IIC中断，即可跳出该死循环

	i2cflag = 1;

	rIICDS = wordAddr;            //写入从设备内存地址
	rIICCON &= ~0x10;

	while(i2cflag)
		OSTimeDly(2);

	//连续写入数据
	for(i=0;i<sizeofdate;i++)             
	{
		i2cflag = 1;
		rIICDS = *(buffer+i);
		rIICCON &= ~0x10;
		while(i2cflag)
			OSTimeDly(2);
	}

	rIICSTAT = 0xd0;         //发出stop命令，结束该次通讯
	rIICCON = 0xe0;          //为下次IIC通讯做准备

	OSTimeDly(100);
}


//AT24C02A的序列读，当sizeofdate为1时，是随机读
//输入参数依次为设备内存地址、IIC数据缓存数组和要读取的数据个数
void rd24c02a(UINT8 wordAddr,UINT8 *buffer,UINT32 sizeofdate )
{
	int i;
	unsigned char temp;

	i2cflag =1;
	rIICDS = devAddr;                     //

	rIICCON &= ~0x10;            //清中断标志
	rIICSTAT = 0xf0;                 //主设备发送模式

	while(i2cflag)
		OSTimeDly(2);

	i2cflag = 1;

	rIICDS = wordAddr;
	rIICCON &= ~0x10;
	while(i2cflag)
		OSTimeDly(2);

	i2cflag = 1;
	rIICDS =  devAddr;            //
	rIICCON &= ~0x10;           
	rIICSTAT = 0xb0;                //主设备接收模式
	while (i2cflag)
		OSTimeDly(2);

	i2cflag = 1;
	temp = rIICDS;                    //读取从设备地址
	rIICCON &= ~0x10;
	while(i2cflag)
		OSTimeDly(2);

	//连续读
	for(i=0;i<sizeofdate;i++)
	{
		i2cflag = 1;
		if(i==sizeofdate-1)                             //如果是最后一个数据
			rIICCON &= ~0x80;                   //不再响应
		*(buffer+i) = rIICDS;
		rIICCON &= ~0x10;
		while(i2cflag)
			OSTimeDly(2);
	}

	rIICSTAT = 0x90;         //结束该次通讯
	rIICCON = 0xe0;          //

	OSTimeDly(100);
}
