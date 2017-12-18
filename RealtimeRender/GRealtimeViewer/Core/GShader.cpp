#include "stdafx.h"
#include "GGlobal.h"
#include "GShader.h"
#include "coloring.h"

GShader::GShader()
{

}

GShader::~GShader()
{

}

HRESULT 
GShader::Init( ID3D10Device* pd3dDevice )
{
	HRESULT hr = S_OK;
	m_pd3dDevice = pd3dDevice;

	WCHAR str[MAX_PATH];
	DWORD dwShaderFlags = 0;// D3DXSHADER_DEBUG | D3D10_SHADER_ENABLE_STRICTNESS;
	ID3D10Blob* pBlob;
	V( D3DX10CreateEffectFromFile( L".\\DeferredLighting.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, m_pd3dDevice, NULL, NULL, &m_pEffectMain, &pBlob, NULL ) );
	if( FAILED(hr) )
	{
		GDEBUG( "Failed to compile effect [ %s ]\n", pBlob->GetBufferPointer() );
		char str[1024];
		sprintf( str, "%s", pBlob->GetBufferPointer() );
		TRACE(str);
		return hr;
	}

	//dwShaderFlags = D3DXSHADER_DEBUG;
	//dwShaderFlags = 0;
	V( D3DX10CreateEffectFromFile( L".\\Daubeches2D.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, m_pd3dDevice, NULL, NULL, &m_pEffectDWT, &pBlob, NULL ) );
	if( FAILED(hr) )
	{
		GDEBUG( "Failed to compile effect [ %s ]\n", pBlob->GetBufferPointer() );
		char str[1024];
		sprintf( str, "%s", pBlob->GetBufferPointer() );
		TRACE(str);
		return hr;
	}

	m_pTechniqueFillMRT		= m_pEffectMain->GetTechniqueByName( "FillMRT" );
	m_pTechniqueDrawQuad	= m_pEffectMain->GetTechniqueByName( "DrawQuad" );
	m_pTechniqueDrawSimpleQuad	= m_pEffectMain->GetTechniqueByName( "DrawSimpleQuad" );
	m_pTechniqueLambert		= m_pEffectMain->GetTechniqueByName( "Lambert" );
	m_pTechniqueBlinn		= m_pEffectMain->GetTechniqueByName( "Blinn" );
	m_pTechniquePhong		= m_pEffectMain->GetTechniqueByName( "Phong" );

	m_pSVmatViewProj		= m_pEffectMain->GetVariableByName( "g_mViewProjection" )->AsMatrix();
	m_pSVmatProj			= m_pEffectMain->GetVariableByName( "g_mProjection" )->AsMatrix();
	m_pSVmatView			= m_pEffectMain->GetVariableByName( "g_mView" )->AsMatrix();
	m_pSVmatWorld			= m_pEffectMain->GetVariableByName( "g_mWorld" )->AsMatrix();
	//m_pSVmatPersInv			= m_pEffect10->GetVariableByName( "g_mPersInv" )->AsMatrix();
	m_pSVmatWorldInvTrans	= m_pEffectMain->GetVariableByName( "g_mWorldInverseTranspose" )->AsMatrix();
	m_pSVmatShadowInv		= m_pEffectMain->GetVariableByName( "g_mShadow" )->AsMatrix();

	m_pSVvEyePos			= m_pEffectMain->GetVariableByName( "g_vEyePos" )->AsVector();
	m_pSVfNear				= m_pEffectMain->GetVariableByName( "g_fNear" )->AsScalar();
	m_pSVfFar				= m_pEffectMain->GetVariableByName( "g_fFar" )->AsScalar();
	m_pSVfLeft				= m_pEffectMain->GetVariableByName( "g_fLeft" )->AsScalar();
	m_pSVfTop				= m_pEffectMain->GetVariableByName( "g_fTop" )->AsScalar();
	m_pSVvLightPos			= m_pEffectMain->GetVariableByName( "g_vLightPos" )->AsVector();
	m_pSVvLightDir			= m_pEffectMain->GetVariableByName( "g_vLightDir" )->AsVector();
	m_pSVvLightColor		= m_pEffectMain->GetVariableByName( "g_vLightColor" )->AsVector();
	m_pSVvShadowColor		= m_pEffectMain->GetVariableByName( "g_vShadowColor" )->AsVector();

	m_pSRVShaderParams[0]	= m_pEffectMain->GetVariableByName( "g_txAmbient" )->AsShaderResource();
	m_pSRVShaderParams[1]	= m_pEffectMain->GetVariableByName( "g_txColor" )->AsShaderResource();
	m_pSRVShaderParams[2]	= m_pEffectMain->GetVariableByName( "g_txSpecular" )->AsShaderResource();
	m_pSRVShaderParams[3]	= m_pEffectMain->GetVariableByName( "g_txSpecularAttr1" )->AsShaderResource();
	m_pSRVShaderParams[4]	= m_pEffectMain->GetVariableByName( "g_txTransparency" )->AsShaderResource();	
	m_pSRVShaderParams[5]	= m_pEffectMain->GetVariableByName( "g_txCommon1" )->AsShaderResource();	
	m_pSRVShaderParams[6]	= m_pEffectMain->GetVariableByName( "g_txCommon2" )->AsShaderResource();
	m_pSRVShaderParams[7]	= m_pEffectMain->GetVariableByName( "g_txNormal" )->AsShaderResource();	

	m_pSRVRenderTargets[0]	= m_pEffectMain->GetVariableByName( "g_rtAmbient" )->AsShaderResource();
	m_pSRVRenderTargets[1]	= m_pEffectMain->GetVariableByName( "g_rtColor" )->AsShaderResource();
	m_pSRVRenderTargets[2]	= m_pEffectMain->GetVariableByName( "g_rtSpecular" )->AsShaderResource();
	m_pSRVRenderTargets[3]	= m_pEffectMain->GetVariableByName( "g_rtSpecularAttr1" )->AsShaderResource();
	m_pSRVRenderTargets[4]	= m_pEffectMain->GetVariableByName( "g_rtTransparency" )->AsShaderResource();
	m_pSRVRenderTargets[5]	= m_pEffectMain->GetVariableByName( "g_rtCommon1" )->AsShaderResource();
	m_pSRVRenderTargets[6]	= m_pEffectMain->GetVariableByName( "g_rtCommon2" )->AsShaderResource();
	m_pSRVRenderTargets[7]	= m_pEffectMain->GetVariableByName( "g_rtNormal" )->AsShaderResource();

	m_pVShaderParams[0]		= m_pEffectMain->GetVariableByName( "g_vAmbient" )->AsVector();
	m_pVShaderParams[1]		= m_pEffectMain->GetVariableByName( "g_vColor" )->AsVector();
	m_pVShaderParams[2]		= m_pEffectMain->GetVariableByName( "g_vSpecular" )->AsVector();
	m_pVShaderParams[3]		= m_pEffectMain->GetVariableByName( "g_vSpecularAttr1" )->AsVector();
	m_pVShaderParams[4]		= m_pEffectMain->GetVariableByName( "g_vTransparency" )->AsVector();
	m_pVShaderParams[5]		= m_pEffectMain->GetVariableByName( "g_vCommon1" )->AsVector();
	m_pVShaderParams[6]		= m_pEffectMain->GetVariableByName( "g_vCommon2" )->AsVector();
	m_pVShaderParams[7]		= m_pEffectMain->GetVariableByName( "g_vNormal" )->AsVector();

	m_pSViShaderID			= m_pEffectMain->GetVariableByName( "g_iShaderID" )->AsScalar();	
	m_pSViLightFlag			= m_pEffectMain->GetVariableByName( "g_iLightFlag" )->AsScalar();	
	m_pSViShadowFlag		= m_pEffectMain->GetVariableByName( "g_iShadowFlag" )->AsScalar();	

	m_pSVfCosPenumbra		= m_pEffectMain->GetVariableByName( "g_fCosPenumbra" )->AsScalar();	
	m_pSVfCosUmbra			= m_pEffectMain->GetVariableByName( "g_fCosUmbra" )->AsScalar();	
	m_pSViDecay				= m_pEffectMain->GetVariableByName( "g_fDecay" )->AsScalar();	
	m_pSVfAmbientShade		= m_pEffectMain->GetVariableByName( "g_fAmbientShade" )->AsScalar();	
	m_pSVfDropOff			= m_pEffectMain->GetVariableByName( "g_fDropOff" )->AsScalar();	

	m_pSRVQuad				= m_pEffectMain->GetVariableByName( "g_txQuad" )->AsShaderResource();	
	m_pSRVShadowMap			= m_pEffectMain->GetVariableByName( "tDepthMap" )->AsShaderResource();	
	m_pSRVOffScreenTex		= m_pEffectMain->GetVariableByName( "g_txOffScreen" )->AsShaderResource();	
	//m_pSRVBarrier			= m_pEffect10->GetVariableByName( "g_vBarrier" )->AsVector();	
	//m_pSRVWeights			= m_pEffect10->GetVariableByName( "g_vWeights" )->AsVector();	
	//m_pSRVKernel			= m_pEffect10->GetVariableByName( "g_vKernel" )->AsVector();	

	m_pSVNumUseSample		= m_pEffectMain->GetVariableByName( "g_iNumUseSample" )->AsScalar();

	m_pSVfLightFar			= m_pEffectMain->GetVariableByName( "g_fLightFar" )->AsScalar();
	m_pSVfLightNear			= m_pEffectMain->GetVariableByName( "g_fLightNear" )->AsScalar();

	//////////////////////////////////////////////////////////////////////////
	// DWT

	m_pSRVDWTTexture		= m_pEffectDWT->GetVariableByName( "SourceTexture" )->AsShaderResource();
	m_pSRVDWTLowPassTexture	= m_pEffectDWT->GetVariableByName( "LowPassSourceTexture" )->AsShaderResource();
	m_pSRVDWTHighPassTexture= m_pEffectDWT->GetVariableByName( "HighPassSourceTexture" )->AsShaderResource();
	m_pSVmatDWTModelViewProj= m_pEffectDWT->GetVariableByName( "mvpMatrix" )->AsMatrix();
	m_pSVfTexelWidth		= m_pEffectDWT->GetVariableByName( "texelWidth" )->AsScalar();
	m_pSVfTexelHeight		= m_pEffectDWT->GetVariableByName( "texelHeight" )->AsScalar();
	m_pSVfWidth				= m_pEffectDWT->GetVariableByName( "width" )->AsScalar();
	m_pSVfHeight			= m_pEffectDWT->GetVariableByName( "height" )->AsScalar();

	m_pSRVDWTCoeffs			= m_pEffectDWT->GetVariableByName( "g_txCoeffs" )->AsShaderResource();
	m_pSRVDWTIndices		= m_pEffectDWT->GetVariableByName( "g_txIndices" )->AsShaderResource();
	m_pSRVDWTScale			= m_pEffectDWT->GetVariableByName( "g_txScales" )->AsShaderResource();		
	m_pSViNumCoeffs			= m_pEffectDWT->GetVariableByName( "g_iNumCoeffs" )->AsScalar();
	m_pSViNumLevel			= m_pEffectDWT->GetVariableByName( "g_iLevel" )->AsScalar();
	m_pSViSquareIndex		= m_pEffectDWT->GetVariableByName( "g_iSquareIndex" )->AsScalar();

	return hr;
}

VOID GShader::SetupCamera( const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const GCameraPacket& stCamInfo )
{
	HRESULT hr;

	D3DXMATRIXA16 mWorld;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProj;
	D3DXMATRIXA16 mWorldViewProjection;
	D3DXMATRIXA16 mWorldView;
	D3DXMATRIXA16 mViewProj;
	D3DXMATRIXA16 mPers, mPersInv;
	D3DXMATRIXA16 mWorldInvTrans;
	
	mWorld = matWorld;
	mView = matView;
	mProj = matProj;

	mViewProj = mView * mProj;

	D3DXMatrixIdentity( &mPers );
	D3DXMatrixIdentity( &mPersInv );
	D3DXMatrixIdentity( &mWorldInvTrans );
	D3DXMatrixTranspose( &mPers, &mProj );	
	
	D3DXMatrixInverse( &mWorldInvTrans, 0, &mView );
	//D3DXMatrixTranspose( &mWorldInvTrans, &mWorldInvTrans );

	m_pSVvEyePos->SetFloatVector( (float*)&D3DXVECTOR3(-stCamInfo.vEye[0], -stCamInfo.vEye[2], stCamInfo.vEye[1]) );
	m_pSVfNear->SetFloat( stCamInfo.fNear );
	m_pSVfFar->SetFloat( stCamInfo.fFar );
	m_pSVfLeft->SetFloat( stCamInfo.fLeft );
	m_pSVfTop->SetFloat( stCamInfo.fTop );

	V( m_pSVmatViewProj->SetMatrix( (float*)&mViewProj ) );
	V( m_pSVmatWorld->SetMatrix( (float*)&mWorld ) );
	V( m_pSVmatView->SetMatrix( (float*)&mView ) );
	V( m_pSVmatProj->SetMatrix( (float*)&mProj ) );

	//D3DXMatrixInverse( &mPersInv, 0, &mViewProj );

	//V( m_pSVmatPersInv->SetMatrix( (float*)&mPersInv ) );
	V( m_pSVmatWorldInvTrans->SetMatrix( (float*)&mWorldInvTrans ) );

	m_pSVNumUseSample->SetInt( 1 );
}

VOID GShader::SetupDepthRenderCamera( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const FLOAT fNear, const FLOAT fFar )
{
	HRESULT hr;

	D3DXMATRIXA16 mWorld;
	D3DXMATRIXA16 mView;
	D3DXMATRIXA16 mProj;
	D3DXMATRIXA16 mViewProj;

	mView = matView;
	mProj = matProj;
	mViewProj = mView * mProj;

	m_pSVfLightNear->SetFloat( fNear );
	m_pSVfLightFar->SetFloat( fFar );

	V( m_pSVmatViewProj->SetMatrix( (float*)&mViewProj ) );
	V( m_pSVmatWorld->SetMatrix( (float*)&mWorld ) );
	V( m_pSVmatView->SetMatrix( (float*)&mView ) );
	V( m_pSVmatProj->SetMatrix( (float*)&mProj ) );

	m_pSVNumUseSample->SetInt( 1 );
}

VOID GShader::SetupDWTRender( const D3DXMATRIX& matView, const D3DXMATRIX& matProj, UINT width, UINT height, SGTexture& stTexture )
{
	D3DXMATRIX matViewProj;
	matViewProj = matView * matProj;
	m_pSVmatDWTModelViewProj->SetMatrix( matViewProj );

	m_pSRVDWTTexture->SetResource( stTexture.pTexSRV );
	m_pSVfTexelWidth->SetFloat( 1.0 / width );
	m_pSVfTexelHeight->SetFloat( 1.0 / height );
}

VOID
GShader::SetQuadTexSRV( ID3D10ShaderResourceView** ppSRV )
{
	for( int i=0; i < MAX_RENDERTARGET; i++ )
		m_pSRVRenderTargets[i]->SetResource( ppSRV[i] );
}

VOID
GShader::SetQuadTexSRV( CGMaterial* pMaterial  )
{
	D3DXVECTOR3 vAttrRGBValues[MAX_RENDERTARGET];
	ID3D10Texture2D* pAttrTextures[MAX_RENDERTARGET];

	int shaderID = pMaterial->GetGIRLMaterial()->GetGIRLMaterialTypeID();
	m_pSViShaderID->SetInt( shaderID );

	for( int i=0; i < MAX_RENDERTARGET; i++ )
	{
		CGAttribute attr = pMaterial->GetAttribute( i );
		if( attr.IsConstant() )
		{
			m_pVShaderParams[i]->SetFloatVector( D3DXVECTOR4(attr.constant.c[0], attr.constant.c[1], attr.constant.c[2], attr.constant.alpha) );
			m_pSRVShaderParams[i]->SetResource( NULL );
		}
		else
		{
			m_pVShaderParams[i]->SetFloatVector( D3DXVECTOR4(-9999, -9999, -9999, -9999) );

			m_pSRVShaderParams[i]->SetResource( attr.texture.pTexSRV );
			// DWT

			
		}
	}
}

VOID
GShader::SetQuadTexSRV( ID3D10ShaderResourceView* pSRV )
{
	m_pSRVQuad->SetResource( pSRV );
}

VOID GShader::SetWorldMatrix( float* pMatObjectToWorld )
{
	m_pSVmatWorld->SetMatrix( pMatObjectToWorld );
}

ID3D10EffectTechnique*
GShader::GetTechnique( const char* szTechnique )
{
	ID3D10EffectTechnique* ret = m_pEffectMain->GetTechniqueByName( szTechnique );
	if( !ret )
		ret = m_pEffectDWT->GetTechniqueByName( szTechnique );

	/*
	if( !strcmp("FillMRT", szTechnique) )
		return m_pTechniqueFillMRT;
	if( !strcmp("DrawQuad", szTechnique) )
		return m_pTechniqueDrawQuad;
	if( !strcmp("DrawSimpleQuad", szTechnique) )
		return m_pTechniqueDrawSimpleQuad;
	if( !strcmp("DepthRender", szTechnique) )
		return m_pTechniqueDepthRender;
	*/

	return ret;
}

VOID GShader::SetLighting( GLight* pLight, const D3DXMATRIX* pMatView, UINT iLightFlag )
{
	m_pSViLightFlag->SetInt( iLightFlag );

	if( !pLight ) return;

	if( pLight->HasShadow() )
	{
		m_pSViShadowFlag->SetInt( 1 );
		m_pSVvShadowColor->SetFloatVector( pLight->GetShadowColor() );		
		m_pSRVShadowMap->SetResource( pLight->GetSoftShadowMap()->GetDepthSRV() );
	
		//m_pSRVShadowMap->SetResource( pLight->GetShadowMapSRV() );
		D3DXMATRIX matView, matProj, matViewProj;
		matView = pLight->GetMatView();
		matProj = pLight->GetMatProj();
		D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
		m_pSVmatShadowInv->SetMatrix( matViewProj );
		//m_pSVmatShadowInv->SetMatrix( pLight->GetMatView() );
		m_pSVfLightFar->SetFloat( pLight->GetFarClip() );
		m_pSVfLightNear->SetFloat( pLight->GetNearClip() );
	}
	else
	{
		m_pSViShadowFlag->SetInt( 0 );
		m_pSRVShadowMap->SetResource( NULL );
	}

	GAmbientLight* pAmbientLight = 0;
	GPointLight* pPointLight = 0;
	GSpotLight* pSpotLight = 0;

	switch( pLight->GetType() )
	{
	case MAYA_AMBIENT:	
		pAmbientLight = (GAmbientLight*)pLight;
		m_pSVfAmbientShade->SetFloat( pAmbientLight->GetAmbientShade() );
		break;

	case MAYA_POINT:
		pPointLight = (GPointLight*)pLight;
		m_pSVvLightPos->SetFloatVector( pPointLight->GetPosition() );
		m_pSViDecay->SetInt( pPointLight->GetDecay() );
		break;

	case MAYA_DIRECTIONAL:
		m_pSVvLightDir->SetFloatVector( ((GDirectionalLight*)pLight)->GetWorldDirection() );
		break;

	case MAYA_SPOT:
		pSpotLight = (GSpotLight*)pLight;
		m_pSVvLightDir->SetFloatVector( pSpotLight->GetWorldDirection() );
		m_pSVvLightPos->SetFloatVector( pSpotLight->GetPosition() );
		m_pSVfCosPenumbra->SetFloat( pSpotLight->GetCosPenumbra() );
		m_pSVfCosUmbra->SetFloat( pSpotLight->GetCosUmbra() );
		m_pSVfDropOff->SetFloat( pSpotLight->GetFallOff() );
		m_pSViDecay->SetInt( pSpotLight->GetDecay() );
		break;

	case MAYA_AREA:
		break;
	}
	m_pSVvLightColor->SetFloatVector( pLight->GetColorIntensity() );
}

VOID GShader::SetDWTSRVs( ID3D10ShaderResourceView* pSourceTexSRV, ID3D10ShaderResourceView* pHighTexSRV, ID3D10ShaderResourceView* pLowTexSRV, UINT widthVP, UINT heightVP )
{
	m_pSRVDWTTexture->SetResource( pSourceTexSRV );
	m_pSRVDWTHighPassTexture->SetResource( pHighTexSRV );
	m_pSRVDWTLowPassTexture->SetResource( pLowTexSRV );
	m_pSVfWidth->SetInt( widthVP );
	m_pSVfHeight->SetInt( heightVP );
}

VOID GShader::SetDWTSRVs( ID3D10ShaderResourceView* pScaleSRV, ID3D10ShaderResourceView* pCoeffsSRV, ID3D10ShaderResourceView* pIndicesSRV, UINT iNumCoeffs, UINT iNumLevel, UINT iSquareIndex )
{
	m_pSRVDWTScale->SetResource( pScaleSRV );
	m_pSRVDWTCoeffs->SetResource( pCoeffsSRV );
	m_pSRVDWTIndices->SetResource( pIndicesSRV );
	m_pSViNumCoeffs->SetInt( iNumCoeffs );
	m_pSViNumLevel->SetInt( iNumLevel );
	m_pSViSquareIndex->SetInt( iSquareIndex );
}