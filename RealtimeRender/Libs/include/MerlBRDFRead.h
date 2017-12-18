//-----------------------------------------------------------------------------
//
// JOO-HAENG LEE (JOOHAENG@ETRI.RE.KR)
//
// START: 2007-04-24
//
//-----------------------------------------------------------------------------
#ifndef MERLBRDFREAD_H
#define MERLBRDFREAD_H
// MerlBRDFRead.h*

bool MerlBRDF_load(const char *filename, double* &brdf);

void MerlBRDF_lookup(const double* brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double& red_val,double& green_val,double& blue_val);

#endif //MERLBRDFREAD_H