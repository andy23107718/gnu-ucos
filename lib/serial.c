#include "2440addr.h"
#include "appdef.h"

void	putc(byte data)
{
	while( (rUTRSTAT0 & 0x04) != 0x04 );
        rUTXH0 = data;  
}

byte	getc(void)
{

	return 't';
}
