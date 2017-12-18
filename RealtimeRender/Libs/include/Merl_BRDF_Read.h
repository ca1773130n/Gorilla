#ifndef __MERL_BRDF_READ_H__		/* this ifndef surrounds entire header */
#define __MERL_BRDF_READ_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h> 
#include <stdlib.h>

#ifdef __cplusplus
} \
extern "C++" {
#endif
#include <math.h>
#ifdef __cplusplus
} \
extern "C" {
#endif

#define BRDF_SAMPLING_RES_THETA_H       90
#define BRDF_SAMPLING_RES_THETA_D       90
#define BRDF_SAMPLING_RES_PHI_D         360

#define RED_SCALE (1.0/1500.0)
#define GREEN_SCALE (1.15/1500.0)
#define BLUE_SCALE (1.66/1500.0)
#define M_PI	3.1415926535897932384626433832795

/*----------------------------*/
/* Data Structure Definitions */
/*----------------------------*/


/*---------------------*/
/* Function Prototypes */
/*---------------------*/

/* Merl_BRDF_Read.c */
extern void BRDF_cross_product (double* v1, double* v2, double* out);
extern void BRDF_normalize(double* v);
extern void BRDF_rotate_vector(double* vector, double* axis, double angle, double* out);

extern void BRDF_std_coords_to_half_diff_coords
(
double theta_in,		double fi_in,		double theta_out,		double fi_out,
double *theta_half,	double *fi_half,	double *theta_diff,	double *fi_diff
);

extern int BRDF_theta_half_index(double theta_half);
extern int BRDF_theta_diff_index(double theta_diff);
extern int BRDF_phi_diff_index(double phi_diff);

extern void HJP_MerlBRDF_lookup
(
double *brdf, 
double theta_in,	double fi_in,			double theta_out,		double fi_out, 
double *red_val,	double *green_val,	double *blue_val
);

extern int HJP_MerlBRDF_Read(const char *filename, double **brdf);


/*--------------------------------*/
/* External Variable Declarations */
/*--------------------------------*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MERL_BRDF_READ_H__ */
