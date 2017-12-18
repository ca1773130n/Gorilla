/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   nurbs.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - nurbs.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// nurbs.cpp*
#include "girl.h"
#include "shape.h"
#include "geometry.h"
#include "dynload.h"
#include "texture.h"

namespace girl
{

// NURBS Declarations
class GIRLCOREDLL  NURBS : public Shape {
public:
	// NURBS Methods
	NURBS(const Transform &o2w, bool reverseOrientation, int nu, int uorder,
		const float *uknot, float umin, float umax,
		int nv, int vorder, const float *vknot, float vmin, float vmax,
		const float *P, bool isHomogeneous);
	~NURBS();
	virtual BBox ObjectBound() const;
	virtual BBox WorldBound() const;
	virtual bool CanIntersect() const { return false; }
	virtual void Refine(vector<boost::shared_ptr<Shape> > &refined) const;
	
	static Shape* CreateShape(const Transform &o2w, bool reverseOrientation, const ParamSet &params);
private:
	// NURBS Data
	int nu, uorder, nv, vorder;
	float umin, umax, vmin, vmax;
	float *uknot, *vknot;
	bool isHomogeneous;
	float *P;
};

}//namespace girl
