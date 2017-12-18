/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelslick.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelslick.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FRESNELSLICK_H
#define GIRL_FRESNELSLICK_H
// fresnelslick.h*
#include "girl.h"
#include "fresnel.h"

namespace girl
{

class GIRLCOREDLL   FresnelSlick : public Fresnel {
public:
  void Evaluate(const TsPack *tspack, float cosi, SWCSpectrum *const f) const;
  FresnelSlick (float r);
private:
  float normal_incidence;
};

}//namespace girl

#endif // GIRL_FRESNELSLICK_H

