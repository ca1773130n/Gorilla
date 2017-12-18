/*
**  Designed and copyrighted by Hyungjun Park (2000/02/18)
**
*/

#ifndef __GEOMCONFIG_H__   /* this ifndef surrounds entire header */
#define __GEOMCONFIG_H__

/* Configuration */

/*
** Set MS_AFX, WIN32 by #define or #undef
** Use -D or -U options in compilation
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

#ifdef __cplusplus
} \
extern "C++" {
#endif
#include <math.h>
#ifdef __cplusplus
} \
extern "C" {
#endif


#define USE_DEBUG				0


/*
*********************************************************************
**  BEGIN FUNCTION PROTOTYPES.
*********************************************************************
*/


/*
*********************************************************************
**  END FUNCTION PROTOTYPES.
*********************************************************************
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GEOMCONFIG_H__ */
