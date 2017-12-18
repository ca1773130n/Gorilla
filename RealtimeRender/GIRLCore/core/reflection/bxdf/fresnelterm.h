/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelterm.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelterm.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#ifndef _FRESNELTERM_H
#define _FRESNELTERM_H

#include "mycomplex.h"
#include "util.h"

/*
fresnelterm.h
Fresnel reflection
*/

static inline double Fresnel_term(double c,  complex  n)
{
	complex g2=SQR(n)+SQR(c)-1.0;
	double a=sqrt((abs(g2)+real(g2))/2.0);
	double b=sqrt((abs(g2)-real(g2))/2.0);

	double st=(1.0-SQR(c))/c;
	return 0.5*(SQR(a-c)+SQR(b))/(SQR(a+c)+SQR(b))*
		(1.0+(SQR(a-st)+SQR(b))/(SQR(a+st)+SQR(b)));
}

#endif
