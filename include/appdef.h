/*
this is the datatype used by the application
	don't use the datatype defined by the kernel
	gudujian
		2013.5.6
*/


#ifndef __DEF_H__H
#define __DEF_H__H

typedef	unsigned int	uint32;
typedef	unsigned short	uint16;
typedef	int		int32;
typedef	short	int	int16;
typedef	unsigned char	byte;
typedef	char		int8;

#define TRUE 	1   
#define FALSE 	0
#define OK	1
#define FAIL	0


#ifndef NULL
#define         NULL    0
#endif

typedef	unsigned int	UINT32;
typedef	unsigned short	UINT16;
typedef	unsigned char	UINT8;

typedef unsigned int size_t;

#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)

typedef volatile byte	S3C24X0_REG8;
typedef volatile uint16	S3C24X0_REG16;
typedef volatile uint32	S3C24X0_REG32;

/*
 * NAND Flash Controller (Page 6-1 ~ 6-8)
 * Register
   NFCONF   NAND Flash Configuration    [word, R/W, 0x00000000]
   NFCMD    NAND Flash Command Set      [word, R/W, 0x00000000]
   NFADDR   NAND Flash Address Set      [word, R/W, 0x00000000]
   NFDATA   NAND Flash Data             [word, R/W, 0x00000000]
   NFSTAT   NAND Flash Status           [word, R, 0x00000000]
   NFECC    NAND Flash ECC              [3 bytes, R, 0x00000000]
 */
/* NAND FLASH (see S3C2440 manual chapter 6) */
typedef struct {
    S3C24X0_REG32   NFCONF;
    S3C24X0_REG32   NFCONT;
    S3C24X0_REG32   NFCMD;
    S3C24X0_REG32   NFADDR;
    S3C24X0_REG32   NFDATA;
    S3C24X0_REG32   NFMECCD0;
    S3C24X0_REG32   NFMECCD1;
    S3C24X0_REG32   NFSECCD;
    S3C24X0_REG32   NFSTAT;
    S3C24X0_REG32   NFESTAT0;
    S3C24X0_REG32   NFESTAT1;
    S3C24X0_REG32   NFMECC0;
    S3C24X0_REG32   NFMECC1;
    S3C24X0_REG32   NFSECC;
    S3C24X0_REG32   NFSBLK;
    S3C24X0_REG32   NFEBLK;
} /*__attribute__((__packed__))*/ S3C2440_NAND;

#define ESC_KEY	('q')	// 0x1b
#endif /*__DEF_H__*/
