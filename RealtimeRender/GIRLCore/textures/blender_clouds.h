/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blender_clouds.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blender_clouds.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blender_clouds.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"
#include "error.h"
#include "blender_texlib.h"

namespace girl {

// SMISRA - BlenderCloudsTexture3D Declarations
template <class T>
class GIRLCOREDLL  BlenderCloudsTexture3D : public Texture<T> {
public:
    // BlenderCloudsTexture3D Public Methods

    ~BlenderCloudsTexture3D() {
        delete mapping;
    }

    BlenderCloudsTexture3D(
	        boost::shared_ptr<Texture<T> > c1,
			boost::shared_ptr<Texture<T> > c2,
            float noiseSize,
            short noiseType,
			short noiseDepth,
            short sType,
            short noiseBasis,
            float bright,
            float contrast,
            TextureMapping3D *map) : mapping(map) {
        tex.type = TEX_CLOUDS;

        tex.noisesize = noiseSize;
        tex.noisetype = noiseType;
		tex.noisedepth = noiseDepth;
        tex.stype = sType;
        tex.noisebasis = noiseBasis;
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

        if(resultType & TEX_RGB)
            texres.tin = (0.35 * texres.tr + 0.45 * texres.tg
                    + 0.2 * texres.tb);
        else
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
    // BlenderCloudsTexture3D Private Data

    TextureMapping3D *mapping;
	boost::shared_ptr<Texture<T> > tex1, tex2;
    blender::Tex tex;
};

template <class T> Texture<float> *BlenderCloudsTexture3D<T>::CreateFloatTexture(
        const Transform &tex2world,
        const TextureParams &tp) {
    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	boost::shared_ptr<Texture<float> > tex1 = tp.GetFloatTexture("tex1", 1.f);
	boost::shared_ptr<Texture<float> > tex2 = tp.GetFloatTexture("tex2", 0.f);

	// Decode the noise type
    short type = TEX_DEFAULT;
    string stype = tp.FindString("type");
    if ((stype == "default") || (stype == ""))
        type = TEX_DEFAULT;
    else if (stype == "color")
        type = TEX_COLOR;
    else {
        std::stringstream ss;
        ss << "Unknown noise color type '" << stype << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

	// Decode the noise type
    short ntype = TEX_NOISEPERL;
    string noiseType = tp.FindString("noisetype");
    if ((noiseType == "soft_noise") || (noiseType == ""))
        ntype = TEX_NOISESOFT;
    else if (noiseType == "hard_noise")
        ntype = TEX_NOISEPERL;
    else {
        std::stringstream ss;
        ss << "Unknown noise type '" << noiseType << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

	// Decode the noise basis
    short basis = TEX_BLENDER;
    string noiseBasis = tp.FindString("noisebasis");
    if ((noiseBasis == "blender_original") || (noiseBasis == ""))
        basis = TEX_BLENDER;
    else if (noiseBasis == "original_perlin")
        basis = TEX_STDPERLIN;
    else if (noiseBasis == "improved_perlin")
        basis = TEX_NEWPERLIN;
    else if (noiseBasis == "voronoi_f1")
        basis = TEX_VORONOI_F1;
    else if (noiseBasis == "voronoi_f2")
        basis = TEX_VORONOI_F2;
    else if (noiseBasis == "voronoi_f3")
        basis = TEX_VORONOI_F3;
    else if (noiseBasis == "voronoi_f4")
        basis = TEX_VORONOI_F4;
    else if (noiseBasis == "voronoi_f2f1")
        basis = TEX_VORONOI_F2F1;
    else if (noiseBasis == "voronoi_crackle")
        basis = TEX_VORONOI_CRACKLE;
    else if (noiseBasis == "cell_noise")
        basis = TEX_CELLNOISE;
    else {
        std::stringstream ss;
        ss << "Unknown noise basis '" << noiseBasis << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

    return new BlenderCloudsTexture3D<float>(
			tex1,
			tex2,
            tp.FindFloat("noisesize", 0.250f),
			ntype,
			(short)tp.FindInt("noisedepth", 2),
			type,
			basis,
            tp.FindFloat("bright", 1.0f),
            tp.FindFloat("contrast", 1.0f),
            map);
}

template <class T> Texture<SWCSpectrum> *BlenderCloudsTexture3D<T>::CreateSWCSpectrumTexture(
        const Transform &tex2world,
        const TextureParams &tp) {
    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	boost::shared_ptr<Texture<SWCSpectrum> > tex1 = tp.GetSWCSpectrumTexture("tex1", 1.f);
	boost::shared_ptr<Texture<SWCSpectrum> > tex2 = tp.GetSWCSpectrumTexture("tex2", 0.f);

	// Decode the noise type
    short type = TEX_DEFAULT;
    string stype = tp.FindString("type");
    if ((stype == "default") || (stype == ""))
        type = TEX_DEFAULT;
    else if (stype == "color")
        type = TEX_COLOR;
    else {
        std::stringstream ss;
        ss << "Unknown noise color type '" << stype << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

	// Decode the noise type
    short ntype = TEX_NOISEPERL;
    string noiseType = tp.FindString("noisetype");
    if ((noiseType == "soft_noise") || (noiseType == ""))
        ntype = TEX_NOISESOFT;
    else if (noiseType == "hard_noise")
        ntype = TEX_NOISEPERL;
    else {
        std::stringstream ss;
        ss << "Unknown noise type '" << noiseType << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

	// Decode the noise basis
    short basis = TEX_BLENDER;
    string noiseBasis = tp.FindString("noisebasis");
    if ((noiseBasis == "blender_original") || (noiseBasis == ""))
        basis = TEX_BLENDER;
    else if (noiseBasis == "original_perlin")
        basis = TEX_STDPERLIN;
    else if (noiseBasis == "improved_perlin")
        basis = TEX_NEWPERLIN;
    else if (noiseBasis == "voronoi_f1")
        basis = TEX_VORONOI_F1;
    else if (noiseBasis == "voronoi_f2")
        basis = TEX_VORONOI_F2;
    else if (noiseBasis == "voronoi_f3")
        basis = TEX_VORONOI_F3;
    else if (noiseBasis == "voronoi_f4")
        basis = TEX_VORONOI_F4;
    else if (noiseBasis == "voronoi_f2f1")
        basis = TEX_VORONOI_F2F1;
    else if (noiseBasis == "voronoi_crackle")
        basis = TEX_VORONOI_CRACKLE;
    else if (noiseBasis == "cell_noise")
        basis = TEX_CELLNOISE;
    else {
        std::stringstream ss;
        ss << "Unknown noise basis '" << noiseBasis << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

    return new BlenderCloudsTexture3D<SWCSpectrum>(
			tex1,
			tex2,
            tp.FindFloat("noisesize", 0.250f),
			ntype,
			(short)tp.FindInt("noisedepth", 2),
			type,
			basis,
            tp.FindFloat("bright", 1.0f),
            tp.FindFloat("contrast", 1.0f),
            map);
}

} // namespace girl

