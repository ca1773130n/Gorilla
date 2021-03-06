/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   blender_musgrave.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - blender_musgrave.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// blender_musgrave.cpp*
#include "girl.h"
#include "texture.h"
#include "paramset.h"
#include "error.h"
#include "blender_texlib.h"

namespace girl {
template <class T>
// Dade - BlenderMusgraveTexture3D Declarations
class GIRLCOREDLL  BlenderMusgraveTexture3D : public Texture<T> {
public:
    // BlenderMusgraveTexture3D Public Methods

    ~BlenderMusgraveTexture3D() {
        delete mapping;
    }

    BlenderMusgraveTexture3D(
	        boost::shared_ptr<Texture<T> > c1,
			boost::shared_ptr<Texture<T> > c2,
            float mg_H,
            float mg_lacunarity,
            float mg_octaves,
            float mg_gain,
            float mg_offset,
            float noiseSize,
            float ns_outscale,
            short sType,
            short noiseBasis,
            float bright,
            float contrast,
            TextureMapping3D *map) : mapping(map) {
        tex.type = TEX_MUSGRAVE;

        tex.mg_H = mg_H;
        tex.mg_lacunarity = mg_lacunarity;
        tex.mg_octaves = mg_octaves;
        tex.mg_gain = mg_gain;
        tex.mg_offset = mg_offset;

        tex.noisesize = noiseSize;
        tex.ns_outscale = ns_outscale;
        tex.stype = sType;
        tex.noisebasis = noiseBasis;

        tex.bright = bright;
        tex.contrast = contrast;
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
    // BlenderMusgraveTexture3D Private Data

    TextureMapping3D *mapping;
	boost::shared_ptr<Texture<T> > tex1, tex2;
    blender::Tex tex;
};

template <class T> Texture<float> *BlenderMusgraveTexture3D<T>::CreateFloatTexture(
        const Transform &tex2world,
        const TextureParams &tp) {
    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	boost::shared_ptr<Texture<float> > tex1 = tp.GetFloatTexture("tex1", 1.f);
	boost::shared_ptr<Texture<float> > tex2 = tp.GetFloatTexture("tex2", 0.f);

    // Dade - decode the noise type
    short type = TEX_MFRACTAL;
    string noiseType = tp.FindString("type");
    if ((noiseType == "multifractal") || (noiseType == ""))
        type = TEX_MFRACTAL;
    else if (noiseType == "ridged_multifractal")
        type = TEX_RIDGEDMF;
    else if (noiseType == "hybrid_multifractal")
        type = TEX_HYBRIDMF;
    else if (noiseType == "hetero_terrain")
        type = TEX_HTERRAIN;
    else if (noiseType == "fbm")
        type = TEX_FBM;
    else {
        std::stringstream ss;
        ss << "Unknown noise type '" << noiseType << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

    // Dade - decode the noise basis
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

    return new BlenderMusgraveTexture3D<float>(
			tex1,
			tex2,
            tp.FindFloat("h", 1.0f),
            tp.FindFloat("lacu", 2.0f),
            tp.FindFloat("octs", 2.0f),
            tp.FindFloat("gain", 1.0f),
            tp.FindFloat("offset", 1.0f),
            tp.FindFloat("noisesize", 0.250f),
            tp.FindFloat("outscale", 1.0f),
            type,
            basis,
            tp.FindFloat("bright", 1.0f),
            tp.FindFloat("contrast", 1.0f),
            map);
}

template <class T> Texture<SWCSpectrum> *BlenderMusgraveTexture3D<T>::CreateSWCSpectrumTexture(
        const Transform &tex2world,
        const TextureParams &tp) {
    // Initialize 3D texture mapping _map_ from _tp_
    TextureMapping3D *map = new IdentityMapping3D(tex2world);
	// Apply texture specified transformation option for 3D mapping
	IdentityMapping3D *imap = (IdentityMapping3D*) map;
	imap->Apply3DTextureMappingOptions(tp);

	boost::shared_ptr<Texture<SWCSpectrum> > tex1 = tp.GetSWCSpectrumTexture("tex1", 1.f);
	boost::shared_ptr<Texture<SWCSpectrum> > tex2 = tp.GetSWCSpectrumTexture("tex2", 0.f);

    // Dade - decode the noise type
    short type = TEX_MFRACTAL;
    string noiseType = tp.FindString("type");
    if ((noiseType == "multifractal") || (noiseType == ""))
        type = TEX_MFRACTAL;
    else if (noiseType == "ridged_multifractal")
        type = TEX_RIDGEDMF;
    else if (noiseType == "hybrid_multifractal")
        type = TEX_HYBRIDMF;
    else if (noiseType == "hetero_terrain")
        type = TEX_HTERRAIN;
    else if (noiseType == "fbm")
        type = TEX_FBM;
    else {
        std::stringstream ss;
        ss << "Unknown noise type '" << noiseType << "'";
        girlError(GIRL_BADTOKEN, GIRL_ERROR, ss.str().c_str());
    }

    // Dade - decode the noise basis
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

    return new BlenderMusgraveTexture3D<SWCSpectrum>(
			tex1,
			tex2,
            tp.FindFloat("h", 1.0f),
            tp.FindFloat("lacu", 2.0f),
            tp.FindFloat("octs", 2.0f),
            tp.FindFloat("gain", 1.0f),
            tp.FindFloat("offset", 1.0f),
            tp.FindFloat("noisesize", 0.250f),
            tp.FindFloat("outscale", 1.0f),
            type,
            basis,
            tp.FindFloat("bright", 1.0f),
            tp.FindFloat("contrast", 1.0f),
            map);
}

} // namespace girl
