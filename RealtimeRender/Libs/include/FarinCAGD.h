#include <stdio.h>
#include <malloc.h>
#include <math.h>

//
// Newly added functions
//
void CubicBspCurveFit2D(int numData, float *_data[3], float **knot, int *numCP, float ***_cp);

//
// Original FarinCAGD functions
//
void parameters(float data_x[], float data_y[], int l, float knot[]);
float area(float p1[2], float p2[2], float p3[2]);
void bessel_ends(float data[], float knot[], int l);
void c2_spline(	float knot[], int l, float data_x[],float data_y[],float bspl_x[],float bspl_y[]);
float curvature_0(float bez_x[], float bez_y[], float weight[], int degree);
float curvature_1(float bez_x[], float bez_y[], float weight[], int degree);
void differences(int degree, float coeff[], float diffs[]);
void fair_bspline(float bspl[], float knot[], int l, int from, int to);
float height(float px,float py,float ax,float ay,float bx,float by);
float hornbez(int degree, float coeff[], float t);
void l_u_system(float alpha[], float beta[], float gamma[], int l, float up[], float low[]);
void minmax(float bx[], float by[], int degree, float value[]);
void normalize_array(float data[], int size, int max_idx);
void subdiv(int degree, float coeff[], float weight[], float t, float bleft[], float bright[],
			float wleft[], float wright[]);
void ratbez_to_points(int degree, int npoints, float coeff[], float coeff_w[], float points[]);
void set_up_system(float knot[], int l, float alpha[], float beta[], float gamma[]);
void solve_system(float up[], float low[], float gamma[], int l, float rhs[], float d[]);