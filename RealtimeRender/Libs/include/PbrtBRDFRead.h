//-----------------------------------------------------------------------------
//
// JOO-HAENG LEE (JOOHAENG@ETRI.RE.KR)
//
// START: 2007-08-18
//
//-----------------------------------------------------------------------------
#ifndef PBRTBRDFREAD_H
#define PBRTBRDFREAD_H
// PbrtBRDFRead.h*

bool PbrtBRDF_load(const char *filename, double* &brdf);
bool PbrtPDF_load(const char *filename, double* &brdf);

void PbrtBRDF_lookup(const double* brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double &red_val, double &green_val, double &blue_val);
void PbrtPDF_lookup(const double* brdf, double theta_in, double fi_in,
			  double theta_out, double fi_out, 
			  double &pdf_val);

#endif //PBRTBRDFREAD_H