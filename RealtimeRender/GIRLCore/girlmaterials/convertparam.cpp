/////////////// GIRL (Global Ilummination Renderer Library) Materials //////////////////
//	FileName:   convertparam.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-04-20  :   First version - convertparam.cpp initial implementation
//					DLL export/import directives
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#include "convertparam.h"

void conParam2Lambert(const TextureParams &mp, stLambert &lParam)
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
	int matteOpacityMode	= mp.FindInt("matteOpacityMode", 1);
	float matteOpacity		= mp.FindFloat("matteOpacity", 1.0f);
	bool refractions		= mp.FindBool("refractions", false);
	bool chromaticAberration= mp.FindBool("chromaticAberration", false);
	bool hideSource			= mp.FindBool("hideSource", false);
	float refractiveIndex	= mp.FindFloat("refractiveIndex", 0.8f);
	float refractionLimit	= mp.FindFloat("refractionLimit", 0.8f);
	float lightAbsorbance	= mp.FindFloat("lightAbsorbance", 0.8f);
	float surfaceThickness	= mp.FindFloat("surfaceThickness", 0.8f);	
	float shadowAttenuation	= mp.FindFloat("shadowAttenuation", 0.8f);

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
	lParam.matteOpacityMode	= (MOM) matteOpacityMode;
	lParam.matteOpacity		= matteOpacity;
	lParam.refactions		= refractions;
	lParam.chromaticAberration= chromaticAberration;
	lParam.hideSource		= hideSource;
	lParam.refractionIndex	= refractiveIndex;
	lParam.refractionLimit	= refractionLimit;
	lParam.lightAbsorbance	= lightAbsorbance;
	lParam.surfaceThickness	= surfaceThickness;
	lParam.surfaceThickness	= shadowAttenuation;
}

void parseLambertParam(const TextureParams &mp, ParamSet &tParam)
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
	int matteOpacityMode	= mp.FindInt("matteOpacityMode", 1);
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
	tParam.AddInt("matteOpacityMode", &matteOpacityMode);
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

ParamSet makeParamSet(string shaderName, const TextureParams &mp)
{
	ParamSet	tParam;

	if (shaderName.c_str() == "GirlLambert") {
		parseLambertParam(mp, tParam);
	}

	return tParam;
}

stLambert convertLambertParam(ParamSet ps)
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
	int matteOpacityMode	= ps.FindOneInt("matteOpacityMode", 1);
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
	lParam.matteOpacityMode	= (MOM) matteOpacityMode;
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