#include "appdef.h"
#include "s3c2440s.h"

#define BUSY            1

#define NAND_SECTOR_SIZE_LP    2048

#define NAND_BLOCK_MASK_LP     (NAND_SECTOR_SIZE_LP - 1)

void nand_deselect_chip(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

	s3c2440nand->NFCONT |= (1<<1);
}

void __attribute__((optimize("O0"))) wait_idle()
{
	int i;

	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFSTAT;

	while(!(*p & BUSY))
		for(i=0; i<10; i++);
}

void __attribute__((optimize("O0"))) write_addr_lp(unsigned int addr)
{

	int i;

	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFADDR;
	int col, page;

	col = addr & NAND_BLOCK_MASK_LP;
	page = addr / NAND_SECTOR_SIZE_LP;

	*p = col & 0xff;			/* Column Address A0~A7 */
	for(i=0; i<10; i++);		
	*p = (col >> 8) & 0x0f;		/* Column Address A8~A11 */
	for(i=0; i<10; i++);
	*p = page & 0xff;			/* Row Address A12~A19 */
	for(i=0; i<10; i++);
	*p = (page >> 8) & 0xff;	/* Row Address A20~A27 */
	for(i=0; i<10; i++);
	*p = (page >> 16) & 0x03;	/* Row Address A28~A29 */
	for(i=0; i<10; i++);
}

void write_cmd(unsigned char cmd) 
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFCMD;
	*p = cmd;
}

unsigned char read_data(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
	volatile unsigned char *p = (volatile unsigned char *)&s3c2440nand->NFDATA;

	return *p;
}


void __attribute__((optimize("O0"))) nand_select_chip(void)
{
	int i;

	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;
	s3c2440nand->NFCONT &= ~(1<<1);
	for(i=0; i<10; i++);    
}


void __attribute__((optimize("O0"))) nand_read_ll(unsigned char *buf,unsigned long start_addr, int size)
{
	int i, j;

	nand_select_chip();

	for(i=start_addr; i < (start_addr + size);) {
		/* ·¢³öREAD0ÃüÁî */
		write_cmd(0);

		/* Write Address */
		write_addr_lp(i);
		write_cmd(0x30);
		wait_idle();

		for(j=0; j < NAND_SECTOR_SIZE_LP; j++, i++) {
			*buf = read_data();
			buf++;
		}
	}

	/* È¡ÏûÆ¬Ñ¡ÐÅºÅ */
	nand_deselect_chip();
}

void nand_reset(void)
{
	nand_select_chip();
	write_cmd(0xff);  // ¸´Î»ÃüÁî
	wait_idle();
	nand_deselect_chip();
}

void nand_init_ll(void)
{
	S3C2440_NAND * s3c2440nand = (S3C2440_NAND *)0x4e000000;

#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0

	/* ÉèÖÃÊ±Ðò */
	s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
	/* Ê¹ÄÜNAND Flash¿ØÖÆÆ÷, ³õÊ¼»¯ECC, ½ûÖ¹Æ¬Ñ¡ */
	s3c2440nand->NFCONT = (1<<4)|(1<<1)|(1<<0);

	/* ¸´Î»NAND Flash */
	nand_reset();
}

int copy2ram(unsigned long start_addr, unsigned char *buf, int size)
{
	nand_init_ll();

	nand_read_ll(buf,start_addr, (size + NAND_BLOCK_MASK_LP)& ~(NAND_BLOCK_MASK_LP) );
}

