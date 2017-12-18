/*--------------------------------------------------*/
/*
	MemUtils.h 
	Hyungjun Park, 1999/07/20
	Model viewer program.
 */
/*--------------------------------------------------*/

#ifndef __MEMUTILS_H__
#define __MEMUTILS_H__

#include "GeomConfig.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern int VxMemAlloc(int nbytes,void **ptr);
extern int VxMemFree(void **ptr);
extern int VxMemResize(int nbytes,void **ptr);
extern int **VxMemAllocA2Int(int rows,int cols);
extern double **VxMemAllocA2Dbl(int rows,int cols);
extern void **VxMemAllocA2(const int rows,const int cols,const int nByte);
extern void VxMemFreeA2Int(int **arr);
extern void VxMemFreeA2Dbl(double **arr);
extern void VxMemFreeA2(void **arr);
extern double ***VxMemAllocA3Dbl(int ind1,int ind2,int ind3);
extern void ***VxMemAllocA3(int ind1,int ind2,int ind3,int nByte);
extern void VxMemFreeA3Dbl(double ***arr);
extern void VxMemFreeA3(void ***arr);
	
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEMUTILS_H__ */
