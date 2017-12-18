/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   ParamConvertor.h
//	Author	:   Sung-Soo Kim
//	Purpose :   ParamConvertor.h
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2009-05-11  :   First version - ParamConvertor.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2009 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "girl.h"
#include "girlmaterials.h"
#include "paramset.h"

namespace girl {

	// ParamConvertor class definition
	class GIRLCOREDLL ParamConvertor {
	public:
		ParamConvertor(){};
		~ParamConvertor(){};

	private:
		int getEnumOpacityMode(string strVal);

	public:
		// convert TextureParams to ParamSet.
		ParamSet makeParamSet(string shaderName, const TextureParams &mp);
		
		//----------- Parameter parsing methods.
		// parse Lambert parameter.
		void parseLambertParam(const TextureParams &mp, ParamSet &tParam);
		// parse Reflect parameter.
		void parseReflectParam(const TextureParams &mp, ParamSet &tParam);
		// parse Blinn parameter.
		void parseBlinnParam(const TextureParams &mp, ParamSet &tParam);
		// parse Phong parameter.
		void parsePhongParam(const TextureParams &mp, ParamSet &tParam);
		// parse PhongE parameter.
		void parsePhongEParam(const TextureParams &mp, ParamSet &tParam);

		//----------- Parameter conversion methods. (ParamSet to shader parameter structures.)
		// convert Lambert structure.
		stLambert	convertLambertParam(ParamSet ps);
		stReflect	convertReflectParam(ParamSet ps);
		stBlinn		convertBlinnParam(ParamSet ps);
		stPhong		convertPhongParam(ParamSet ps);
		stPhongE	convertPhongEParam(ParamSet ps);
	};
}// end of namespace

