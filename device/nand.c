#include "appdef.h"
#include "2440addr.h"
#include "2440lib.h"


#define NAND_SECTOR_SIZE_LP    2048

#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

#define CMD_WRITE1               0x80              //页写命令周期1
#define CMD_WRITE2               0x10              //页写命令周期2

#define CMD_STATUS                0x70              //读状态命令

#define CMD_READ1                 0x00              //页读命令周期1
#define CMD_READ2                 0x30              //页读命令周期2

#define CMD_RANDOMREAD1         0x05       //随意读命令周期1
#define CMD_RANDOMREAD2         0xE0       //随意读命令周期2

#define CMD_RANDOMWRITE         0x85       //随意写命令

#define CMD_ERASE1               0x60              //块擦除命令周期1
#define CMD_ERASE2               0xd0              //块擦除命令周期2

#define CMD_READID               0x90              //读ID命令

UINT8  ECCBuf[6];


//传输地址
void inline write_addr(UINT8 addr)
{
	rNFADDR8 = addr;
}

void inline write_data8(UINT8  data)
{
	rNFDATA8 = data;
}


void nand_readID(UINT8  *idbuf)
{
	nand_select_chip();                 	//打开nandflash片选
	nand_clr_RnB();                      	//清RnB信号
	write_cmd(CMD_READID);         		//读ID命令
	write_addr(0x0);                        //写0x00地址

	//读五个周期的ID
	idbuf[0] = read_data8();               	//厂商ID：0xEC
	idbuf[1] = read_data8();               	//设备ID：0xDA
	idbuf[2] = read_data8();               	//0x10
	idbuf[3] = read_data8();               	//0x95
	idbuf[4] = read_data8();              	//0x44

	nand_deselect_chip();                  	//关闭nandflash片选

	return ;
}

UINT8  __attribute__((optimize("O0"))) nand_eraseblock(UINT32  block_number)
{
	char stat, temp;
	int	i;

	temp = nand_is_badblk(block_number);     //判断该块是否为坏块
	if(temp == 0x33)
		return 0x42;           //是坏块，返回

	nand_select_chip();             //打开片选
	nand_clr_RnB();        		//清RnB信号

	write_cmd(CMD_ERASE1);         //擦除命令周期1

	//写入3个地址周期，从A18开始写起
	write_addr((block_number << 6) & 0xff);         //行地址A18~A19
	write_addr((block_number >> 2) & 0xff);         //行地址A20~A27
	write_addr((block_number >> 10) & 0xff);        //行地址A28

	write_cmd(CMD_ERASE2);         //擦除命令周期2

	for(i = 0;i < 1000;i ++);

	write_cmd(CMD_STATUS);          //读状态命令

	//判断状态值的第6位是否为1，即是否在忙，该语句的作用与NF_DETECT_RB();相同
	do{
		stat = read_data8();
	}while(!(stat&0x40));


	nand_deselect_chip();             //关闭nandflash片选

	//判断状态值的第0位是否为0，为0则擦除操作正确，否则错误
	if (stat & 0x1)
	{
		temp = nand_markbadblock(block_number);         //标注该块为坏块
		if (temp == 0x21)
			return 0x43;            //标注坏块失败
		else
			return 0x44;           //擦除操作失败
	}
	else
		return 0x66;                  //擦除操作成功
}


UINT8  __attribute__((optimize("O0"))) nand_ramdom_write(UINT32  page_number,UINT32 add,UINT8  dat)
{
	UINT8  stat;
	UINT32 	i;

	nand_select_chip();		//打开nandflash片选

	nand_clr_RnB();			//清RnB信号

	write_cmd(CMD_WRITE1);                //页写命令周期1

	//写入5个地址周期
	write_addr_lp(page_number * NAND_SECTOR_SIZE_LP);

	write_cmd(CMD_RANDOMWRITE);                 //随意写命令

	//页内地址
	write_addr((char)(add&0xff));                   //列地址A0~A7
	write_addr((char)((add>>8)&0x0f));          //列地址A8~A11

	write_data8(dat);                          //写入数据

	write_cmd(CMD_WRITE2);                //页写命令周期2

	for(i = 0;i < 1000;i ++);		//延时一段时间

	write_cmd(CMD_STATUS);                        //读状态命令

	//判断状态值的第6位是否为1，即是否在忙，该语句的作用与NF_DETECT_RB();相同
	do{
		stat =  read_data8();
	}while(!(stat&0x40));

	nand_deselect_chip();                    //关闭nandflash片选

	//判断状态值的第0位是否为0，为0则写操作正确，否则错误
	if (stat & 0x1)
		return 0x44;                  //失败
	else
		return 0x66;                  //成功
}

UINT8 nand_ramdom_read(UINT32 page_number,UINT32 add)
{
	nand_select_chip();

	nand_clr_RnB();

	write_cmd(CMD_READ1);           //页读命令周期1

	//写入5个地址周期
	write_addr_lp(page_number * NAND_SECTOR_SIZE_LP);
	
	write_cmd(CMD_READ2);          //页读命令周期2

	wait_idle();

	write_cmd(CMD_RANDOMREAD1);                 //随意读命令周期1

	//页内地址
	write_addr((char)(add&0xff));                          //列地址A0~A7
	write_addr((char)((add>>8)&0x0f));                 //列地址A8~A11
	write_cmd(CMD_RANDOMREAD2);                //随意读命令周期2

	return read_data8();               //读取数据
}

UINT8  nand_is_badblk(UINT32 block)
{
       return nand_ramdom_read(block*64, 2054);
}


UINT8  nand_markbadblock(UINT32 block)
{
	UINT8  result;

	result	= nand_ramdom_write(block*64, 2054, 0x33);

	if(result == 0x44)
		return 0x21;                  //写坏块标注失败
	else
		return 0x60;                  //写坏块标注成功
}

UINT8  __attribute__((optimize("O0"))) nand_writepage(UINT32 page_number,UINT8  *buffer,UINT32	size)
{
	UINT32	i, mecc0, secc;
	UINT8   stat, temp;

	temp = nand_is_badblk(page_number>>6);              //判断该块是否为坏块
	if(temp == 0x33)
		return 0x42;           //是坏块，返回

	nand_rst_ECC();

	nand_mecc_unlock();

	nand_select_chip();

	nand_clr_RnB();

	write_cmd(CMD_WRITE1);                //页写命令周期1

	//写入5个地址周期
	write_addr_lp(page_number * NAND_SECTOR_SIZE_LP);

	//写入数据
	for (i = 0; (i < size) && (i < 2048); i++)
	{
		write_data8(buffer[i]);
	}

	nand_mecc_lock();

	mecc0 = rNFMECC0;                    //读取main区的ECC校验码
	//把ECC校验码由字型转换为字节型，并保存到全局变量数组ECCBuf中

	ECCBuf[0] = (UINT8)(mecc0&0xff);
	ECCBuf[1] = (UINT8)((mecc0>>8) & 0xff);
	ECCBuf[2] = (UINT8)((mecc0>>16) & 0xff);
	ECCBuf[3] = (UINT8)((mecc0>>24) & 0xff);

	nand_secc_unlock();

	//把main区的ECC值写入到spare区的前4个字节地址内，即第2048~2051地址
	for(i=0;i<4;i++)
	{
		write_data8(ECCBuf[i]);
	}

	nand_secc_lock();

	secc = rNFSECC;                   //读取spare区的ECC校验码

	//把ECC校验码保存到全局变量数组ECCBuf中
	ECCBuf[4] = (UINT8)(secc&0xff);
	ECCBuf[5] = (UINT8)((secc>>8) & 0xff);

	//把spare区的ECC值继续写入到spare区的第2052~2053地址内
	for(i=4;i<6;i++)
	{
		write_data8(ECCBuf[i]);
	}

	write_cmd(CMD_WRITE2);                //页写命令周期2

	for(i = 0; i< 1000;i ++);		//延时一段时间，以等待写操作完成

	write_cmd(CMD_STATUS);                 //读状态命令

	//判断状态值的第6位是否为1，即是否在忙，该语句的作用与NF_DETECT_RB();相同
	do{
		stat = read_data8();
	}while(!(stat&0x40));

	nand_deselect_chip();

	//判断状态值的第0位是否为0，为0则写操作正确，否则错误
	if (stat & 0x1)
	{
		temp = nand_markbadblock(page_number>>6);         //标注该页所在的块为坏块
		if (temp == 0x21)
			return 0x43;            //标注坏块失败
		else
			return 0x44;           //写操作失败
	}
	else
		return 0x66;                  //写操作成功
}
