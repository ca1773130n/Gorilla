#pragma once

#include "stdafx.h"
#include "GCamera.h"
#include "GLight.h"
#include "GMesh.h"
#include "MeshLoader10.h"
#include "GShader.h"

#include <vector>
using namespace std;

struct StaticVertex
{
	float pos[3];
	float normal[3];
	float tangent[3];
	float tc[2];
};

class CGRenderer
{
public:
	CGRenderer();
	~CGRenderer();

	HRESULT				Initialize( UINT width, UINT height );

	HRESULT				MakeMainRenderTargets( ID3D10Device* pd3dDevice, UINT iWidth, UINT iHeight, UINT iSSMultiplier );

	HRESULT				MakeOtherRenderTargets( ID3D10Device* pd3dDevice );

	VOID				CleanUp( VOID );

	VOID				LoadCacheFile( const CString& lpszCacheFilename );

	VOID				SetupScene( VOID );

	VOID				RenderSubset( UINT iSubset );

	VOID				RenderDepthMap( GLight* pLight, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj );

	VOID				RenderDWT( );

	VOID ApplyLight( GLight* pLight, ID3D10RenderTargetView** pOldRTV, ID3D10DepthStencilView* pOldDSV );

	VOID				DrawQuad( VOID );

	/************************************************************************/
	/* DXUT �ݹ� �ڵ鷯 �Լ���                                              */
	/************************************************************************/

	/// ��ġ�� ���� ȯ���� �������� ���� �������� �Ǵ�
	bool				IsDeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BufferFormat, bool bWindowed, VOID* pUserContext );

	/// ��ġ �������� �ڵ鷯
	HRESULT				OnCreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc, VOID* pUserContext );

	/// ���� ü�� ������¡ �ڵ鷯
	HRESULT				OnResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBufferSurfaceDesc, VOID* pUserContext );

	/// ���� ü�� ������ �ڵ鷯
	VOID				OnReleasingSwapChain( VOID* pUserContext );

	/// ��ġ ���½��� �ڵ鷯
	HRESULT				OnResetDevice( ID3D10Device* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, VOID* pUserContext );

	/// ������ �̵��� �ڵ鷯
	VOID				OnFrameMove( double fTime, float fElapsedTime, VOID* pUserContext );

	/// ������ �������� �ڵ鷯
	VOID				OnFrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, VOID* pUserContext );

	/// �⺻ �޽��� �ڵ鷯
	LRESULT				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, VOID* pUserContext );

	/// ��ġ�� �Ҿ������� �ڵ鷯
	VOID				OnLostDevice( VOID* pUserContext );

	/// ��ġ �ı����� �ڵ鷯
	VOID				OnDestroyDevice( VOID* pUserContext );

	/// CustomUI �̺�Ʈ �ڵ鷯
	VOID				OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, VOID* pUserContext );

	/// Ű���� �̺�Ʈ �ڵ鷯
	VOID				OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, VOID* pUserContext );

	/// ��ġ ������ ����ɶ��� �ڵ鷯
	bool				ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, VOID* pUserContext );

	/// ĳ�� ���� ���������� �ڵ鷯
	VOID				OnFileOpen( LPCWSTR lpszCacheFilename );

	/*
	/// ��ġ�� caps�� �˻��Ͽ� ������ ȯ���� �������� ���� �������� �Ǵ�
	BOOL				IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );

	/// ��ġ ������ ����ɶ��� �ڵ鷯
	BOOL				ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

	/// ��ġ �������� �ڵ鷯
	HRESULT				OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );

	/// ��ġ ���½��� �ڵ鷯
	HRESULT				OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );

	/// ������ �̵��� �ڵ鷯
	VOID				OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );

	/// ������ �������� �ڵ鷯
	VOID				OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );

	/// �⺻ �޽��� �ڵ鷯
	LRESULT				MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext );

	/// ��ġ�� �Ҿ������� �ڵ鷯
	VOID				OnLostDevice( void* pUserContext );

	/// ��ġ �ı����� �ڵ鷯
	VOID				OnDestroyDevice( void* pUserContext );

	/// CustomUI �̺�Ʈ �ڵ鷯
	VOID				OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
	*/

	/************************************************************************/
	/* Get / Set Stuffs                                                     */
	/************************************************************************/
	BOOL				IsRenderStart( VOID )					{ return m_bRenderStart; }

	BOOL				CacheLoaded( VOID )						{ return m_bCacheLoaded; }

	UINT				GetWidth( VOID )						{ return m_iRenderWidth; }

	UINT				GetHeight( VOID )						{ return m_iRenderHeight; }

	GCamera*			GetCurrentCam( VOID )					{ if( m_Cameras.size() == 0 ) { m_Cameras.push_back(*(m_pCurrentCam = new GCamera())); } return &m_Cameras[m_iCurCamIndex]; }

	vector<GLight*>&	GetLights( VOID )						{ return m_Lights; }

	VOID				SetCacheLoaded( BOOL bLoaded )			{ m_bCacheLoaded = bLoaded; }

	VOID				SetRenderStart( VOID )					{ m_bRenderStart = TRUE; }

	VOID				SetCacheDirty( BOOL bDirty )			{ m_bMakeBuffer = bDirty; }

	VOID				SetAddTexFlag( BOOL bFlag )				{ m_bAddTexture = bFlag; m_bNeedRedraw = TRUE; }

	VOID				SetMulTexFlag( BOOL bFlag )				{ m_bMultiplyTexture = bFlag; m_bNeedRedraw = TRUE; }

	//VOID				SetAddTexIntensity( FLOAT fIntensity )	{ m_Shader.SetAddTexIntensity( fIntensity ); m_bNeedRedraw = TRUE; }

	//VOID				SetMulTexIntensity( FLOAT fIntensity )	{ m_Shader.SetMulTexIntensity( fIntensity ); m_bNeedRedraw = TRUE; }

	VOID				SetNeedRedraw( BOOL bRedraw )			{ m_bNeedRedraw = bRedraw; }

	VOID				SetFirstRender( BOOL bFirstRender )		{ m_bFirstRender = bFirstRender; }

	BOOL				IsNeedRedraw( VOID )					{ return m_bNeedRedraw; }

	VOID				SetViewFlag( ULONG iFlag )				{ m_iViewFlag = iFlag; }

	Bitmask*			GetBitmaskLightsInBaseTex( VOID )		{ return &m_BitmaskLightsInBaseTex; }

	Bitmask*			GetBitmaskLink( VOID )					{ return &m_BitmaskObjectLink; }

	VOID				SetCameraChanged( BOOL bChanged )		{ m_bCameraChanged = bChanged; }

	VOID				Lock( VOID )							{ m_bLocked = TRUE; }

	VOID				Unlock( VOID )							{ m_bLocked = FALSE; }

private:

	/************************************************************************/
	/* MFC, ���� ����                                                       */
	/************************************************************************/
	HWND							m_hWnd;
	TCHAR							m_lpszWorkingDir[MAX_PATH];

	/************************************************************************/
	/* Direct3D 10 ����                                                     */
	/************************************************************************/
	ID3D10Device*					m_pd3dDevice;
	ID3D10EffectTechnique*			m_pTechnique;
	D3D10_DRIVER_TYPE				m_driverType;
	IDXGISwapChain*					m_pSwapChain;
	ID3D10DepthStencilState*		m_pDepthStencilState;

	/************************************************************************/
	/* Deferred Lighting                                                    */
	/************************************************************************/
	ID3D10RenderTargetView*			m_pRTVShaderParams[MAX_RENDERTARGET];
	ID3D10RenderTargetView*			m_pRTVShadowRenderTarget;	
	ID3D10RenderTargetView*			m_pRTVNormalDepth[2];
	ID3D10RenderTargetView*			m_pRTVMicroRenders[MAX_RENDERTARGET];
	ID3D10RenderTargetView*			m_pRTVMicroRender[MAX_RENDERTARGET];
	
	ID3D10DepthStencilView*			m_pShadowDSV;
	ID3D10DepthStencilView*			m_pMainDSV;
	ID3D10DepthStencilView*			m_pDSVMicroRender;

	ID3D10RenderTargetView*			m_pRTVOffScreen;

	ID3D10ShaderResourceView*		m_pSRVShaderParams[MAX_RENDERTARGET];
	ID3D10ShaderResourceView*		m_pSRVShadowRenderTarget;
	ID3D10ShaderResourceView*		m_pSRVRenderTargetTex;
	ID3D10ShaderResourceView*		m_pSRVOffScreenRenderTargetTex;
	ID3D10ShaderResourceView*		m_pSRVDepth;
	ID3D10ShaderResourceView*		m_pSRVMicroRenders[MAX_RENDERTARGET];
	ID3D10ShaderResourceView*		m_pSRVMicroRender[MAX_RENDERTARGET];

	ID3D10Texture2D*				m_pTexShaderParams[MAX_RENDERTARGET];
	ID3D10Texture2D*				m_pTexRenderTarget[MAX_RENDERTARGET];
	ID3D10Texture2D*				m_pTexMainShadowDepth;
	ID3D10Texture2D*				m_pTexMainDepth;
	ID3D10Texture2D*				m_pTexShadowRenderTarget;
	ID3D10Texture2D*				m_pTexMainRenderTarget;
	ID3D10Texture2D*				m_pTexOffScreenRenderTarget;
	ID3D10Texture2D*				m_pTexNormalDepth[2];
	ID3D10Texture2D*				m_pTexNormalDepthST[2];
	ID3D10Texture2D*				m_pTexMicroRenders[MAX_RENDERTARGET];
	ID3D10Texture2D*				m_pTexMicroRender[MAX_RENDERTARGET];
	ID3D10Texture2D*				m_pTexMicroRenderDepth;


	ID3D10Texture2D*				m_pTexRenderTargetST[MAX_RENDERTARGET];

	IDirect3DVertexDeclaration9*	m_pDeclMain;
	IDirect3DVertexDeclaration9*	m_pDeclQuad;

	IDirect3DPixelShader9*			m_pPSLight;
	IDirect3DPixelShader9*			m_pPSFillMRT;
	IDirect3DPixelShader9*			m_pPSGrid;
	IDirect3DPixelShader9*			m_pPSMask;
	IDirect3DPixelShader9*			m_pPSLightNoAA;
	IDirect3DVertexShader9*			m_pVSLight;
	IDirect3DVertexShader9*			m_pVSFillMRT;

	/************************************************************************/
	/* DWT                                                                  */
	/************************************************************************/
	ID3D10RenderTargetView*			m_pRTVDWTThin[12][4];
	ID3D10RenderTargetView*			m_pRTVDWTSquare[12][6];
	ID3D10Texture2D*				m_pTexDWTThin[12][4];
	ID3D10Texture2D*				m_pTexDWTSquare[12][6];
	ID3D10Texture2D*				m_pTexDWTThinST[12][4];
	ID3D10Texture2D*				m_pTexDWTSquareST[12][6];
	ID3D10ShaderResourceView*		m_pSRVDWTThin[12][4];
	ID3D10ShaderResourceView*		m_pSRVDWTSquare[12][6];
	ID3D10ShaderResourceView*		m_pSRVDWTThinST[12][4];
	ID3D10ShaderResourceView*		m_pSRVDWTSquareST[12][6];

	/************************************************************************/
	/* ���� ����                                                            */
	/************************************************************************/
	BOOL					m_bLocked;
	BOOL					m_bInitialized;
	BOOL					m_bFirstRender;
	BOOL					m_bNeedRedraw;
	BOOL					m_bNeedDepthCaching;
	BOOL					m_bMakeBuffer;
	BOOL					m_bSelectUpdate;
	BOOL					m_bRenderStart;
	BOOL					m_bSelectionChanged;
	BOOL					m_bAddTexture;
	BOOL					m_bMultiplyTexture;
	BOOL					m_bShadowFlag;
	BOOL					m_bCacheLoaded;	
	BOOL					m_bCameraChanged;
	ULONG					m_iViewFlag;

	/************************************************************************/
	/* Scene, Mesh ����	                                                    */
	/************************************************************************/
	CGGeometry				m_SceneGeometry;
	CMeshLoader10			m_MeshLoader;	
	CString					m_lpszSceneFilename;
	ID3DX10Mesh*			m_pQuadMesh;
	ID3DX10MeshBuffer*		m_pMVBQuad;
	ID3DX10MeshBuffer*		m_pMIBQuad;
	ID3D10Buffer*			m_pVBQuad;
	ID3D10Buffer*			m_pIBQuad;
	ID3D10InputLayout*		m_pVertexLayout;

	/************************************************************************/
	/* Camera, Film ����                                                    */
	/************************************************************************/
	vector<GCamera>			m_Cameras;
	GCamera*				m_pCurrentCam;
	UINT					m_iCurCamIndex;
	UINT					m_iRenderWidth;
	UINT					m_iRenderHeight;
	GCamera					m_VirtualCam;

	/************************************************************************/
	/* Light ����                                                           */
	/************************************************************************/
	UINT					m_iMaxLight;
	vector<GLight*>			m_Lights;
	Bitmask					m_BitmaskLightsInBaseTex;
	Bitmask					m_BitmaskObjectLink;
	UINT					m_iLightFlag;

	/************************************************************************/
	/* Shader ����                                                          */
	/************************************************************************/
	GShader					m_Shader;
};
