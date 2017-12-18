#pragma once

#include "stdafx.h"
#include "GCamera.h"
#include "SDKmisc.h"
#include "resource.h"
#include "GMaterial.h"
#include "GLight.h"

#define MAX_RENDERTARGET	8

class GShader
{
public:

	GShader( VOID );
	~GShader( VOID );

	HRESULT								Init( ID3D10Device* pd3dDevice );

	VOID SetupCamera( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const GCameraPacket& stCamInfo );

	VOID SetupDepthRenderCamera( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const FLOAT fNear, const FLOAT fFar );

	VOID SetDWTSRVs( ID3D10ShaderResourceView* pSourceTexSRV, ID3D10ShaderResourceView* pHighTexSRV, ID3D10ShaderResourceView* pLowTexSRV, UINT widthVP, UINT heightVP );

	VOID SetDWTSRVs( ID3D10ShaderResourceView* pScaleSRV, ID3D10ShaderResourceView* pCoeffsSRV, ID3D10ShaderResourceView* pIndicesSRV, UINT iNumCoeffs, UINT iNumLevel, UINT iSquareIndex );

	VOID SetVShaderParams( UINT iAttr, D3DXVECTOR4& vValue ) { m_pVShaderParams[iAttr]->SetFloatVector( vValue ); }

	VOID SetSRVShaderParams( UINT iAttr, ID3D10ShaderResourceView* pSRV ) { m_pSRVShaderParams[iAttr]->SetResource( pSRV ); }

	VOID								SetQuadTexSRV( CGMaterial* pMaterial );

	VOID								SetQuadTexSRV( ID3D10ShaderResourceView** ppSRV );
	
	VOID								SetQuadTexSRV( ID3D10ShaderResourceView* pSRV );

	VOID								SetShadowMapTex( ID3D10ShaderResourceView* pSRV ) { m_pSRVShadowMap->SetResource( pSRV ); }

	//VOID								SetShadowInvMatrix( D3DXMATRIX* matShadowInv ) { m_pSVmatShadowInv->SetMatrix( matShadowInv ); }
	VOID								SetOffScreenTex( ID3D10ShaderResourceView* pSRV ) { m_pSRVOffScreenTex->SetResource( pSRV ); }

	VOID SetWorldMatrix( float* pMatObjectToWorld );

	VOID SetWorldInvTransMatrix( D3DXMATRIX* pMatWorldInvTrans ) { m_pSVmatWorldInvTrans->SetMatrix( *pMatWorldInvTrans ); }

	VOID SetLighting( GLight* pLight, const D3DXMATRIX* matViewInv, UINT iLightFlag );

	VOID SetNumUseSample( UINT iNumSample ) { m_pSVNumUseSample->SetInt( iNumSample ); }
	//VOID SetDepthTexture( ID3D10DepthStencilView* pDSV ) { m_pSRVDepthTexture->SetResource(pDSV); }

	VOID SetupDWTRender( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, UINT width, UINT height, SGTexture& stTexture );



	/************************************************************************/
	/* Get / Set Stuffs                                                     */
	/************************************************************************/

	ID3D10Effect*						GetMainEffect( VOID ) { return m_pEffectMain; }

	ID3D10Effect*						GetSSMapEffect( VOID ) { return m_pEffectSoftShadowMap; }

	ID3D10Effect*						GetDWTEffect( VOID ) { return m_pEffectDWT; }

	ID3D10EffectTechnique*				GetTechnique( const char* szTechnique );

protected:

	BOOL								bLoadedBaseShaders;

	/************************************************************************/
	/* Device                                                               */
	/************************************************************************/
	ID3D10Device*						m_pd3dDevice;
	ID3D10Effect*                       m_pEffectMain;
	ID3D10Effect*                       m_pEffectDWT;
	ID3D10Effect*                       m_pEffectSoftShadowMap;

	/************************************************************************/
	/* Buffers                                                              */
	/************************************************************************/
	ID3D10Buffer*                       m_pQuadVB;
	ID3D10InputLayout*                  m_pQuadLayout;

	/************************************************************************/
	/* SRVs                                                                 */
	/************************************************************************/
	ID3D10EffectShaderResourceVariable*	m_pSRVShaderParams[MAX_RENDERTARGET];
	ID3D10EffectShaderResourceVariable*	m_pSRVRenderTargets[MAX_RENDERTARGET];
	ID3D10EffectShaderResourceVariable*	m_pSRVQuad;
	ID3D10EffectShaderResourceVariable*	m_pSRVShadowMap;
	ID3D10EffectShaderResourceVariable*	m_pSRVOffScreenTex;
	
	/************************************************************************/
	/* Techniques                                                           */
	/************************************************************************/
	ID3D10EffectTechnique*              m_pTechniqueFillMRT;
	ID3D10EffectTechnique*              m_pTechniqueDrawQuad;
	ID3D10EffectTechnique*              m_pTechniqueDrawSimpleQuad;
	ID3D10EffectTechnique*              m_pTechniqueLambert;
	ID3D10EffectTechnique*              m_pTechniqueBlinn;
	ID3D10EffectTechnique*              m_pTechniquePhong;

	/************************************************************************/
	/* Shader Variables                                                     */
	/************************************************************************/
	ID3D10EffectVectorVariable*         m_pVShaderParams[MAX_RENDERTARGET];
	
	ID3D10EffectVectorVariable*         m_pSVvEyePos;
	ID3D10EffectVectorVariable*         m_pSVvLightPos;
	ID3D10EffectVectorVariable*         m_pSVvLightDir;
	ID3D10EffectVectorVariable*         m_pSVvLightColor;	
	ID3D10EffectVectorVariable*         m_pSVvShadowColor;

	ID3D10EffectVectorVariable*			m_pSRVBarrier;
	ID3D10EffectVectorVariable*			m_pSRVKernel;
	ID3D10EffectVectorVariable*			m_pSRVWeights;
	
	ID3D10EffectScalarVariable*         m_pSVfNear;
	ID3D10EffectScalarVariable*         m_pSVfFar;
	ID3D10EffectScalarVariable*         m_pSVfLeft;
	ID3D10EffectScalarVariable*         m_pSVfTop;
	ID3D10EffectScalarVariable*         m_pSVfLightNear;
	ID3D10EffectScalarVariable*         m_pSVfLightFar;

	ID3D10EffectScalarVariable*         m_pSViShaderID;
	ID3D10EffectScalarVariable*         m_pSViLightFlag;
	ID3D10EffectScalarVariable*         m_pSViShadowFlag;

	ID3D10EffectScalarVariable*         m_pSVfCosPenumbra;
	ID3D10EffectScalarVariable*         m_pSVfCosUmbra;
	ID3D10EffectScalarVariable*         m_pSVfDropOff;
	ID3D10EffectScalarVariable*         m_pSVfAmbientShade;
	ID3D10EffectScalarVariable*         m_pSViDecay;	

	ID3D10EffectScalarVariable*			m_pSVNumUseSample;
	
	ID3D10EffectMatrixVariable*         m_pSVmatViewProj;
	ID3D10EffectMatrixVariable*         m_pSVmatProj;
	ID3D10EffectMatrixVariable*         m_pSVmatView;
	ID3D10EffectMatrixVariable*         m_pSVmatWorld;
	ID3D10EffectMatrixVariable*         m_pSVmatPersInv;
	ID3D10EffectMatrixVariable*         m_pSVmatShadowInv;
	ID3D10EffectMatrixVariable*         m_pSVmatWorldInvTrans;

	ID3D10EffectShaderResourceVariable*	m_pSRVDepthTexture;

	/************************************************************************/
	/* Soft Shadow Map                                                      */
	/************************************************************************/

	/************************************************************************/
	/* Daubeches DWT 2D                                                     */
	/************************************************************************/
	ID3D10EffectShaderResourceVariable*	m_pSRVDWTTexture;
	ID3D10EffectShaderResourceVariable*	m_pSRVDWTLowPassTexture;
	ID3D10EffectShaderResourceVariable*	m_pSRVDWTHighPassTexture;
	ID3D10EffectMatrixVariable*         m_pSVmatDWTModelViewProj;
	ID3D10EffectScalarVariable*         m_pSVfTexelWidth;	
	ID3D10EffectScalarVariable*         m_pSVfTexelHeight;	
	ID3D10EffectScalarVariable*         m_pSVfWidth;	
	ID3D10EffectScalarVariable*         m_pSVfHeight;	

	ID3D10EffectScalarVariable*         m_pSViNumCoeffs;	
	ID3D10EffectScalarVariable*         m_pSViNumLevel;
	ID3D10EffectScalarVariable*         m_pSViSquareIndex;
	ID3D10EffectShaderResourceVariable*	m_pSRVDWTScale;
	ID3D10EffectShaderResourceVariable*	m_pSRVDWTCoeffs;	
	ID3D10EffectShaderResourceVariable*	m_pSRVDWTIndices;
	
	/************************************************************************/
	/* UI                                                                   */
	/************************************************************************/
	ID3DX10Font*                        m_pFont10;
	ID3DX10Sprite*                      m_pSprite10;
};