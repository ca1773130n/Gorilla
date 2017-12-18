#pragma once

#include "material.h"
#include "girlmaterial.h"
#include "spectrumwavelengths.h"
#include "texture.h"
#include <vector.h>
#include "half.h"

using namespace girl;
using namespace std;

class SWC
{
public:
	USHORT index[2];
	float value;

public:
	bool operator<( SWC rhs ) { return fabs(float(value)) > fabs(float(rhs.value)); }
};

struct SGTexture
{
	UINT width;
	UINT height;

	ID3D10Texture2D* pTex;
	ID3D10ShaderResourceView* pTexSRV;
	int channelIndex;

	// DWT
	int startLevel;
	ID3D10Texture2D* pTexScale;
	ID3D10Texture2D* pTexCoeffs;
	ID3D10Texture2D* pTexIndices;
	ID3D10ShaderResourceView* pSRVScale;
	ID3D10ShaderResourceView* pSRVCoeffs;
	ID3D10ShaderResourceView* pSRVIndices;

	vector<SWC> coeffs[3];
	UINT numCoeffs;
};

class CGAttribute
{
public:
	CGAttribute() 
	{ 
		texture.pTex = NULL; 
		texture.pTexSRV = NULL;
	}
	BOOL IsConstant( VOID ) { return ((texture.pTexSRV == NULL) ? TRUE : FALSE); }
public:
	SGTexture texture;		
	RGBColor constant;	
};

class CGMaterial 
{
public:
	CGMaterial( GIRLMaterial* pGIRLMaterial ) 
	{		
		m_pGIRLMaterial = pGIRLMaterial; 
		SetupAttribute( m_AttrAmbientColor, m_pGIRLMaterial->ambientColor.get() );
		SetupAttribute( m_AttrColor, m_pGIRLMaterial->color.get() );		
		SetupAttribute( m_AttrTransparency, m_pGIRLMaterial->transparency.get() );
		SetupAttribute( m_AttrIncandescence, m_pGIRLMaterial->incandescence.get() );
		SetupAttribute( m_AttrNormal, m_pGIRLMaterial->bumpMap.get(), NULL, TRUE );
		SetupAttribute( m_AttrCommon1, m_pGIRLMaterial->diffuse.get(), m_pGIRLMaterial->refractiveIndex.get(), m_pGIRLMaterial->surfaceThickness.get() );

		/*
		SetupAttribute( m_AttrCommon1, 
						m_pGIRLMaterial->surfaceThickness.get(), 
						m_pGIRLMaterial->refractionLimit.get(), 
						m_pGIRLMaterial->translucenceFocus.get()
						);
		
		SetupAttribute( m_AttrCommon2, 
						m_pGIRLMaterial->shadowAttenuation.get(),
						m_pGIRLMaterial->translucence.get(),
						m_pGIRLMaterial->refractiveIndex.get()
						);
		SetupAttribute( m_AttrCommon3, 
						m_pGIRLMaterial->glowIntensity.get(),
						m_pGIRLMaterial->lightAbsorbance.get(),	
						m_pGIRLMaterial->translucenceDepth.get()
						);
		SetupAttribute( m_AttrCommon4, 
						m_pGIRLMaterial->bumpMap.get(),
						m_pGIRLMaterial->diffuse.get(),
						m_pGIRLMaterial->matteOpacity.get()
						);
						*/
	}

	CGAttribute&				GetAttribute( int iAttr );

	VOID						SetupAttribute( CGAttribute& attr, RGBColor rgb ) { attr.constant = rgb; }

	VOID						SetupAttribute( CGAttribute& attr, FLOAT f ) { attr.constant = RGBColor( f ); }

	VOID						SetupAttribute( CGAttribute& attr, Texture<SWCSpectrum>* pTex1, Texture<FLOAT>* pTex2 = NULL, bool bBumpMap = FALSE );

	VOID						SetupAttribute( CGAttribute& attr, Texture<float>* pTex1, Texture<float>* pTex2, Texture<float>* pTex3, Texture<float>* pTex4 = NULL );

	VOID						SetupLambert( VOID );

	VOID						SetupBlinn( VOID );

	VOID						SetupPhong( VOID );

	enGIRLMaterialType			GetType( VOID ) { return m_pGIRLMaterial->GetGIRLMaterialType(); }

	GIRLMaterial*				GetGIRLMaterial( VOID ) { return m_pGIRLMaterial; }

	VOID						SetGIRLMaterial( Material* pMat ) { m_pGIRLMaterial = (GIRLMaterial*)pMat; }

	ID3D10EffectTechnique*		GetTechnique( VOID ) { return m_pTechnique; }

	VOID						SetTechnique( ID3D10EffectTechnique* pTechnique ) { m_pTechnique = pTechnique; }

public:
	GIRLMaterial*				m_pGIRLMaterial;
	CGAttribute					m_AttrTransparency;
	CGAttribute					m_AttrIncandescence;
	CGAttribute					m_AttrAmbientColor;
	CGAttribute					m_AttrColor;
	CGAttribute					m_AttrSpecularColor;	
	CGAttribute					m_AttrSpecularAttr1;
	CGAttribute					m_AttrSpecularAttr2;
	CGAttribute					m_AttrNormal;
	CGAttribute					m_AttrCommon1;
	CGAttribute					m_AttrCommon2;
	CGAttribute					m_AttrCommon3;	
	CGAttribute					m_AttrCommon4;	
	ID3D10EffectTechnique*		m_pTechnique;
};