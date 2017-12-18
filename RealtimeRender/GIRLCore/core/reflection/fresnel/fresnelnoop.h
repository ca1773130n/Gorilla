/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   fresnelnoop.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - fresnelnoop.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_FRESNELNOOP_H
#define GIRL_FRESNELNOOP_H
// fresnelnoop.h*
#include "girl.h"
#include "fresnel.h"

namespace girl
{

class GIRLCOREDLL   FresnelNoOp : public Fresnel {
public:
	void Evaluate(const TsPack *tspack, float, SWCSpectrum *const f) const { *f = SWCSpectrum(1.); }
};

}//namespace girl

#endif // GIRL_FRESNELNOOP_H

