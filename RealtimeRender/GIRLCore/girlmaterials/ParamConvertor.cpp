/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   ParamConvertor.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   ParamConvertor.cpp
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-05-11  :   First version - ParamConvertor.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2009 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "ParamConvertor.h"
#include "GirlTextureParams.h"

using namespace girl;

ParamSet ParamConvertor::makeParamSet(string shaderName, const TextureParams &mp)
{
	ParamSet	tParam;

	if (shaderName == "GirlLambert") {
		parseLambertParam(mp, tParam);
	} else if (shaderName == "GirlBlinn") {
		parseBlinnParam(mp, tParam);
	} else if (shaderName == "GirlPhong") {
		parseReflectParam(mp, tParam);
	} else if (shaderName == "GirlPhongE") {
		parsePhongEParam(mp, tParam);
	}	

	return tParam;
}

TP_GirlMaterial ParamConvertor::makeGirlParamSet(string shaderName, const TextureParams &mp)
{
	TP_GirlMaterial	tParam;

	if (shaderName == "GirlLambert") {
		parseLambertParam(mp, tParam);
	} else if (shaderName == "GirlBlinn") {
		parseBlinnParam(mp, tParam);
	} else if (shaderName == "GirlPhong") {
		parsePhongParam(mp, tParam);
	} else if (shaderName == "GirlPhongE") {
		parsePhongEParam(mp, tParam);
	}	

	return tParam;
}

int ParamConvertor::getEnumOpacityMode(string strVal)
{
	if (strVal == "Opacity Gain")
		return 0;
	else if (strVal == "Solid Matte")
		return 1;
	else if (strVal == "Black Hole")
		return 2;
	else
		return 0;
}

void ParamConvertor::parseLambertParam(const TextureParams &mp, TP_GirlMaterial& tpMaterial)
{
	tpMaterial.color				= mp.GetSWCSpectrumTexture("color", RGBColor(0.0f));
	tpMaterial.transparency			= mp.GetSWCSpectrumTexture("transparency", RGBColor(0.0f));
	tpMaterial.ambientColor			= mp.GetSWCSpectrumTexture("ambientColor", RGBColor(0.0f));
	tpMaterial.incandescence		= mp.GetSWCSpectrumTexture("incandescence", RGBColor(0.0f));
	tpMaterial.diffuse				= mp.GetFloatTexture("diffuse", 1.0f);
	tpMaterial.translucence			= mp.GetFloatTexture("translucence", 0.0f);
	tpMaterial.translucenceDepth	= mp.GetFloatTexture("translucenceDepth", 0.0f);
	tpMaterial.translucenceFocus	= mp.GetFloatTexture("translucenceFocus", 0.0f);
	tpMaterial.glowIntensity		= mp.GetFloatTexture("glowIntensity", 0.0f);
	tpMaterial.matteOpacityMode		= mp.FindString("matteOpacityMode");
	tpMaterial.matteOpacity			= mp.GetFloatTexture("matteOpacity", 0.0f);
	tpMaterial.refractions			= mp.FindBool("refractions", false);
	tpMaterial.chromaticAberration	= mp.FindBool("chromaticAberration", false);
	tpMaterial.hideSource			= mp.FindBool("hideSource", false);
	tpMaterial.refractiveIndex		= mp.GetFloatTexture("refractiveIndex", 0.0f);
	tpMaterial.refractionLimit		= mp.GetFloatTexture("refractionLimit", 0.0f);
	tpMaterial.lightAbsorbance		= mp.GetFloatTexture("lightAbsorbance", 0.0f);
	tpMaterial.surfaceThickness		= mp.GetFloatTexture("surfaceThickness", 0.0f);
	tpMaterial.shadowAttenuation	= mp.GetFloatTexture("shadowAttenuation", 0.0f);

}

// parse Reflect parameter.
void ParamConvertor::parseReflectParam(const TextureParams &mp, TP_GirlMaterial &tpMaterial)
{
	parseLambertParam(mp, tpMaterial);

	// extract the parameter set.
	tpMaterial.reflectionLimit			= mp.FindInt("reflectionLimit", 2);
	tpMaterial.specularColor			= mp.GetSWCSpectrumTexture("specularColor", RGBColor(1.0f));
	tpMaterial.reflectivity				= mp.GetFloatTexture("reflectivity", 1.0f);	
	tpMaterial.reflectedColor			= mp.GetSWCSpectrumTexture("reflectedColor", RGBColor(1.0f));
	tpMaterial.reflectionSpecularity	= mp.GetFloatTexture("reflectionSpecularity", 1.0f);
}

// parse Blinn parameter.
void ParamConvertor::parseBlinnParam(const TextureParams &mp, TP_GirlMaterial &tpMaterial)
{
	parseLambertParam(mp, tpMaterial);
	parseReflectParam(mp, tpMaterial);

	// extract the parameter set.
	tpMaterial.eccentricity			= mp.GetFloatTexture("eccentricity", 1.0f);
	tpMaterial.specularRollOff		= mp.GetFloatTexture("specularRollOff", 1.0f);
	tpMaterial.reflectionRollOff	= mp.FindBool("reflectionRollOff", false);
}

// parse Phong parameter.
void ParamConvertor::parsePhongParam(const TextureParams &mp, TP_GirlMaterial &tpMaterial)
{
	parseLambertParam(mp, tpMaterial);
	parseReflectParam(mp, tpMaterial);

	// extract the parameter set.
	tpMaterial.cosinePower		= mp.GetFloatTexture("cosinePower", 1.0f);
}

// parse PhongE parameter.
void ParamConvertor::parsePhongEParam(const TextureParams &mp, TP_GirlMaterial &tpMaterial)
{
	parseLambertParam(mp, tpMaterial);
	parseReflectParam(mp, tpMaterial);

	// extract the parameter set.
	tpMaterial.roughness		= mp.GetFloatTexture("roughness", 1.0f);
	tpMaterial.highlightSize	= mp.GetFloatTexture("highlightSize", 1.0f);
	tpMaterial.whiteness		= mp.GetSWCSpectrumTexture("whiteness", RGBColor(1.0f));
}

void ParamConvertor::parseLambertParam(const TextureParams &mp, ParamSet &tParam)
{
	// extract the parameter set.
	RGBColor color			= mp.FindRGBColor("color", RGBColor(1.0f));
	RGBColor transparency	= mp.FindRGBColor("transparency", RGBColor(1.0f));
	RGBColor ambientColor	= mp.FindRGBColor("ambientColor", RGBColor(1.0f));
	RGBColor incandescence	= mp.FindRGBColor("incandescence", RGBColor(1.0f));
	float diffuse			= mp.FindFloat("diffuse", 0.8f);
	float translucence		= mp.FindFloat("translucence", 0.8f);
	float translucenceDepth = mp.FindFloat("translucenceDepth", 0.8f);
	float translucenceFocus = mp.FindFloat("translucenceFocus", 0.8f);
	float glowIntensity		= mp.FindFloat("glowIntensity", 0.8f);
	string matteOpacityMode	= mp.FindString("matteOpacityMode");

	float matteOpacity		= mp.FindFloat("matteOpacity", 1.0f);
	bool refractions		= mp.FindBool("refractions", false);
	bool chromaticAberration= mp.FindBool("chromaticAberration", false);
	bool hideSource			= mp.FindBool("hideSource", false);
	float refractiveIndex	= mp.FindFloat("refractiveIndex", 0.8f);
	float refractionLimit	= mp.FindFloat("refractionLimit", 0.8f);
	float lightAbsorbance	= mp.FindFloat("lightAbsorbance", 0.8f);
	float surfaceThickness	= mp.FindFloat("surfaceThickness", 0.8f);	
	float shadowAttenuation	= mp.FindFloat("shadowAttenuation", 0.8f);

	tParam.AddRGBColor("color", &color);
	tParam.AddRGBColor("transparency", &transparency);
	tParam.AddRGBColor("ambientColor", &ambientColor);
	tParam.AddRGBColor("incandescence", &incandescence);
	tParam.AddFloat("diffuse", &diffuse);
	tParam.AddFloat("translucence", &translucence);
	tParam.AddFloat("translucenceDepth", &translucenceDepth);
	tParam.AddFloat("translucenceFocus", &translucenceFocus);
	tParam.AddFloat("glowIntensity", &glowIntensity);
	tParam.AddString("matteOpacityMode", &matteOpacityMode);
	tParam.AddFloat("matteOpacity", &matteOpacity);
	tParam.AddBool("refractions", &refractions);
	tParam.AddBool("chromaticAberration", &chromaticAberration);
	tParam.AddBool("hideSource", &hideSource);
	tParam.AddFloat("refractiveIndex", &refractiveIndex);
	tParam.AddFloat("refractionLimit", &refractionLimit);
	tParam.AddFloat("lightAbsorbance", &lightAbsorbance);
	tParam.AddFloat("surfaceThickness", &surfaceThickness);
	tParam.AddFloat("shadowAttenuation", &shadowAttenuation);
}

// parse Reflect parameter.
void ParamConvertor::parseReflectParam(const TextureParams &mp, ParamSet &tParam)
{
	parseLambertParam(mp, tParam);
	
	// extract the parameter set.
	int			reflectionLimit			= mp.FindInt("reflectionLimit", 2);
	RGBColor	specularColor			= mp.FindRGBColor("specularColor", RGBColor(1.0f));
	float		reflectivity			= mp.FindFloat("reflectivity", 1.0f);	
	RGBColor	reflectedColor			= mp.FindRGBColor("reflectedColor", RGBColor(1.0f));
	float		reflectionSpecularity	= mp.FindFloat("reflectionSpecularity", 1.0f);

	// assign the values to the ParamSet object.
	tParam.AddInt("reflectionLimit", &reflectionLimit);
	tParam.AddRGBColor("specularColor", &specularColor);
	tParam.AddFloat("reflectivity", &reflectivity);
	tParam.AddRGBColor("reflectedColor", &reflectedColor);
	tParam.AddFloat("reflectionSpecularity", &reflectionSpecularity);

}
// parse Blinn parameter.
void ParamConvertor::parseBlinnParam(const TextureParams &mp, ParamSet &tParam)
{
	parseLambertParam(mp, tParam);
	parseReflectParam(mp, tParam);

	// extract the parameter set.
	float	eccentricity		= mp.FindFloat("eccentricity", 1.0f);
	float	specularRollOff		= mp.FindFloat("specularRollOff", 1.0f);
	bool	reflectionRollOff	= mp.FindBool("reflectionRollOff", false);

	// assign the values to the ParamSet object.
	tParam.AddFloat("eccentricity", &eccentricity);
	tParam.AddFloat("specularRollOff", &specularRollOff);
	tParam.AddBool("reflectionRollOff", &reflectionRollOff);
	
}
// parse Phong parameter.
void ParamConvertor::parsePhongParam(const TextureParams &mp, ParamSet &tParam)
{
	parseLambertParam(mp, tParam);
	parseReflectParam(mp, tParam);

	// extract the parameter set.
	float		cosinePower		= mp.FindFloat("cosinePower", 1.0f);

	// assign the values to the ParamSet object.
	tParam.AddFloat("cosinePower", &cosinePower);
}

// parse PhongE parameter.
void ParamConvertor::parsePhongEParam(const TextureParams &mp, ParamSet &tParam)
{
	parseLambertParam(mp, tParam);
	parseReflectParam(mp, tParam);

	// extract the parameter set.
	float		roughness		= mp.FindFloat("roughness", 1.0f);
	float		highlightSize	= mp.FindFloat("highlightSize", 1.0f);
	RGBColor	whiteness		= mp.FindRGBColor("whiteness", RGBColor(1.0f));

	// assign the values to the ParamSet object.
	tParam.AddFloat("roughness", &roughness);
	tParam.AddFloat("highlightSize", &highlightSize);
	tParam.AddRGBColor("whiteness", &whiteness);

}

stLambert ParamConvertor::convertLambertParam(LunaBxDFParamSet lp)
{
	stLambert lParam;

	// assign the parameter set.
	lParam.color.set(lp.color.c[0], lp.color.c[1], lp.color.c[2]);
	lParam.transparency.set(lp.transparency.c[0], lp.transparency.c[1], lp.transparency.c[2]);
	lParam.ambientColor.set(lp.ambientColor.c[0], lp.ambientColor.c[1], lp.ambientColor.c[2]);
	lParam.incandescence.set(lp.incandescence.c[0], lp.incandescence.c[1], lp.incandescence.c[2]);
	lParam.diffuse			= lp.diffuse;
	lParam.translucence		= lp.translucence;
	lParam.translucenceDepth= lp.translucenceDepth;
	lParam.translucenceFocus= lp.translucenceFocus;
	lParam.glowIntensity	= lp.glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(lp.matteOpacityMode);
	lParam.matteOpacity		= lp.matteOpacity;
	lParam.refactions		= lp.refractions;
	lParam.chromaticAberration= lp.chromaticAberration;
	lParam.hideSource		= lp.hideSource;
	lParam.refractionIndex	= lp.refractiveIndex;
	lParam.refractionLimit	= lp.refractionLimit;
	lParam.lightAbsorbance	= lp.lightAbsorbance;
	lParam.surfaceThickness	= lp.surfaceThickness;
	lParam.shadowAttenuation= lp.shadowAttenuation;

	return lParam;
}

stLambert ParamConvertor::convertLambertParam(ParamSet ps)
{
	stLambert lParam;

	// extract the parameter set.
	RGBColor color			= ps.FindOneRGBColor("color", RGBColor(1.0f));
	RGBColor transparency	= ps.FindOneRGBColor("transparency", RGBColor(1.0f));
	RGBColor ambientColor	= ps.FindOneRGBColor("ambientColor", RGBColor(1.0f));
	RGBColor incandescence	= ps.FindOneRGBColor("incandescence", RGBColor(1.0f));
	float diffuse			= ps.FindOneFloat("diffuse", 0.8f);
	float translucence		= ps.FindOneFloat("translucence", 0.8f);
	float translucenceDepth = ps.FindOneFloat("translucenceDepth", 0.8f);
	float translucenceFocus = ps.FindOneFloat("translucenceFocus", 0.8f);
	float glowIntensity		= ps.FindOneFloat("glowIntensity", 0.8f);
	string matteOpacityMode	= ps.FindOneString("matteOpacityMode", "Opacity Gain");
	float matteOpacity		= ps.FindOneFloat("matteOpacity", 1.0f);
	bool refractions		= ps.FindOneBool("refractions", false);
	bool chromaticAberration= ps.FindOneBool("chromaticAberration", false);
	bool hideSource			= ps.FindOneBool("hideSource", false);
	float refractiveIndex	= ps.FindOneFloat("refractiveIndex", 0.8f);
	float refractionLimit	= ps.FindOneFloat("refractionLimit", 0.8f);
	float lightAbsorbance	= ps.FindOneFloat("lightAbsorbance", 0.8f);
	float surfaceThickness	= ps.FindOneFloat("surfaceThickness", 0.8f);	
	float shadowAttenuation	= ps.FindOneFloat("shadowAttenuation", 0.8f);

	// assign the parameter set.
	lParam.color.set(color.c[0], color.c[1], color.c[2]);
	lParam.transparency.set(transparency.c[0], transparency.c[1], transparency.c[2]);
	lParam.ambientColor.set(ambientColor.c[0], ambientColor.c[1], ambientColor.c[2]);
	lParam.incandescence.set(incandescence.c[0], incandescence.c[1], incandescence.c[2]);
	lParam.diffuse			= diffuse;
	lParam.translucence		= translucence;
	lParam.translucenceDepth= translucenceDepth;
	lParam.translucenceFocus= translucenceFocus;
	lParam.glowIntensity	= glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(matteOpacityMode);
	lParam.matteOpacity		= matteOpacity;
	lParam.refactions		= refractions;
	lParam.chromaticAberration= chromaticAberration;
	lParam.hideSource		= hideSource;
	lParam.refractionIndex	= refractiveIndex;
	lParam.refractionLimit	= refractionLimit;
	lParam.lightAbsorbance	= lightAbsorbance;
	lParam.surfaceThickness	= surfaceThickness;
	lParam.surfaceThickness	= shadowAttenuation;

	return lParam;
}

stReflect ParamConvertor::convertReflectParam(LunaBxDFParamSet lp)
{
	stReflect lParam;

	// extract the parameter set. (reflect)
	lParam.reflectionLimit = lp.reflectionLimit;
	lParam.specularColor.set(lp.specularColor.c[0], lp.specularColor.c[1], lp.specularColor.c[1]);
	lParam.reflectivity = lp.reflectivity;
	lParam.reflectedColor.set(lp.reflectedColor.c[0], lp.reflectedColor.c[1], lp.reflectedColor.c[1]);
	lParam.reflectionSpecularity = lp.reflectionSpecularity;

	return lParam;
}

stReflect ParamConvertor::convertReflectParam(ParamSet ps)
{
	stReflect lParam;

	// extract the parameter set.
	int			reflectionLimit			= ps.FindOneInt("reflectionLimit", 2);
	RGBColor	specularColor			= ps.FindOneRGBColor("specularColor", RGBColor(1.0f));
	float		reflectivity			= ps.FindOneFloat("reflectivity", 1.0f);	
	RGBColor	reflectedColor			= ps.FindOneRGBColor("reflectedColor", RGBColor(1.0f));
	float		reflectionSpecularity	= ps.FindOneFloat("reflectionSpecularity", 1.0f);

	// assign the parameter set.
	lParam.reflectionLimit = reflectionLimit;
	lParam.specularColor.set(specularColor.c[0],specularColor.c[1],specularColor.c[2]);
	lParam.reflectivity = reflectivity;
	lParam.reflectedColor.set(reflectedColor.c[0],reflectedColor.c[1],reflectedColor.c[2]);
	lParam.reflectionSpecularity = reflectionSpecularity;

	return lParam;
}

stBlinn	ParamConvertor::convertBlinnParam(LunaBxDFParamSet lp)
{
	stBlinn lParam;

	// assign the parameter set.
	lParam.color.set(lp.color.c[0], lp.color.c[1], lp.color.c[2]);
	lParam.transparency.set(lp.transparency.c[0], lp.transparency.c[1], lp.transparency.c[2]);
	lParam.ambientColor.set(lp.ambientColor.c[0], lp.ambientColor.c[1], lp.ambientColor.c[2]);
	lParam.incandescence.set(lp.incandescence.c[0], lp.incandescence.c[1], lp.incandescence.c[2]);
	lParam.diffuse			= lp.diffuse;
	lParam.translucence		= lp.translucence;
	lParam.translucenceDepth= lp.translucenceDepth;
	lParam.translucenceFocus= lp.translucenceFocus;
	lParam.glowIntensity	= lp.glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(lp.matteOpacityMode);
	lParam.matteOpacity		= lp.matteOpacity;
	lParam.refactions		= lp.refractions;
	lParam.chromaticAberration= lp.chromaticAberration;
	lParam.hideSource		= lp.hideSource;
	lParam.refractionIndex	= lp.refractiveIndex;
	lParam.refractionLimit	= lp.refractionLimit;
	lParam.lightAbsorbance	= lp.lightAbsorbance;
	lParam.surfaceThickness	= lp.surfaceThickness;
	lParam.shadowAttenuation= lp.shadowAttenuation;

	// extract the parameter set. (blinn)
	lParam.eccentricity = lp.eccentricity;
	lParam.specularRollOff = lp.specularRollOff;
	lParam.reflectionRollOff = lp.reflectionRollOff;

	return lParam;
}

stBlinn	ParamConvertor::convertBlinnParam(ParamSet ps)
{
	stBlinn lParam;

	// extract the parameter set.
	RGBColor color			= ps.FindOneRGBColor("color", RGBColor(1.0f));
	RGBColor transparency	= ps.FindOneRGBColor("transparency", RGBColor(1.0f));
	RGBColor ambientColor	= ps.FindOneRGBColor("ambientColor", RGBColor(1.0f));
	RGBColor incandescence	= ps.FindOneRGBColor("incandescence", RGBColor(1.0f));
	float diffuse			= ps.FindOneFloat("diffuse", 0.8f);
	float translucence		= ps.FindOneFloat("translucence", 0.8f);
	float translucenceDepth = ps.FindOneFloat("translucenceDepth", 0.8f);
	float translucenceFocus = ps.FindOneFloat("translucenceFocus", 0.8f);
	float glowIntensity		= ps.FindOneFloat("glowIntensity", 0.8f);
	string matteOpacityMode	= ps.FindOneString("matteOpacityMode", "Opacity Gain");
	float matteOpacity		= ps.FindOneFloat("matteOpacity", 1.0f);
	bool refractions		= ps.FindOneBool("refractions", false);
	bool chromaticAberration= ps.FindOneBool("chromaticAberration", false);
	bool hideSource			= ps.FindOneBool("hideSource", false);
	float refractiveIndex	= ps.FindOneFloat("refractiveIndex", 0.8f);
	float refractionLimit	= ps.FindOneFloat("refractionLimit", 0.8f);
	float lightAbsorbance	= ps.FindOneFloat("lightAbsorbance", 0.8f);
	float surfaceThickness	= ps.FindOneFloat("surfaceThickness", 0.8f);	
	float shadowAttenuation	= ps.FindOneFloat("shadowAttenuation", 0.8f);

	// extract the parameter set. (reflect)
	int			reflectionLimit			= ps.FindOneInt("reflectionLimit", 2);
	RGBColor	specularColor			= ps.FindOneRGBColor("specularColor", RGBColor(1.0f));
	float		reflectivity			= ps.FindOneFloat("reflectivity", 1.0f);	
	RGBColor	reflectedColor			= ps.FindOneRGBColor("reflectedColor", RGBColor(1.0f));
	float		reflectionSpecularity	= ps.FindOneFloat("reflectionSpecularity", 1.0f);

	// extract the parameter set. (blinn)
	float	eccentricity		= ps.FindOneFloat("eccentricity", 1.0f);
	float	specularRollOff		= ps.FindOneFloat("specularRollOff", 1.0f);
	bool	reflectionRollOff	= ps.FindOneBool("reflectionRollOff", false);

	// assign the parameter set.
	lParam.color.set(color.c[0], color.c[1], color.c[2]);
	lParam.transparency.set(transparency.c[0], transparency.c[1], transparency.c[2]);
	lParam.ambientColor.set(ambientColor.c[0], ambientColor.c[1], ambientColor.c[2]);
	lParam.incandescence.set(incandescence.c[0], incandescence.c[1], incandescence.c[2]);
	lParam.diffuse			= diffuse;
	lParam.translucence		= translucence;
	lParam.translucenceDepth= translucenceDepth;
	lParam.translucenceFocus= translucenceFocus;
	lParam.glowIntensity	= glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(matteOpacityMode);
	lParam.matteOpacity		= matteOpacity;
	lParam.refactions		= refractions;
	lParam.chromaticAberration= chromaticAberration;
	lParam.hideSource		= hideSource;
	lParam.refractionIndex	= refractiveIndex;
	lParam.refractionLimit	= refractionLimit;
	lParam.lightAbsorbance	= lightAbsorbance;
	lParam.surfaceThickness	= surfaceThickness;
	lParam.surfaceThickness	= shadowAttenuation;

	// assign the parameter set. (reflect)
	lParam.reflectionLimit = reflectionLimit;
	lParam.specularColor.set(specularColor.c[0],specularColor.c[1],specularColor.c[2]);
	lParam.reflectivity = reflectivity;
	lParam.reflectedColor.set(reflectedColor.c[0],reflectedColor.c[1],reflectedColor.c[2]);
	lParam.reflectionSpecularity = reflectionSpecularity;

	// assign the parameter set. (blinn)
	lParam.eccentricity = eccentricity;
	lParam.specularRollOff = specularRollOff;
	lParam.reflectionRollOff = reflectionRollOff;

	return lParam;
}

stPhong	ParamConvertor::convertPhongParam(LunaBxDFParamSet lp)
{
	stPhong lParam;

	// assign the parameter set.
	lParam.color.set(lp.color.c[0], lp.color.c[1], lp.color.c[2]);
	lParam.transparency.set(lp.transparency.c[0], lp.transparency.c[1], lp.transparency.c[2]);
	lParam.ambientColor.set(lp.ambientColor.c[0], lp.ambientColor.c[1], lp.ambientColor.c[2]);
	lParam.incandescence.set(lp.incandescence.c[0], lp.incandescence.c[1], lp.incandescence.c[2]);
	lParam.diffuse			= lp.diffuse;
	lParam.translucence		= lp.translucence;
	lParam.translucenceDepth= lp.translucenceDepth;
	lParam.translucenceFocus= lp.translucenceFocus;
	lParam.glowIntensity	= lp.glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(lp.matteOpacityMode);
	lParam.matteOpacity		= lp.matteOpacity;
	lParam.refactions		= lp.refractions;
	lParam.chromaticAberration= lp.chromaticAberration;
	lParam.hideSource		= lp.hideSource;
	lParam.refractionIndex	= lp.refractiveIndex;
	lParam.refractionLimit	= lp.refractionLimit;
	lParam.lightAbsorbance	= lp.lightAbsorbance;
	lParam.surfaceThickness	= lp.surfaceThickness;
	lParam.shadowAttenuation= lp.shadowAttenuation;

	// extract the parameter set. (reflect)
	lParam.reflectionLimit = lp.reflectionLimit;
	lParam.specularColor.set(lp.specularColor.c[0], lp.specularColor.c[1], lp.specularColor.c[1]);
	lParam.reflectivity = lp.reflectivity;
	lParam.reflectedColor.set(lp.reflectedColor.c[0], lp.reflectedColor.c[1], lp.reflectedColor.c[1]);
	lParam.reflectionSpecularity = lp.reflectionSpecularity;
	
	// extract the parameter set.
	lParam.cosinePower = lp.cosinePower;

	return lParam;
}

stPhong	ParamConvertor::convertPhongParam(ParamSet ps)
{
	stPhong lParam;

	// extract the parameter set.
	RGBColor color			= ps.FindOneRGBColor("color", RGBColor(1.0f));
	RGBColor transparency	= ps.FindOneRGBColor("transparency", RGBColor(1.0f));
	RGBColor ambientColor	= ps.FindOneRGBColor("ambientColor", RGBColor(1.0f));
	RGBColor incandescence	= ps.FindOneRGBColor("incandescence", RGBColor(1.0f));
	float diffuse			= ps.FindOneFloat("diffuse", 0.8f);
	float translucence		= ps.FindOneFloat("translucence", 0.8f);
	float translucenceDepth = ps.FindOneFloat("translucenceDepth", 0.8f);
	float translucenceFocus = ps.FindOneFloat("translucenceFocus", 0.8f);
	float glowIntensity		= ps.FindOneFloat("glowIntensity", 0.8f);
	string matteOpacityMode	= ps.FindOneString("matteOpacityMode", "Opacity Gain");
	float matteOpacity		= ps.FindOneFloat("matteOpacity", 1.0f);
	bool refractions		= ps.FindOneBool("refractions", false);
	bool chromaticAberration= ps.FindOneBool("chromaticAberration", false);
	bool hideSource			= ps.FindOneBool("hideSource", false);
	float refractiveIndex	= ps.FindOneFloat("refractiveIndex", 0.8f);
	float refractionLimit	= ps.FindOneFloat("refractionLimit", 0.8f);
	float lightAbsorbance	= ps.FindOneFloat("lightAbsorbance", 0.8f);
	float surfaceThickness	= ps.FindOneFloat("surfaceThickness", 0.8f);	
	float shadowAttenuation	= ps.FindOneFloat("shadowAttenuation", 0.8f);

	// extract the parameter set. (reflect)
	int			reflectionLimit			= ps.FindOneInt("reflectionLimit", 2);
	RGBColor	specularColor			= ps.FindOneRGBColor("specularColor", RGBColor(1.0f));
	float		reflectivity			= ps.FindOneFloat("reflectivity", 1.0f);	
	RGBColor	reflectedColor			= ps.FindOneRGBColor("reflectedColor", RGBColor(1.0f));
	float		reflectionSpecularity	= ps.FindOneFloat("reflectionSpecularity", 1.0f);

	// extract the parameter set.
	float		cosinePower		= ps.FindOneFloat("cosinePower", 1.0f);

	// assign the parameter set.
	lParam.color.set(color.c[0], color.c[1], color.c[2]);
	lParam.transparency.set(transparency.c[0], transparency.c[1], transparency.c[2]);
	lParam.ambientColor.set(ambientColor.c[0], ambientColor.c[1], ambientColor.c[2]);
	lParam.incandescence.set(incandescence.c[0], incandescence.c[1], incandescence.c[2]);
	lParam.diffuse			= diffuse;
	lParam.translucence		= translucence;
	lParam.translucenceDepth= translucenceDepth;
	lParam.translucenceFocus= translucenceFocus;
	lParam.glowIntensity	= glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(matteOpacityMode);
	lParam.matteOpacity		= matteOpacity;
	lParam.refactions		= refractions;
	lParam.chromaticAberration= chromaticAberration;
	lParam.hideSource		= hideSource;
	lParam.refractionIndex	= refractiveIndex;
	lParam.refractionLimit	= refractionLimit;
	lParam.lightAbsorbance	= lightAbsorbance;
	lParam.surfaceThickness	= surfaceThickness;
	lParam.surfaceThickness	= shadowAttenuation;

	// assign the parameter set. (reflect)
	lParam.reflectionLimit = reflectionLimit;
	lParam.specularColor.set(specularColor.c[0],specularColor.c[1],specularColor.c[2]);
	lParam.reflectivity = reflectivity;
	lParam.reflectedColor.set(reflectedColor.c[0],reflectedColor.c[1],reflectedColor.c[2]);
	lParam.reflectionSpecularity = reflectionSpecularity;

	// assign the parameter set. (phong)
	lParam.cosinePower = cosinePower;

	return lParam;
}

stPhongE ParamConvertor::convertPhongEParam(LunaBxDFParamSet lp)
{
	stPhongE lParam;

	// assign the parameter set.
	lParam.color.set(lp.color.c[0], lp.color.c[1], lp.color.c[2]);
	lParam.transparency.set(lp.transparency.c[0], lp.transparency.c[1], lp.transparency.c[2]);
	lParam.ambientColor.set(lp.ambientColor.c[0], lp.ambientColor.c[1], lp.ambientColor.c[2]);
	lParam.incandescence.set(lp.incandescence.c[0], lp.incandescence.c[1], lp.incandescence.c[2]);
	lParam.diffuse			= lp.diffuse;
	lParam.translucence		= lp.translucence;
	lParam.translucenceDepth= lp.translucenceDepth;
	lParam.translucenceFocus= lp.translucenceFocus;
	lParam.glowIntensity	= lp.glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(lp.matteOpacityMode);
	lParam.matteOpacity		= lp.matteOpacity;
	lParam.refactions		= lp.refractions;
	lParam.chromaticAberration= lp.chromaticAberration;
	lParam.hideSource		= lp.hideSource;
	lParam.refractionIndex	= lp.refractiveIndex;
	lParam.refractionLimit	= lp.refractionLimit;
	lParam.lightAbsorbance	= lp.lightAbsorbance;
	lParam.surfaceThickness	= lp.surfaceThickness;
	lParam.shadowAttenuation= lp.shadowAttenuation;

	// extract the parameter set. (reflect)
	lParam.reflectionLimit = lp.reflectionLimit;
	lParam.specularColor.set(lp.specularColor.c[0], lp.specularColor.c[1], lp.specularColor.c[2]);
	lParam.reflectivity = lp.reflectivity;
	lParam.reflectedColor.set(lp.reflectedColor.c[0], lp.reflectedColor.c[1], lp.reflectedColor.c[2]);
	lParam.reflectionSpecularity = lp.reflectionSpecularity;

	// extract the parameter set.
	lParam.roughness = lp.roughness;
	lParam.highlightSize = lp.highlightSize;
	lParam.whiteness.set(lp.whiteness.c[0], lp.whiteness.c[1], lp.whiteness.c[2]);

	return lParam;
}


stPhongE ParamConvertor::convertPhongEParam(ParamSet ps)
{
	stPhongE lParam;

	// extract the parameter set.
	RGBColor color			= ps.FindOneRGBColor("color", RGBColor(1.0f));
	RGBColor transparency	= ps.FindOneRGBColor("transparency", RGBColor(1.0f));
	RGBColor ambientColor	= ps.FindOneRGBColor("ambientColor", RGBColor(1.0f));
	RGBColor incandescence	= ps.FindOneRGBColor("incandescence", RGBColor(1.0f));
	float diffuse			= ps.FindOneFloat("diffuse", 0.8f);
	float translucence		= ps.FindOneFloat("translucence", 0.8f);
	float translucenceDepth = ps.FindOneFloat("translucenceDepth", 0.8f);
	float translucenceFocus = ps.FindOneFloat("translucenceFocus", 0.8f);
	float glowIntensity		= ps.FindOneFloat("glowIntensity", 0.8f);
	string matteOpacityMode	= ps.FindOneString("matteOpacityMode", "Opacity Gain");
	float matteOpacity		= ps.FindOneFloat("matteOpacity", 1.0f);
	bool refractions		= ps.FindOneBool("refractions", false);
	bool chromaticAberration= ps.FindOneBool("chromaticAberration", false);
	bool hideSource			= ps.FindOneBool("hideSource", false);
	float refractiveIndex	= ps.FindOneFloat("refractiveIndex", 0.8f);
	float refractionLimit	= ps.FindOneFloat("refractionLimit", 0.8f);
	float lightAbsorbance	= ps.FindOneFloat("lightAbsorbance", 0.8f);
	float surfaceThickness	= ps.FindOneFloat("surfaceThickness", 0.8f);	
	float shadowAttenuation	= ps.FindOneFloat("shadowAttenuation", 0.8f);

	// extract the parameter set. (reflect)
	int			reflectionLimit			= ps.FindOneInt("reflectionLimit", 2);
	RGBColor	specularColor			= ps.FindOneRGBColor("specularColor", RGBColor(1.0f));
	float		reflectivity			= ps.FindOneFloat("reflectivity", 1.0f);	
	RGBColor	reflectedColor			= ps.FindOneRGBColor("reflectedColor", RGBColor(1.0f));
	float		reflectionSpecularity	= ps.FindOneFloat("reflectionSpecularity", 1.0f);

	// extract the parameter set.
	float		roughness		= ps.FindOneFloat("roughness", 1.0f);
	float		highlightSize	= ps.FindOneFloat("highlightSize", 1.0f);
	RGBColor	whiteness		= ps.FindOneRGBColor("whiteness", RGBColor(1.0f));

	// assign the parameter set.
	lParam.color.set(color.c[0], color.c[1], color.c[2]);
	lParam.transparency.set(transparency.c[0], transparency.c[1], transparency.c[2]);
	lParam.ambientColor.set(ambientColor.c[0], ambientColor.c[1], ambientColor.c[2]);
	lParam.incandescence.set(incandescence.c[0], incandescence.c[1], incandescence.c[2]);
	lParam.diffuse			= diffuse;
	lParam.translucence		= translucence;
	lParam.translucenceDepth= translucenceDepth;
	lParam.translucenceFocus= translucenceFocus;
	lParam.glowIntensity	= glowIntensity;
	lParam.matteOpacityMode	= (MOM) getEnumOpacityMode(matteOpacityMode);
	lParam.matteOpacity		= matteOpacity;
	lParam.refactions		= refractions;
	lParam.chromaticAberration= chromaticAberration;
	lParam.hideSource		= hideSource;
	lParam.refractionIndex	= refractiveIndex;
	lParam.refractionLimit	= refractionLimit;
	lParam.lightAbsorbance	= lightAbsorbance;
	lParam.surfaceThickness	= surfaceThickness;
	lParam.surfaceThickness	= shadowAttenuation;

	// assign the parameter set. (reflect)
	lParam.reflectionLimit = reflectionLimit;
	lParam.specularColor.set(specularColor.c[0],specularColor.c[1],specularColor.c[2]);
	lParam.reflectivity = reflectivity;
	lParam.reflectedColor.set(reflectedColor.c[0],reflectedColor.c[1],reflectedColor.c[2]);
	lParam.reflectionSpecularity = reflectionSpecularity;

	// assign the values to the ParamSet object.
	lParam.roughness = roughness;
	lParam.highlightSize = highlightSize;
	lParam.whiteness.set(whiteness.c[0], whiteness.c[1], whiteness.c[2]);

	return lParam;
}