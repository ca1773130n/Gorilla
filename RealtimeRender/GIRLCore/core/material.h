/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   material.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - material.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_MATERIAL_H
#define GIRL_MATERIAL_H
// material.h*
#include "girl.h"

namespace girl
{

// Material Class Declarations
class GIRLCOREDLL   Material  {
public:
	// Material Interface
	virtual BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading, float u) const = 0;
	virtual ~Material();
	static void Bump(boost::shared_ptr<Texture<float> > d, const DifferentialGeometry &dgGeom,
		const DifferentialGeometry &dgShading, DifferentialGeometry *dgBump);
	void SetChild1(boost::shared_ptr<Material> x) { child1 = x; }
	void SetChild2(boost::shared_ptr<Material> x) { child2 = x; }

	void SetType(const string& matname) { mattype = matname; }
	void SetName(const string& name) { matname = name; }
	string& GetType() { return mattype; }
	boost::shared_ptr<Material> child1, child2;
	string mattype;
	string matname;
};

}//namespace girl

#endif // GIRL_MATERIAL_H
