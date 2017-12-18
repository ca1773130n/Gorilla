#pragma once

#include "stdafx.h"
#include <string>
#include "ou_thread.h"
#include "bitmask.h"
#include "SoftShadowMap.h"

#include <set>

#define MAYA_AMBIENT		0
#define MAYA_POINT			1
#define MAYA_DIRECTIONAL	2
#define MAYA_SPOT			3
#define MAYA_AREA			4
#define MAYA_VOLUME			5
#define MAYA_ENVIRONMENT	6

#define SHADOWMAP_SIZE 1024

using namespace std;
using namespace openutils;

class GLight
{
protected:

	BOOL			bDirty;
	BOOL 			bSelected;
	BOOL			bHasKey;
	BOOL			bInBaseTex;				
	BOOL			bHasShadow;
	BOOL			bNeedDepthRendering;
	BOOL			bNeedLinkUpdate;

	UINT			iType;		
	string			szName;			
	
	D3DXVECTOR3		vColor;
	D3DXVECTOR3		vShadowColor;
	FLOAT			fIntensity;		
	FLOAT			fFar;
	FLOAT			fNear;

	D3DXMATRIX		matShadowInv;
	D3DXMATRIX		matView;
	D3DXMATRIX		matProj;
	Mutex*			pMutex;

	BOOL			bLinkInfoInclusive;
	BOOL			bLinkInfoAll;
	Bitmask			LinkBitmask;
	int				iLinkMapSize;
	ULONG			iNumTotalObject;

	ID3D10Texture2D*	pShadowMaskTex;
	ID3D10Texture2D*	pLinkMaskTex;
	ID3D10ShaderResourceView* pSRVShadowMask;
	ID3D10ShaderResourceView* pSRVLinkMask;
	ID3D10DepthStencilView* pDSVShadowMask;
	ID3D10RenderTargetView* pRTVShadowMask;
	//GLenum			enLinkMaskTex;
	//GLenum			enShadowMaskTex;
	SSMap*			pSoftShadowMap;

public:


	GLight( VOID );
	GLight( char** token, int type );
	~GLight( VOID );

	string&			GetName( VOID )								{ return szName; }

	UINT			GetType( VOID )								{ return iType; }

	D3DXVECTOR3		GetColorIntensity( VOID )					{ return vColor * fIntensity; }

	D3DXVECTOR3&	GetShadowColor( VOID )						{ return vShadowColor; }

	ID3D10DepthStencilView* GetDSV( VOID )						{ return pDSVShadowMask; }

	ID3D10RenderTargetView* GetRTV( VOID )						{ return pRTVShadowMask; }

	ID3D10ShaderResourceView* GetShadowMapSRV( VOID )			{ return pSRVShadowMask; }

	ID3D10Texture2D* GetShadowMapTex( VOID )					{ return pShadowMaskTex; }

	SSMap*			GetSoftShadowMap( VOID )					{ return pSoftShadowMap; }

	FLOAT			GetFarClip( VOID )							{ return fFar; }

	FLOAT			GetNearClip( VOID )							{ return fNear; }

	SSMap*			GetSSMap( VOID )							{ return pSoftShadowMap; }

	VOID			SetColorIntensity( float i )				{ fIntensity = i; }

	//VOID			SetLinkMaskTex( GLenum tex );

	VOID			SetType( UINT type )						{ iType = type; }

	VOID Setup( const D3DXMATRIX& matViewInv, UINT& iLightFlag );

	VOID			Modify( char** token );

	HRESULT SetToDepthRendering( ID3D10Device* pd3dDevice, FLOAT fOrtho, FLOAT fAspect, D3DXVECTOR3 vCenter, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );

	VOID			InBaseTex( BOOL bInBase )					{ bInBaseTex = bInBase; }

	BOOL			IsInBaseTex( VOID )							{ return bInBaseTex; }

	BOOL			HasShadow( VOID )							{ return bHasShadow; }

	BOOL			NeedDepthRendering( VOID )					{ return bNeedDepthRendering; }

	BOOL			NeedLinkUpdate( VOID )						{ return bNeedLinkUpdate; }

	VOID			SetNeedDepthRendering( BOOL bSet )			{ bNeedDepthRendering = bSet; }

	D3DXMATRIX&		GetMatView( VOID )							{ return matView; }

	D3DXMATRIX&		GetMatProj( VOID )							{ return matProj; }

	D3DXMATRIX&		GetMatShadowInv( VOID )						{ return matShadowInv; }

	VOID			SetMatShadowInv( D3DXMATRIX& mat )			{ matShadowInv = mat; }

	VOID			SetMutex( Mutex* pM )						{ pMutex = pM; }

	//VOID			GenerateLinkMap( UINT iRenderWidth, UINT iRenderHeight, GLenum link, LTGA* pObjectMap );

	VOID			ParseLinkData( Thread* pThread, char** token );

	size_t			GetNumLinkedObject( VOID )					{ return iLinkMapSize; }

	//GLenum			GetLinkMaskTex( VOID )						{ return enLinkMaskTex; }

	//GLenum			GetShadowMaskTex( VOID )					{ return enShadowMaskTex; }

	//VOID			SetShadowMaskTex( UINT tex )				{ enShadowMaskTex = tex; }

	BOOL			IsDirty( VOID )								{ return bDirty; }

	VOID			SetDirty( BOOL bD )							{ bDirty = bD; }

	BOOL			IsLinkInfoInclusive( VOID )					{ return bLinkInfoInclusive; }

	BOOL			IsLinkInfoAll( VOID )						{ return bLinkInfoAll; }
};

class GPointLight : public GLight
{
protected:

	int				iDecay;
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3		vTransPosition;

public:

	GPointLight( VOID ) {}
	GPointLight( char** token, int type );

	VOID Setup( const D3DXMATRIX& matViewInv );

	VOID			Modify( char** token );

	D3DXVECTOR3&	GetPosition( VOID )							{ return vPosition; }

	D3DXVECTOR3&	GetTransPosition( VOID )					{ return vTransPosition; }

	int				GetDecay( VOID )							{ return iDecay; }

	VOID			SetPosition( D3DXVECTOR3 vPos )				{ vPosition = vPos; }
};

class GAmbientLight : public GPointLight
{
protected:

	FLOAT			fAmbientShade;

public:

	GAmbientLight( VOID ) {}
	GAmbientLight( char** token, int type );

	VOID Setup( const D3DXMATRIX& matViewInv );

	VOID			Modify( char** token );

	FLOAT			GetAmbientShade( VOID )						{ return fAmbientShade; }
};

class GDirectionalLight : public GLight
{
protected:

	D3DXMATRIX		matRotate;	
	D3DXVECTOR3		vDirection;
	D3DXVECTOR3		vTransDirection;
	D3DXVECTOR3		vWorldDirection;

public:

	GDirectionalLight( VOID ) {}
	GDirectionalLight( char** token, int type );

	VOID Setup( const D3DXMATRIX& matViewInv );

	VOID			Modify( char** token );

	D3DXVECTOR3&	GetDirection( VOID )						{ return vDirection; }

	D3DXVECTOR3&	GetTransDirection( VOID )					{ return vTransDirection; }

	D3DXVECTOR3&	GetWorldDirection( VOID )					{ return vWorldDirection; }

	VOID			SetDirection( D3DXVECTOR3 vDir )			{ vDirection = vDir; }

	VOID			SetWorldDirection( D3DXVECTOR3 vWorldDir )	{ vWorldDir = vWorldDirection; }

	VOID			SetToDepthRendering( FLOAT fOrtho, FLOAT fAspect, D3DXVECTOR3 vCenter );
};

class GSpotLight : public GDirectionalLight
{
protected:

	int				iDecay;
	D3DXVECTOR3		vPosition;
	D3DXVECTOR3		vTransPosition;
	FLOAT 			fAmbientShade;	
	FLOAT			fAngleTheta;
	FLOAT			fTransAngleTheta;
	FLOAT			fAnglePhi;
	FLOAT			fTransAnglePhi;
	FLOAT			fFalloff;
	FLOAT			fCosPenumbra;
	FLOAT			fCosUmbra;



public:



	GSpotLight( VOID ) {}
	GSpotLight( char** token, int type );

	VOID Setup( const D3DXMATRIX& matViewInv );

	VOID			Modify( char** token );

	D3DXVECTOR3&		GetPosition( VOID )							{ return vPosition; }

	D3DXVECTOR3&		GetTransPosition( VOID )					{ return vTransPosition; }

	int				GetDecay( VOID )							{ return iDecay; }

	VOID			SetPosition( D3DXVECTOR3 vPos )				{ vPosition = vPos; }

	FLOAT			GetCosPenumbra( VOID )						{ return fCosPenumbra; }

	FLOAT			GetCosUmbra( VOID )							{ return fCosUmbra; }

	FLOAT			GetFallOff( VOID )							{ return fFalloff; }

	HRESULT SetToDepthRendering( ID3D10Device* pd3dDevice, FLOAT fOrtho, FLOAT fAspect, D3DXVECTOR3 vCenter, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );
};