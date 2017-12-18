/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   BlinnPhongBxDF.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - BlinnPhongBxDF.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_BLINNPHONGBXDF_H
#define GIRL_BLINNPHONGBXDF_H
// cooktorrance.h*
#include "girl.h"
#include "bxdf.h"
#include "spectrum.h"

namespace girl
{
	class GIRLCOREDLL PhongBxDF : public BxDF {
	public:
		// BlinnPhongBxDF Public Methods
		PhongBxDF(float nS = 1.f);

		void f(const TsPack *tspack, const Vector &wo, const Vector &wi, SWCSpectrum *const f) const;

	private:
		// Blinn Phong BxDF Private Data
		SWCSpectrum Kd;
		SWCSpectrum Ks;
		float nShiness;
	};

}//namespace girl

#endif // GIRL_BLINNPHONGBXDF_H

