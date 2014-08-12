#include "appdef.h"
#include "s3c2440s.h"
#include "2440addr.h"

#define BUSY            (1<<2)

#define NAND_SECTOR_SIZE_LP    2048

#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

void inline nand_deselect_chip(void)
{
	rNFCONT		|= 1<<1;
}

void inline wait_idle(void)
{
	while(!(rNFSTAT & BUSY));
}

void write_addr_lp(unsigned int addr)
{
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;

	rNFADDR8	= col & 0xff;                        /* Column Address A0~A7 */
	rNFADDR8	= (col >> 8) & 0x0f;		/* Column Address A8~A11 */
	rNFADDR8	= page & 0xff;			/* Row Address A12~A19 */
	rNFADDR8	= (page >> 8) & 0xff;	/* Row Address A20~A27 */
	rNFADDR8 	= (page >> 16) & 0x03;	/* Row Address A28~A29 */
}

void inline write_cmd(unsigned char cmd) 
{
	rNFCMD8		= cmd;
}

UINT8  inline read_data8(void)
{
	return rNFDATA8;
}

UINT32  inline read_data(void)
{
	return rNFDATA;
}

void inline nand_select_chip(void)
{
	rNFCONT &= ~(1<<1);
}

void inline nand_clr_RnB(void)
{
	rNFSTAT		|= (1<<2);
}

/*复位ECC*/
void inline nand_rst_ECC(void)
{
	rNFCONT |= (1<<4);
}

/*解锁main区ECC*/
void inline nand_mecc_unlock(void)
{
	rNFCONT &= ~(1<<5);
}

/*锁定main区ECC*/
void inline nand_mecc_lock(void)
{
	rNFCONT |= (1<<5);
}

/*解锁spare区ECC*/
void inline nand_secc_unlock(void)
{
	rNFCONT &= ~(1<<6);
}


/*锁定spare区的ECC值*/
void inline nand_secc_lock(void)
{
	rNFCONT |= (1<<6);
}

UINT8  __attribute__((optimize("O0"))) nand_readpage(UINT32 page_number,UINT8  *buffer,UINT32	size)
{
	int  j;
	unsigned int mecc0,secc;
	unsigned char *buf	= buffer;

	nand_rst_ECC();
	nand_mecc_unlock();
	nand_select_chip();
	nand_clr_RnB();

	write_cmd(0);
	write_addr_lp(page_number * NAND_SECTOR_SIZE_LP);
	write_cmd(0x30);

	wait_idle();

	for(j=0; (j < NAND_SECTOR_SIZE_LP) && (j < size); j+=4 ) {
		*(UINT32 *)buf = read_data();
		buf+=4;
	}
	nand_mecc_lock();
	nand_secc_unlock();

	/* 读spare区的前4个地址内容，即第2048~2051地址，这4个字节为main区的ECC
	   把读取到的main区的ECC校验码放入NFMECCD0/1的相应位置内 */
	mecc0	= rNFDATA;		
	rNFMECCD0 = ((mecc0&0xff00)<<8)|(mecc0&0xff);
	rNFMECCD1 =((mecc0&0xff000000)>>8)|((mecc0&0xff0000)>>16);

	nand_secc_lock();

	/* 继续读spare区的4个地址内容，即第2052~2055地址，其中前2个字节为spare区的ECC值
	   把读取到的spare区的ECC校验码放入NFSECCD的相应位置内 */
	secc	= rNFDATA;

	rNFSECCD=((secc&0xff00)<<8)|(secc&0xff);

	nand_deselect_chip();
	if ((rNFESTAT0&0xf) == 0x0){
		return 0x66;		///right !
	}else
		return 0x44;		///wrong !
}

UINT8   nand_read_3(unsigned long start_addr,unsigned char *buffer,UINT32 size)
{
	return nand_readpage(start_addr / NAND_SECTOR_SIZE_LP,buffer,size);
}

void __attribute__((optimize("O0"))) nand_read_ll(unsigned char *buffer,unsigned long start_addr, int size)
{
	int i, j;

	/// 这里是从nandflash启动，一般ecc校验都是错误的！
	for(j = 0,i = start_addr;i < (start_addr + size); i+= NAND_SECTOR_SIZE_LP,j += NAND_SECTOR_SIZE_LP) {
		nand_read_3(i ,buffer +j,NAND_SECTOR_SIZE_LP);
	}
}

void nand_reset(void)
{
	nand_select_chip();
	nand_clr_RnB();	
	write_cmd(0xff);  // ¸´Î»ÃüÁî
	wait_idle();
	nand_deselect_chip();
}

void nand_init_ll(void)
{
	rGPACON = (rGPACON &~(0x3f<<17)) | (0x3f<<17);            //配置芯片引脚

#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

	rNFCONF	= (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);

	/*非锁定，屏蔽nandflash中断，初始化ECC及锁定main区和spare区ECC，使能nandflash片选及控制器*/
	rNFCONT = (0<<13)|(0<<12)|(0<<10)|(0<<9)|(0<<8)|(1<<6)|(1<<5)|(1<<4)|(1<<1)|(1<<0);

	/* reset NAND Flash */
	nand_reset();
}

int copy2ram(unsigned long start_addr, unsigned char *buf, int size)
{
	nand_init_ll();

	nand_read_ll(buf,start_addr, (size + NAND_BLOCK_MASK_LP)& ~(NAND_BLOCK_MASK_LP) );

	return 0;
}

