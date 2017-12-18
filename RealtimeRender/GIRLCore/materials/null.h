/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   null.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - null.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// null.cpp*
#include "girl.h"
#include "material.h"

namespace girl
{

// Null Class Declarations
class GIRLCOREDLL  Null : public Material {
public:
	// Null Public Methods
	Null() {
	}
	BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;
	
	static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
private:
	// Null Private Data
};

}//namespace girl

