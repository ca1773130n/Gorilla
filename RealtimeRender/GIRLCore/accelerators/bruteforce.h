/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   bruteforce.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - bruteforce.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// bruteforce.cpp*
#include "girl.h"
#include "primitive.h"

namespace girl
{

// BruteForceAccel Declarations
class GIRLCOREDLL   BruteForceAccel : public Aggregate {
public:
	// BruteForceAccel Public Methods
	BruteForceAccel(const vector<boost::shared_ptr<Primitive> > &p);
	~BruteForceAccel();
	BBox WorldBound() const;
	bool CanIntersect() const { return true; }
	bool Intersect(const Ray &ray, Intersection *isect) const;
	bool IntersectP(const Ray &ray) const;

	void GetPrimitives(vector<boost::shared_ptr<Primitive> > &prims);

	static Aggregate *CreateAccelerator(const vector<boost::shared_ptr<Primitive> > &prims, const ParamSet &ps);

private:
	// BruteForceAccel Private Data
	vector<boost::shared_ptr<Primitive> > prims;
	BBox bounds;
};

}//namespace girl

