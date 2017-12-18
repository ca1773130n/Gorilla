/*
**  Designed and copyrighted by Hyungjun Park (2007/05/16)
**
*/

#ifndef __BRDF_VOL_PUB_H__   /* this ifndef surrounds entire header */
#define __BRDF_VOL_PUB_H__

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

#ifndef M_PI
#define M_PI					3.1415926535897932384626433832795
#endif

#define	V_THETA_IN_MAX		0.5*M_PI
#define	V_PHI_IN_MAX		2.0*M_PI
#define	V_THETA_OT_MAX		0.5*M_PI
#define	V_PHI_OT_MAX		2.0*M_PI

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


typedef struct VsNurbSurf_REF
{
	int		 uni_prd_flag;		/* Flag to indicate if all parameters are uniform cubic periodic */
   VsParmDat u;					/* u parameter space data */
   VsParmDat v;					/* v parameter space data */
	VsCtrlPointData cp;			/* control point data for surface */
} VsNurbSurf_REF;


typedef struct VsNurbVolume
{
	int		 uni_prd_flag;			/* Flag to indicate if all parameters are uniform cubic periodic */
   VsParmDat u;						/* u parameter space data */
   VsParmDat v;						/* v parameter space data */
	VsParmDat w;						/* w parameter space data */
   VsCtrlPointData cp;				/* control point data for volume */
} VsNurbVolume;


/*---------------------*/
/* Function Prototypes */
/*---------------------*/

extern int LoadNurbsVolume_BIN
(
char				*filename,	/* I : Filename to load */
VsNurbVolume	**ppzVol		/* O : Volume V(u,v,w) */
);

extern int VolumeDealloc
(
VsNurbVolume	**ppzVol		/* I/O : Volume V(u,v,w) */
);

extern int SurfDealloc_REF
(
VsNurbSurf_REF	**ppzSrf		/* I/O : Surface S(u,v) */
);

extern int GetIsoSrfFmVol_REF
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) representing BRDF data for a specific material */
double			theta_in,	/* I : theta for the light [0, PI/2] */
VsNurbSurf_REF	**ppzSrf		/* O : Surface S(u,v) which is the iso-surface of the volume */
);

extern int GetIsoMeshFmVol
(
VsNurbVolume	*pzVol,		/* I   : Volume V(u,v,w) representing BRDF data for a specific material */
double			theta_in,	/* I : theta for the light [0, PI/2] */
int				u_pt_num,	/* I   : Number of BRDF values in U direction (theta_out) */
int				v_pt_num,	/* I   : Number of BRDF values in V direction (phi_out) */
VsPoint			**pnts		/* I/O : Mesh of BRDF values */
);

extern int GetIsoMeshOnMapFmVol
(
VsNurbVolume	*pzVol,		/* I   : Volume V(u,v,w) representing BRDF data for a specific material */
int				rgb_flag,	/* I	 : 0 - red, 1 - green, 2 - blue, 3 - magnitude */
double			theta_in,	/* I	 : theta for the light [0, PI/2] */
int				u_pt_num,	/* I   : Number of BRDF values in U direction (theta_out) */
int				v_pt_num,	/* I   : Number of BRDF values in V direction (phi_out) */
VsPoint			**pnts		/* I/O : Mesh of BRDF values */
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

extern int GetUvwfValFmVol
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) representing UVWF data for a specific material */
double			i_theta,		/* I : theta for the light [0, PI/2] */
double			i_phi,		/* I : phi for the light [0, 2PI] */
									/*     Actually we don't use it as we consider isotropic materials */
double			o_theta,		/* I : theta for the viewer [0, PI/2] */
double			o_phi,		/* I : phi for the viewer [0, 2PI] */
double			*pdVal		/* O : UVWF value */
);

extern int VolumeEvalPnt
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) */
double			u,				/* I : U (0<=u<=1) */ 
double			v,				/* I : V (0<=v<=1) */
double			w,				/* I : W (0<=w<=1) */
VsPoint			*pzPnt		/* O : Output point */
);

extern int GetVolCtrlPnt
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) */
int				i,				/* I : Index i */
int				j,				/* I : Index j */
int				k,				/* I : Index k */
VsPoint			*pzPnt		/* O : Output control point */
);

extern int SetVolCtrlPnt
(
VsNurbVolume	*pzVol,		/* I/O : Volume V(u,v,w) */
int				i,				/* I : Index i */
int				j,				/* I : Index j */
int				k,				/* I : Index k */
VsPoint			zPnt			/* I : input control point */
);

extern int SurfaceEvalPnt_REF
(
VsNurbSurf_REF	*pzSrf,		/* I : Surface S(u,v) */
double			u,				/* I : U (0<=u<=1) */ 
double			v,				/* I : V (0<=v<=1) */
VsPoint			*pzPnt		/* O : Output point */
);

extern int GetSrfCtrlPnt_REF
(
VsNurbSurf_REF	*pzSrf,		/* I : Surface S(u,v) */
int				i,				/* I : Index i */
int				j,				/* I : Index j */
VsPoint			*pzPnt		/* O : Ouput control point */
);

extern int SetSrfCtrlPnt_REF
(
VsNurbSurf_REF	*pzSrf,		/* I : Surface S(u,v) */
int				i,				/* I : Index i */
int				j,				/* I : Index j */
VsPoint			zPnt			/* I : Input control point */
);

extern int GetGrevilleCoordFmVolCpt
(
VsNurbVolume	*pzVol,		/* I : Volume V(u,v,w) */
int				i,				/* I : Index i */
int				j,				/* I : Index j */
int				k,				/* I : Index k */
double			*u,			/* O : Greville Abscissae in U direction */
double			*v,         /* O : Greville Abscissae in V direction */
double			*w          /* O : Greville Abscissae in W direction */
);

extern int GetGrevilleCoordFmSrfCpt
(
VsNurbSurf_REF	*pzSrf,	/* I : Surface S(u,v) */
int				i,			/* I : Index i */
int				j,			/* I : Index j */
double			*u,		/* O : Greville Abscissae in U direction */
double			*v       /* O : Greville Abscissae in V direction */
);

/*--------------------*/
/* Macro Declarations */
/*--------------------*/

/* vol : Pointer to a B-spline Volume */
#define  VmBspVolCptNumU(vol)        ( (vol)->u.num_kt-(vol)->u.degree-1 )
#define  VmBspVolCptNumV(vol)        ( (vol)->v.num_kt-(vol)->v.degree-1 )
#define  VmBspVolCptNumW(vol)        ( (vol)->w.num_kt-(vol)->w.degree-1 )

#define  VmBspVolDegU(vol)           ( (vol)->u.degree )
#define  VmBspVolDegV(vol)           ( (vol)->v.degree )
#define  VmBspVolDegW(vol)           ( (vol)->w.degree )

#define  VmBspVolKnotNumU(vol)        ( (vol)->u.num_kt )
#define  VmBspVolKnotNumV(vol)        ( (vol)->v.num_kt )
#define  VmBspVolKnotNumW(vol)        ( (vol)->w.num_kt )

#define  VmBspVolKnotValU(vol, i)     ( (vol)->u.knots[(i)] )
#define  VmBspVolKnotValV(vol, j)     ( (vol)->v.knots[(j)] )
#define  VmBspVolKnotValW(vol, k)     ( (vol)->w.knots[(k)] )


/* srf : Pointer to a B-spline Surface */
#define  VmBspSrfCptNumU(srf)        ( (srf)->u.num_kt-(srf)->u.degree-1 )
#define  VmBspSrfCptNumV(srf)        ( (srf)->v.num_kt-(srf)->v.degree-1 )

#define  VmBspSrfDegU(srf)           ( (srf)->u.degree )
#define  VmBspSrfDegV(srf)           ( (srf)->v.degree )

#define  VmBspSrfKnotNumU(srf)        ( (srf)->u.num_kt )
#define  VmBspSrfKnotNumV(srf)        ( (srf)->v.num_kt )

#define  VmBspSrfKnotValU(srf, i)     ( (srf)->u.knots[(i)] )
#define  VmBspSrfKnotValV(srf, j)     ( (srf)->v.knots[(j)] )


/*--------------------------------*/
/* External Variable Declarations */
/*--------------------------------*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BRDF_VOL_PUB_H__ */
