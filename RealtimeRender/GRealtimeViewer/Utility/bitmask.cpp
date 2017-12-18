#include "stdafx.h"
#include "bitmask.h"
#include "math.h"

Bitmask::Bitmask()
{
	m_iSize = 0;
	m_pBitMask = NULL;
}

Bitmask::Bitmask(int n)
{
	Allocate(n);
}

Bitmask::~Bitmask()
{

}

void
Bitmask::Allocate(ULONG nSize)
{
	m_iSize = (int)ceil((double)nSize / (double)BITSSIZE);
	m_pBitMask = new int[m_iSize];
	AllClear();
}

void
Bitmask::Reallocate(ULONG nSize, int emptyBitSet)
{
	if( m_iSize == 0 )
	{
		Allocate(nSize);
	}
	else
	{
		m_iSize = nSize / BITSSIZE;

		//int* temp = new int[m_iSize];
		//memset(temp, emptyBitSet, m_iSize * sizeof(int));
		//memcpy(temp, m_pBitMask, m_iSize * sizeof(int));
		delete[] m_pBitMask;
		Allocate(nSize);

		//m_pBitMask = temp;
		if( emptyBitSet ) 
			AllSet();
		else 
			AllClear();
	}	
}

bool 
Bitmask::BitGet(int index)
{
	return MSBMASK & (m_pBitMask[index / BITSSIZE] >> index % (m_iSize * BITSSIZE));
}

void
Bitmask::BitSet(int index)
{
	int temp = 1 << index % BITSSIZE;

	m_pBitMask[index / BITSSIZE] |= temp;
}

void
Bitmask::BitClear(int index)
{
	int temp = 1 << index % BITSSIZE;

	m_pBitMask[index / BITSSIZE] &= ~temp;
}

bool
Bitmask::Compare(Bitmask* mask)
{
	int* pMask = mask->GetMask();

	for( int i=0; i < m_iSize; i++ )
	{
		if( m_pBitMask[i] != pMask[i] ) return false;
	}
	return true;
}

void
Bitmask::BitInvert(int index)
{
	int temp = 1 << index % BITSSIZE;

	m_pBitMask[index / BITSSIZE] ^= temp;
}

bool 
Bitmask::IsAllSet(void)
{
	for( int i=0; i < m_iSize; i++ )
	{
		if( m_pBitMask[i] != -1 ) return false;
	}
	return true;
}

int 
Bitmask::BitCount(unsigned int u)
{
	unsigned int uCount;

	uCount = u
		- ((u >> 1) & 033333333333)
		- ((u >> 2) & 011111111111);
	return
		((uCount + (uCount >> 3))
		& 030707070707) % 63;
}

int 
Bitmask::First0Bit(int i, int startIndex)
{
	i=~i;
	int temp = BitCount((i&(-i))-1);
	i = i >> startIndex;
	int temp2 = BitCount((i&(-i))-1);
	return temp2 + startIndex;
}

int 
Bitmask::First1Bit(int i, int startIndex)
{
	int temp = BitCount((i&(-i))-1);
	i = i >> startIndex;
	int temp2 = BitCount((i&(-i))-1);
	return temp2 + startIndex;
}

ULONG
Bitmask::CountSetBit(void)
{
	ULONG iCount = 0;

	for( int i=0; i < m_iSize; i++ )
		iCount += BitCount(m_pBitMask[i]);

	return iCount;
}

ULONG
Bitmask::CountZeroBit(void)
{
	ULONG iCount = 0;

	for( int i=0; i < m_iSize; i++ )
		iCount += BITSSIZE - BitCount(m_pBitMask[i]);

	return iCount;
}

ULONG
Bitmask::FindFirst0Bit(int startIndex)
{
	ULONG index = 0, count = 0, temp = 0;

	index = (startIndex / BITSSIZE);
	count += BITSSIZE * index;
	
	startIndex %= BITSSIZE;

	for( ULONG i=index; i < m_iSize; i++ )
	{
		if( (temp = First0Bit(m_pBitMask[i], startIndex)) < BITSSIZE )
			return (count + temp);
		else 
		{
			startIndex = 0;
			count += BITSSIZE;
		}
	}
	return 0;
}

ULONG 
Bitmask::FindFirst1Bit(int startIndex)
{
	ULONG index = 0, count = 0, temp = 0;

	index = (startIndex / BITSSIZE);
	count += BITSSIZE * index;

	startIndex %= BITSSIZE;

	for( ULONG i=index; i < m_iSize; i++ )
	{
		if( (temp = First1Bit(m_pBitMask[i], startIndex)) < BITSSIZE )
			return (count + temp);
		else 
		{
			startIndex = 0;
			count += BITSSIZE;
		}
	}
	return 0;
}