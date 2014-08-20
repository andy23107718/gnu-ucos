#include "2440addr.h"
#include "target.h"
#include "appdef.h"


//定义显示缓存区

volatile U32 LCD_BUFFER[LCD_HEIGHT][LCD_WIDTH];


void	lcd_init(void) 
{ 
	//配置LCD相关引脚 
	rGPCUP  = 0x00000000; 
	rGPCCON = 0xaaaa02a9; 
	rGPDUP  = 0x00000000; 
	rGPDCON=0xaaaaaaaa; 


	rLCDCON1=(CLKVAL_TFT<<8)|(MVAL_USED<<7)|(PNRMODE_TFT<<5)|(BPPMODE_TFT<<1)|0;
	rLCDCON2=(VBPD<<24)|(LINEVAL<<14)|(VFPD<<6)|(VSPW);
	rLCDCON3=(HBPD<<19)|(HOZVAL<<8)|(HFPD);
	rLCDCON4=(HSPW);
	rLCDCON5 = (BPP24BL<<12) | (INVVCLK<<10) | (INVVLINE<<9) | (INVVFRAME<<8) | (0<<7) | (INVVDEN<<6) | (PWREN<<3)  |(BSWP<<1) | (HWSWP);

	rLCDSADDR1=(((U32)LCD_BUFFER>>22)<<21)|M5D((U32)LCD_BUFFER>>1);
	rLCDSADDR2=M5D( ((U32)LCD_BUFFER+(LCD_WIDTH*LCD_HEIGHT*4))>>1 );
	rLCDSADDR3=LCD_WIDTH*32/16;
	rLCDINTMSK|=(3);      // 屏蔽LCD中断
	rTCONSEL = 0;            //无效LPC3480

	rGPGUP=(rGPGUP&(~(1<<4)))|(1<<4);      //GPG4上拉电阻无效
	rGPGCON=(rGPGCON&(~(3<<8)))|(3<<8); //设置GPG4为LCD_PWREN
	rGPGDAT = rGPGDAT | (1<<4) ;               //GPG4置1

	rLCDCON5=(rLCDCON5&(~(1<<3)))|(1<<3);   //有效PWREN信号
	rLCDCON5=(rLCDCON5&(~(1<<5)))|(0<<5);   //PWREN信号极性不翻转
}


//绘制屏幕背景颜色，颜色为c
void Brush_Background( U32 c)
{
	int x,y ;

	for( y = 0 ; y < LCD_HEIGHT ; y++ )
	{
		for( x = 0 ; x < LCD_WIDTH ; x++ )
		{
			LCD_BUFFER[y][x] = c ;
		}
	}
}

//画实心圆，颜色为c。圆心在屏幕中心，半径为80个像素
void Draw_Circular(U32 c)
{
#define	HF_WIDTH		(LCD_WIDTH/2)	
#define	HF_HEIGHT		(LCD_HEIGHT/2)
	int x,y;
	int tempX,tempY;
	int radius = 80;
	int SquareOfR = radius*radius;

	for( y = 0 ; y < LCD_HEIGHT ; y++ )
	{
		for( x = 0 ; x < LCD_WIDTH ; x++ )
		{
			if(y <= HF_HEIGHT && x<= HF_WIDTH)
			{
				tempY=HF_HEIGHT-y;
				tempX=HF_WIDTH-x;                        
			}
			else if(y<= HF_HEIGHT && x>= HF_WIDTH)
			{
				tempY=HF_HEIGHT-y;
				tempX=x-HF_WIDTH;                        
			}
			else if(y>= HF_HEIGHT && x<= HF_WIDTH)
			{
				tempY=y-HF_HEIGHT;
				tempX=HF_WIDTH-x;                        
			}
			else
			{
				tempY = y-HF_HEIGHT;
				tempX = x-HF_WIDTH;
			}

			if ((tempY*tempY+tempX*tempX)<=SquareOfR)
				LCD_BUFFER[y][x] = c ;
		}
	}
}
