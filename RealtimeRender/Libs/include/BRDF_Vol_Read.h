/*
**  Designed and copyrighted by Hyungjun Park (2007/05/16)
**
*/

#ifndef __BRDF_VOL_READ_H__   /* this ifndef surrounds entire header */
#define __BRDF_VOL_READ_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "GeomConfig.h"
#include "MemUtils.h"

#ifndef NULL
#define NULL 		0
#endif

#ifndef V_NULL
#define V_NULL 	0
#endif

#ifndef TRUE
#define TRUE 		1
#endif

#ifndef FALSE
#define FALSE 		0
#endif

#ifndef V_TRUE
#define V_TRUE 	1
#endif

#ifndef V_FALSE
#define V_FALSE 	0
#endif

#ifndef V_PI
#define V_PI		3.14159265358979323844
#endif

#define	V_THETA_IN_MAX		0.5*V_PI
#define	V_PHI_IN_MAX		2.0*V_PI
#define	V_THETA_OT_MAX		0.5*V_PI
#define	V_PHI_OT_MAX		2.0*V_PI

#define BRDF_SAMPLING_RES_THETA_I       90
#define BRDF_SAMPLING_RES_PHI_I         360
#define BRDF_SAMPLING_RES_THETA_O       90
#define BRDF_SAMPLING_RES_PHI_O         360


/*----------------------------*/
/* Data Structure Definitions */
/*----------------------------*/

typedef struct VsPoint
{
double x;
double y;
double z;
} VsPoint;

typedef struct VsLim1
{
double min;
double max;
} VsLim1;

typedef struct VsLim2
{
VsLim1 x;
VsLim1 y;
} VsLim2;

typedef struct VsLim3
{
VsLim1 x;
VsLim1 y;
VsLim1 z;
} VsLim3;

typedef struct VsParmDat
{
int closed;         /* V_TRUE-closed curve, V_FALSE-open curve */
int degree;         /* degree (order = deg + 1) */
int num_kt;         /* number of knots */
VsLim1 bnd;         /* boundaries of parameter space */
double *knots;      /* num_kt  knot values */
} VsParmDat;

typedef struct VsCtrlPointData
{
int rat;            /* 1=RATIONAL, 0=NONRATIONAL control points  */
int dim;            /* number of coordinates per control point (1-4) */
int plane;          /* cp hyper plane type: 1:pnt, 2:line, 3:plane  */
int num_cp;         /* number of control points in list */
VsLim3 box;         /* bounding box of control points */
double *list;       /* num_cp points (possibly weighted), thus if
                       rat == RATIONAL points are of form (wx,wy,wz,w) */
} VsCtrlPointData;

typedef struct VsNurbVolume
{
VsParmDat u;          /* u parameter space data */
VsParmDat v;          /* v parameter space data */
VsParmDat w;          /* w parameter space data */
VsCtrlPointData cp;   /* control point data for volume */
} VsNurbVolume;



/*---------------------*/
/* Function Prototypes */
/*---------------------*/

extern int LoadNurbsVolume_BIN
(
char				*filename,	/* I : Filename to load */
VsNurbVolume	**ppzVol		/* O : Volume V(u,v,w) */
);

int GetGapBtVolBRDF
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) representing BRDF data for a specific material */
double			*pdBRDF,		/* I : BRDF data */
double			*pdMaxLen,	/* O : Maximum size */
double			*pdAvgLen,	/* O : Average size */
double			*pdMaxGap,	/* O : Maximum gap */
double			*pdAvgGap	/* O : Average gap */
);

extern int GetBrdfValFmVol
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) representing BRDF data for a specific material */
double			i_theta,		/* I : theta for the light [0, PI/2] */
double			i_phi,		/* I : phi for the light [0, 2PI] */
									/*     Actually we don't use it as we consider isotropic materials */
double			o_theta,		/* I : theta for the viewer [0, PI/2] */
double			o_phi,		/* I : phi for the viewer [0, 2PI] */
double			*pdRed,		/* O : Red component of BRDF value */
double			*pdGreen,	/* O : Green component of BRDF value */
double			*pdBlue		/* O : Blue component of BRDF value */
);

extern int VolumeEvalPnt
(
VsNurbVolume	*pzVol,	/* I : Volume V(u,v,w) */
double			u,			/* I : U */ 
double			v,			/* I : V */
double			w,			/* I : W */
VsPoint			*pzPnt	/* O : Output point */
);

extern int VolumeDeallocKntCpt
(
VsNurbVolume	*pzVol	/* I/O : Volume V(u,v,w) */
);

/*--------------------------------*/
/* External Variable Declarations */
/*--------------------------------*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BRDF_VOL_READ_H__ */
