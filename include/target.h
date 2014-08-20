#ifndef  Target_H
#define  Target_H

void TargetInit(void);
void Rtc_Init(void);
void Time0Init(void);
void TickIsrInit(void);
void Timer0Init(void);
void TickISRInit(void);
void MMU_DisableICache(void);
void MMU_DisableDCache(void);
void MMU_Init(void);



#define M5D(n)            ((n) & 0x1fffff)     //用于设置显示缓存区时，取低21位地址


#if 1
#define LCD_WIDTH    320                //屏幕的宽
#define LCD_HEIGHT  240                //屏幕的高

//垂直同步信号的脉宽、后肩和前肩
#define VSPW       (3-1)
#define VBPD              (15-1)
#define VFPD        (12-1)

//水平同步信号的脉宽、后肩和前肩
#define HSPW       (30-1)
#define HBPD       (38-1)
#define HFPD (20-1)


//for LCDCON1
#define CLKVAL_TFT          6            //设置时钟信号
#define MVAL_USED          0            //
#define PNRMODE_TFT      3            //TFT型LCD
#define BPPMODE_TFT      13           //24位TFT型LCD

//for LCDCON5
#define BPP24BL          0     //32位数据表示24位颜色值时，低位数据有效，高8位无效
#define INVVCLK        0     //像素值在VCLK下降沿有效
#define INVVLINE       1     //翻转HSYNC信号
#define INVVFRAME   1     //翻转VSYNC信号
#define INVVD            0     //正常VD信号极性
#define INVVDEN        0     //正常VDEN信号极性
#define PWREN           1     //使能PWREN信号
#define BSWP                     0     //颜色数据字节不交换
#define HWSWP           0     //颜色数据半字不交换

#else

#define LCD_WIDTH    	240		//屏幕的宽
#define LCD_HEIGHT  	320		//屏幕的高

//垂直同步信号的脉宽、后肩和前肩
#define VSPW       	0
#define VBPD            6
#define VFPD        	7

//水平同步信号的脉宽、后肩和前肩
#define HSPW       	5
#define HBPD       	0
#define HFPD 		24

//for LCDCON1
#define CLKVAL_TFT          8            //设置时钟信号
#define MVAL_USED          0            //
#define PNRMODE_TFT      3            //TFT型LCD
#define BPPMODE_TFT      13           //24位TFT型LCD

//for LCDCON5
#define BPP24BL         0     	//32位数据表示24位颜色值时，低位数据有效，高8位无效
#define INVVCLK        	1     	//像素值在VCLK下降沿有效

#define INVVLINE       	1     	//翻转HSYNC信号
#define INVVFRAME   	1     	//翻转VSYNC信号

#define INVVD           1     	//正常VD信号极性
#define INVVDEN        	1     	//正常VDEN信号极性

#define PWREN           1     	//使能PWREN信号
#define BSWP            0     	//颜色数据字节不交换
#define HWSWP           0     	//颜色数据半字不交换

#endif




//显示尺寸
#define LINEVAL  (LCD_HEIGHT-1)
#define HOZVAL   (LCD_WIDTH-1)

#define RGB(r,g,b)   		(uint32 )( (r << 16) + (g << 8) + b )
#define FROM_BCD(n)		((((n) >> 4) * 10) + ((n) & 0xf))
#define TO_BCD(n)		((((n) / 10) << 4) | ((n) % 10))


#endif
