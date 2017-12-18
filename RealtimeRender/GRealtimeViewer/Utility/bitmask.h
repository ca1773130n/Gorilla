#pragma once

#include "string.h"

#define BITSSIZE (sizeof(int) * 8)
#define MSBMASK 0x1

typedef unsigned long ULONG;

class Bitmask
{
public:
	Bitmask();
	Bitmask(int n);
	~Bitmask();

	void Allocate(ULONG nSize);
	void Reallocate(ULONG nSize, int emptyBitSet);

	int GetTotalBit(void) { return BITSSIZE * m_iSize; }
	int GetSize(void) { return m_iSize; }
	int* GetMask(void) { return m_pBitMask; }

	bool BitGet(int index);
	void BitSet(int index);
	void BitClear(int index);
	void BitInvert(int index);

	bool Compare(Bitmask* mask);
	bool IsAllSet(void);

	void AllClear(void) { memset(m_pBitMask, 0, sizeof(int) * m_iSize); }
	void AllSet(void) { memset(m_pBitMask, -1, sizeof(int) * m_iSize); }
	void AllInvert(void);

	int BitCount(unsigned int u);
	int First0Bit(int i, int startIndex);
	int First1Bit(int i, int startIndex);
	ULONG CountSetBit(void);
	ULONG CountZeroBit(void);

	ULONG FindFirst1Bit(int startIndex);
	ULONG FindFirst0Bit(int startIndex);

protected:
	int m_iSize;
	int* m_pBitMask;	
};