/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blender_voronoi.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blender_voronoi.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "girl.h"
#include "texture.h"
#include "paramset.h"
#include "error.h"
#include "blender_texlib.h"

namespace girl {

template <class T>
class GIRLCOREDLL  BlenderVoronoiTexture3D : public Texture<T> {
public:
    // BlenderVoronoiTexture3D Public Methods

    ~BlenderVoronoiTexture3D() {
        delete mapping;
    }

    BlenderVoronoiTexture3D(
	        boost::shared_ptr<Texture<T> > c1,
			boost::shared_ptr<Texture<T> > c2,
			short colType,
			short distMetric,
			float mExp,
			float ns_outscale,
			float noiseSize,
			float nabla,
			float w1,
			float w2,
			float w3,
			float w4,
            float bright,
            float contrast,
            TextureMapping3D *map) : mapping(map) {
        tex.type = TEX_VORONOI;

		tex.vn_coltype = colType;
		tex.vn_distm = distMetric;

		tex.vn_mexp = mExp;
		tex.ns_outscale = ns_outscale;
		tex.noisesize = noiseSize;
		tex.nabla = nabla;

		tex.vn_w1 = w1;
		tex.vn_w2 = w2;
		tex.vn_w3 = w3;
		tex.vn_w4 = w4;
        tex.bright = bright;
        tex.contrast = contrast;
		tex.rfac = 1.0f;
		tex.gfac = 1.0f;
		tex.bfac = 1.0f;

		tex1 = c1;
		tex2 = c2;
    }

    T Evaluate(const TsPack *tspack, const DifferentialGeometry &dg) const {
        Vector dpdx, dpdy;
        Point P = mapping->Map(dg, &dpdx, &dpdy);

        blender::TexResult texres;
		int resultType = multitex(&tex, &P.x, &texres);

        if((resultType & TEX_RGB) && (!(resultType & TEX_INT)))
            texres.tin = (0.35 * texres.tr + 0.45 * texres.tg
                    + 0.2 * texres.tb);
		else if((resultType & TEX_INT) && (!(resultType & TEX_RGB)))
            texres.tr = texres.tg = texres.tb = texres.tin;

		T t1 = tex1->Evaluate(tspack, dg), t2 = tex2->Evaluate(tspack, dg);
		return (1.f - texres.tin) * t1 + texres.tin * t2;
    }
	void SetPower(float power, float area) {
		// Update sub-textures
		tex1->SetPower(power, area);
		tex2->SetPower(power, area);
	}
	void SetIlluminant() {
		// Update sub-textures
		tex1->SetIlluminant();
		tex2->SetIlluminant();
	}
    static Texture<float> *CreateFloatTexture(const Transform &tex2world, const TextureParams &tp);
	static Texture<SWCSpectrum> *CreateSWCSpectrumTexture(const Transform &tex2world, const TextureParams &tp);
private:
    // BlenderVoronoiTexture3D Private Data

    TextureMapping3D *mapping;
	boost::shared_ptr<Texture<T> > tex1, tex2;
    blender::Tex tex;
};

template <class T> Texture<float> *BlenderVoronoiTexture3D<T>::CreateFloatTexture(
        const Transform &tex2world,
        const TextureParams &tp) {
    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	boost::shared_ptr<Texture<float> > tex1 = tp.GetFloatTexture("tex1", 1.f);
	boost::shared_ptr<Texture<float> > tex2 = tp.GetFloatTexture("tex2", 0.f);

	// Dade - decode the distance metric
    short distType = TEX_DISTANCE;
    string sDistType = tp.FindString("distmetric");
    if ((sDistType == "actual_distance") || (sDistType == ""))
        distType = TEX_DISTANCE;
	else if (sDistType == "distance_squared")
        distType = TEX_DISTANCE_SQUARED;
	else if (sDistType == "manhattan")
        distType = TEX_MANHATTAN;
	else if (sDistType == "chebychev")
        distType = TEX_CHEBYCHEV;
	else if (sDistType == "minkovsky_half")
        distType = TEX_MINKOVSKY_HALF;
	else if (sDistType == "minkovsky_four")
        distType = TEX_MINKOVSKY_FOUR;
	else if (sDistType == "minkovsky")
        distType = TEX_MINKOVSKY;
    else {
        std::stringstream ss;
        ss << "Unknown distance metric '" << sDistType << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

    return new BlenderVoronoiTexture3D<float>(
			tex1,
			tex2,
			tp.FindInt("coltype", 0),
			distType,
			tp.FindFloat("minkovsky_exp", 2.5f),
			tp.FindFloat("outscale", 1.0f),
			tp.FindFloat("noisesize", 0.25f),
			tp.FindFloat("nabla", 0.025f),
			tp.FindFloat("w1", 1.0f),
			tp.FindFloat("w2", 0.0f),
			tp.FindFloat("w3", 0.0f),
			tp.FindFloat("w4", 0.0f),
            tp.FindFloat("bright", 1.0f),
            tp.FindFloat("contrast", 1.0f),
            map);
}

template <class T> Texture<SWCSpectrum> *BlenderVoronoiTexture3D<T>::CreateSWCSpectrumTexture(
        const Transform &tex2world,
        const TextureParams &tp) {
    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	boost::shared_ptr<Texture<SWCSpectrum> > tex1 = tp.GetSWCSpectrumTexture("tex1", 1.f);
	boost::shared_ptr<Texture<SWCSpectrum> > tex2 = tp.GetSWCSpectrumTexture("tex2", 0.f);

    // Dade - decode the distance metric
    short distType = TEX_DISTANCE;
    string sDistType = tp.FindString("distmetric");
    if ((sDistType == "actual_distance") || (sDistType == ""))
        distType = TEX_DISTANCE;
	else if (sDistType == "distance_squared")
        distType = TEX_DISTANCE_SQUARED;
	else if (sDistType == "manhattan")
        distType = TEX_MANHATTAN;
	else if (sDistType == "chebychev")
        distType = TEX_CHEBYCHEV;
	else if (sDistType == "minkovsky_half")
        distType = TEX_MINKOVSKY_HALF;
	else if (sDistType == "minkovsky_four")
        distType = TEX_MINKOVSKY_FOUR;
	else if (sDistType == "minkovsky")
        distType = TEX_MINKOVSKY;
    else {
        std::stringstream ss;
        ss << "Unknown distance metric '" << sDistType << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

    return new BlenderVoronoiTexture3D<SWCSpectrum>(
			tex1,
			tex2,
			tp.FindInt("coltype", 0),
			distType,
			tp.FindFloat("minkovsky_exp", 2.5f),
			tp.FindFloat("outscale", 1.0f),
			tp.FindFloat("noisesize", 0.25f),
			tp.FindFloat("nabla", 0.025f),
			tp.FindFloat("w1", 1.0f),
			tp.FindFloat("w2", 0.0f),
			tp.FindFloat("w3", 0.0f),
			tp.FindFloat("w4", 0.0f),
            tp.FindFloat("bright", 1.0f),
            tp.FindFloat("contrast", 1.0f),
            map);
}

} // namespace girl
