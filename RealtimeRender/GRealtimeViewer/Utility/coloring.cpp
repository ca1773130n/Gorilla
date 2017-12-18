#include "stdafx.h"
#include "coloring.h"

VOID getColorFTag(BYTE* aRGB, int num)
{
	aRGB[0] = aRGB[1] = aRGB[2] = 0;
	
	int e = 128;
	while( num > 0 )
	{
		for( int j=0; j < 3; j++ )
		{
			aRGB[j] += e * (num & 0x1);
			num = num>>1;
			if( num == 0 ) break;
		}		
		e /= 2;
	}

}






















