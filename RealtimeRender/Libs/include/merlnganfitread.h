#ifndef LIX_MERL_NGAN_FIT_READ_H
#define LIX_MERL_NGAN_FIT_READ_H

//
// PBRT
//
#include "color.h"
#include "geometry.h"
#include "reflection.h"

//
// Joo-Haeng's Extension
//
#include "MerlNganFit.h"

bool MerlNganFit_load_CKT(const char *material_name, BxDF *&brdf);

bool MerlNganFit_load(const char *brdf_rep, const char *material_name, BxDF *&brdf);

void MerlNganFit_lookup(BxDF *&brdf, double theta_in, double fi_in, double theta_out, double fi_out, double& red_val,double& green_val,double& blue_val);

void MerlNganFit_lookup(BxDF *&brdf, double theta_in, double fi_in, double theta_out, double fi_out, double f[3]);

#endif