/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   BlinnPhong.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - BlinnPhong.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_BLINNPHONG_H
#define GIRL_BLINNPHONG_H
// BlinnPhong.cpp*
#include "girl.h"
#include "material.h"
#include "BlinnPhongBxDF.h"

namespace girl
{
	// BlinnPhong Class Declarations
	class GIRLCOREDLL  BlinnPhong : public Material {
	public:
		// BlinnPhong Public Methods
		BlinnPhong(boost::shared_ptr<Texture<SWCSpectrum> > kd, boost::shared_ptr<Texture<SWCSpectrum> > ks,
			boost::shared_ptr<Texture<float> > _n,
			boost::shared_ptr<Texture<float> > bump) {
				Kd = kd;
				Ks = ks;
				n = _n;
				bumpMap = bump;
		}
		BSDF *GetBSDF(const TsPack *tspack, const DifferentialGeometry &dgGeom, const DifferentialGeometry &dgShading, float u) const;

		static Material * CreateMaterial(const Transform &xform, const TextureParams &mp);
	private:
		// BlinnPhong Private Data
		boost::shared_ptr<Texture<SWCSpectrum> > Kd, Ks;
		boost::shared_ptr<Texture<float> > n;
		boost::shared_ptr<Texture<float> > bumpMap;
	};

}//namespace girl

#endif // GIRL_BLINNPHONG_H