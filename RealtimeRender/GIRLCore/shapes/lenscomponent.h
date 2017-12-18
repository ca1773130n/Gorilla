/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   lenscomponent.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - lenscomponent.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "shape.h"

namespace girl
{

class GIRLCOREDLL  LensComponent : public Shape {
public:
    LensComponent(const Transform &o2w, bool ro, float rad,
        float zmin, float zmax, float phiMax, float aperture);
    // LensComponent public data
    BBox ObjectBound() const;
    bool Intersect(const Ray &ray, float *tHit,
        DifferentialGeometry *dg) const;
    bool IntersectP(const Ray &ray) const;
    float Area() const;
    Point Sample(float u1, float u2, float u3, Normal *n) const;

	static Shape* CreateShape(const Transform &o2w,
                   bool reverseOrientation,
                   const ParamSet &params);

private:
    // LensComponent Private Data
    float apRadius;
    float radius;
    float phiMax;
    float zmin, zmax;
    float thetaMin, thetaMax;
};

}//namespace girl

