/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   MerlBxDF.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - MerlBxDF.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "MerlBxDF.h"
#include "MerlBRDFRead.h"

using namespace girl;

#if 0
Spectrum MerlBxDF::f(const Vector &wo, const Vector &wi) const
{
 
	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	/// <Q> Why do we negate the y component?
	/// <Q> No way to remove four trigonometric functions?
	//
	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]); 

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);

	double red, green, blue;

	MerlBRDF_lookup(
		MerlBRDF_table, 
		theta_in, phi_in, 
		theta_out, phi_out, 
		red, green, blue
		);

	//if ( red < 0 || green < 0 || blue < 0 ) red = green = blue = 0.0;
	if ( red < 0 ) red = 0.0;
	if ( green < 0 ) green = 0.0;
	if ( blue < 0 ) blue = 0.0;

	float color[3] = {red, green, blue};

	return Spectrum(color);
}
#endif

void MerlBxDF::f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const
{
	double theta_in, phi_in, theta_out, phi_out;

	const double pi2 = M_PI * 2.0;

	/// <Q> Why do we negate the y component?
	/// <Q> No way to remove four trigonometric functions?
	//
	theta_in	= acos(wi[2]);

	phi_in		= atan2(-wi[1], wi[0]); 

	theta_out	= acos(wo[2]);

	phi_out		= atan2(-wo[1], wo[0]);

	double red, green, blue;

	MerlBRDF_lookup(
		MerlBRDF_table, 
		theta_in, phi_in, 
		theta_out, phi_out, 
		red, green, blue
		);

	//if ( red < 0 || green < 0 || blue < 0 ) red = green = blue = 0.0;
	if ( red < 0 ) red = 0.0;
	if ( green < 0 ) green = 0.0;
	if ( blue < 0 ) blue = 0.0;

	// apply RGB control factors.
	red*=fR;
	green*=fG;
	blue*=fB;

	float color[3] = {red, green, blue};
	RGBColor retColor(color);

	//*f += SWCSpectrum(color);
	
	*f += SWCSpectrum(tspack,retColor);
}