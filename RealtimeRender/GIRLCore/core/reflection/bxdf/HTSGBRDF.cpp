/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   HTSGBRDF.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - HTSGBRDF.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////
#include "girl.h"
#include "bxdf.h"
#include "color.h"
#include "spectrum.h"
#include "fresnelterm.h"
#include "HTSGBRDF.h"

using namespace girl;

/* Solves y=x*exp(x^2) for x given y */
static inline double solve_xexpx2(double y)
{
	double x=(y>1)?sqrt(log(y)):y;
	int i;
	if (y > 3.0)
		for (i=0;i<4;i++)
			x=sqrt(log(y/x));
	else
		for (i=0;i<4;i++)
			x=0.5*(x+y*exp(-SQR(x)));
	return x;
}

static inline double Htsg_sum(double g, double T)
{
	if (g < 15) {
		double sum=0;
		int m=1;
		double term1=exp(-g);
		for (;m<40;m++) {
			register double recm=(1.0/(double)m);
			term1*=g*recm;
			sum += term1*recm*exp(T*recm);
		}
		return sum;
	}
	double mx=g;
	int i;
	for (i=0;i<4;i++)
		mx=g*exp(-1.5/mx-T/SQR(mx));
	return sqrt(g)*exp(mx*log(g)-g-(mx+1.5)*log(mx)+mx+T/mx);
}

void HTSGBRDF::f(const TsPack *tspack, const Vector &_wo, const Vector &_wi, SWCSpectrum *const f) const
{
	double theta_in, phi_in, theta_out, phi_out;

	theta_in	= acos(_wi[2]);
	phi_in		= atan2(-_wi[1], _wi[0]);
	theta_out	= acos(_wo[2]);
	phi_out		= atan2(-_wo[1], _wo[0]);

	if (theta_in < EPS) {
		theta_in = EPS;
		phi_in=0;
	}
	if (theta_out < EPS) {
		theta_out = EPS;
		phi_out=PI/2.0;
	}

	double ix=sin(theta_in)*cos(phi_in);
	double iy=sin(theta_in)*sin(phi_in);
	double iz=cos(theta_in);
	double ox=sin(theta_out)*cos(phi_out);
	double oy=sin(theta_out)*sin(phi_out);
	double oz=cos(theta_out);
	double lh=sqrt(SQR(ix+ox)+SQR(iy+oy)+SQR(iz+oz));
	double hx=(ix+ox)/lh;
	double hy=(iy+oy)/lh;
	double hz=(iz+oz)/lh;

	double c=CLAMP(ix*hx+iy*hy+iz*hz,EPS,1.0);

	double vx=ox+ix, vy=oy+iy, vz=oz+iz;
	double vxy2=SQR(vx)+SQR(vy);
	double six= -iy, siy=ix, siz=0;
	double lsi=sqrt(SQR(six)+SQR(siy)+SQR(siz));
	six/=lsi; siy/=lsi; siz/=lsi;
	double sox=oy, soy= -ox, soz=0;
	double lso=sqrt(SQR(sox)+SQR(soy)+SQR(soz));
	sox/=lso; soy/=lso; soz/=lso;
	double pix,piy,piz, pox,poy,poz;
	FindCrossProd(six,siy,siz,-ix,-iy,-iz,pix,piy,piz);
	FindCrossProd(sox,soy,soz,ox,oy,oz,pox,poy,poz);

	double Ci=(tau/sigma/2.0/tan(theta_in));
	double Co=(tau/sigma/2.0/tan(theta_out));
	double Ei=erfc(Ci);
	double Eo=erfc(Co);
	double Ki=tan(theta_in)*Ei;
	double Ko=tan(theta_out)*Eo;

	double zo_over_so_over_sqrt2=solve_xexpx2((Ki+Ko)/sqrt(PI)/4.0);
	double _sigma=sigma/sqrt(1.0+2.0*SQR(zo_over_so_over_sqrt2));
	double g=SQR(2.0*PI*_sigma*(iz+oz));
	double T = -vxy2*SQR(PI*tau);
	double D=SQR(PI*tau/2.0)*Htsg_sum(g,T);

	double S=(1.0-0.5*Ei)/(1.0+0.5*(1.0/sqrt(PI)/Ci-Ei))*
		(1.0-0.5*Eo)/(1.0+0.5*(1.0/sqrt(PI)/Co-Eo));

	double kkx, kky, kkz;
	FindCrossProd(ox,oy,oz,-ix,-iy,-iz,kkx,kky,kkz);
	double kk4=SQR(SQR(kkx)+SQR(kky)+SQR(kkz));
	double G=SQR((SQR(vx)+SQR(vy)+SQR(vz))/vz)/kk4*
		(SQR(sox*ix+soy*iy+soz*iz)+SQR(pox*ix+poy*iy+poz*iz))*
		(SQR(six*ox+siy*oy+siz*oz)+SQR(pix*ox+piy*oy+piz*oz));

	double F=Fresnel_term(c,n);

	*f += Kd * INV_PI;

	*f += (F*G*S*D*Ks/iz/oz)/PI;
}
