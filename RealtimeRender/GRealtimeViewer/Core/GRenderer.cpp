#include "stdafx.h"
#include "GRenderer.h"
#include "GGlobal.h"
#include "MainFrm.h"
#include "corona.h"
#include "half.h"

#define MICRO_RENDER_SIZE 16

/************************************************************************/
/* Global Variables                                                     */
/************************************************************************/

CRITICAL_SECTION	g_CS;
BOOL				bDebugMode = FALSE;
CGRenderer*			g_pRenderer = NULL;
float				ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; 

CGRenderer::CGRenderer()
{
	m_bFirstRender = FALSE;
	m_bMakeBuffer = FALSE;
	m_bCameraChanged = FALSE;
	m_pCurrentCam = NULL;
	m_bRenderStart = FALSE;

	for( int i=0; i < MAX_RENDERTARGET; i++ )
	{
		m_pTexShaderParams[i] = 0;
		m_pSRVShaderParams[i] = 0;
		m_pRTVShaderParams[i] = 0;
		m_pTexRenderTarget[i] = 0;
	}

	m_BitmaskLightsInBaseTex.Allocate( DEFAULT_NUMLIGHT );
	m_BitmaskLightsInBaseTex.AllClear();
	m_BitmaskObjectLink.Allocate( DEFAULT_NUMLIGHT );
	m_BitmaskObjectLink.AllClear();

	m_iCurCamIndex = 0;
	m_Cameras.clear();

	m_pRTVOffScreen = 0;
	m_pRTVShadowRenderTarget = 0;

	m_pTexMainDepth = 0;
	m_pMainDSV = 0;
	m_pTexMainShadowDepth = 0;
	m_pShadowDSV = 0;
	m_pTexMainRenderTarget = 0;
	m_pSRVOffScreenRenderTargetTex = 0;
	m_pRTVOffScreen = 0;
}

CGRenderer::~CGRenderer()
{

}

HRESULT
CGRenderer::MakeMainRenderTargets( ID3D10Device* pd3dDevice, UINT iWidth, UINT iHeight, UINT iSSMultiplier )
{
	HRESULT hr;

	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize			= 1;
	texDesc.BindFlags			= D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Width				= iWidth * iSSMultiplier;
	texDesc.Height				= iHeight * iSSMultiplier;
	texDesc.MipLevels			= 1;
	texDesc.MiscFlags			= NULL;
	texDesc.SampleDesc.Count	= 4;
	texDesc.SampleDesc.Quality	= 8;
	texDesc.Usage				= D3D10_USAGE_DEFAULT;
	texDesc.CPUAccessFlags		= NULL;
	
	m_iRenderWidth = iWidth;
	m_iRenderHeight = iHeight;

	//////////////////////////////////////////////////////////////////////////
	// Check the capability of MSAA

	int m_msaa = 8;
	for(UINT i = 2; i <= D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT; i++)
	{
		UINT quality[2] = {0, 0};

		if(SUCCEEDED(pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R32G32B32A32_FLOAT, i, &quality[0])) && quality[0] > 0
			&& SUCCEEDED(pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R32G32B32A32_FLOAT, i, &quality[1])) && quality[1] > 0)
		{
			texDesc.SampleDesc.Count = i;
			texDesc.SampleDesc.Quality = std::min<UINT>(quality[0] - 1, quality[1] - 1);

			if(i >= m_msaa) break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	texDesc.SampleDesc.Count   = 4;
	texDesc.SampleDesc.Quality = 16;	

	D3D10_RENDER_TARGET_VIEW_DESC descRT;
	ID3D10Texture2D* pTexTemp;
	ID3D10RenderTargetView* pRTVTemp;
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &pTexTemp) );
	V( pd3dDevice->CreateRenderTargetView(pTexTemp, NULL, &pRTVTemp) );
	pRTVTemp->GetDesc( &descRT );
	pRTVTemp->Release();
	pTexTemp->Release();

	descRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS;
	descRT.Texture2D.MipSlice = 0;

	D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
	memset( &descSRV, 0, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC) );
	descSRV.Format						= texDesc.Format;
	descSRV.ViewDimension				= D3D10_SRV_DIMENSION_TEXTURE2DMS;
	descSRV.Texture2D.MostDetailedMip	= 0;
	descSRV.Texture2D.MipLevels			= 1;

	for( int i=0; i < 8; i++ )
	{
		SAFE_RELEASE( m_pTexShaderParams[i] );
		SAFE_RELEASE( m_pSRVShaderParams[i] );
		SAFE_RELEASE( m_pRTVShaderParams[i] );

		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexShaderParams[i]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexShaderParams[i], &descSRV, &m_pSRVShaderParams[i]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexShaderParams[i], &descRT, &m_pRTVShaderParams[i]) );		
	}
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexNormalDepth[0]) );
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexNormalDepth[1]) );
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	texDesc.Usage = D3D10_USAGE_STAGING;
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexNormalDepthST[0]) );
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexNormalDepthST[1]) );
	V( pd3dDevice->CreateRenderTargetView(m_pTexNormalDepth[0], NULL, &m_pRTVNormalDepth[0]) );	
	V( pd3dDevice->CreateRenderTargetView(m_pTexNormalDepth[1], NULL, &m_pRTVNormalDepth[1]) );	


	D3D10_TEXTURE2D_DESC d;
	d.CPUAccessFlags	= 0;             
	d.Usage				= D3D10_USAGE_DEFAULT;
	d.Width				= iWidth;                                       
	d.Height			= iHeight;
	d.MipLevels			= 1;
	d.Format			= DXGI_FORMAT_D32_FLOAT;
	d.ArraySize			= 1;
	d.SampleDesc.Count	= 4;
	d.SampleDesc.Quality= 16;
	d.BindFlags			= D3D10_BIND_DEPTH_STENCIL;
	d.MiscFlags			= 0;

	SAFE_RELEASE( m_pTexMainDepth );
	V( pd3dDevice->CreateTexture2D(&d, 0, &m_pTexMainDepth) );

	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	memset( &descDSV, 0, sizeof(D3D10_DEPTH_STENCIL_VIEW_DESC) );
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
	descSRV.Texture2D.MostDetailedMip = 0;
	descSRV.Texture2D.MipLevels = 1;

	SAFE_RELEASE( m_pMainDSV );
	V( pd3dDevice->CreateDepthStencilView(m_pTexMainDepth, &descDSV, &m_pMainDSV) );

	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	d.Width				= SHADOWMAP_SIZE;
	d.Height			= SHADOWMAP_SIZE;
	d.MipLevels			= 0;
	d.Format			= DXGI_FORMAT_D32_FLOAT;
	d.SampleDesc.Count	= 1;
	d.SampleDesc.Quality= 0;

	SAFE_RELEASE( m_pTexMainShadowDepth );
	SAFE_RELEASE( m_pShadowDSV );
	V( pd3dDevice->CreateTexture2D(&d, 0, &m_pTexMainShadowDepth) );
	V( pd3dDevice->CreateDepthStencilView(m_pTexMainShadowDepth, &descDSV, &m_pShadowDSV) );


	texDesc.Usage				= D3D10_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags		= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;	
	texDesc.MipLevels			= 0;
	descSRV.Format						= texDesc.Format;
	descSRV.ViewDimension				= D3D10_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MostDetailedMip	= 0;
	descSRV.Texture2D.MipLevels			= 0;

	SAFE_RELEASE( m_pTexMainRenderTarget );
	SAFE_RELEASE( m_pSRVOffScreenRenderTargetTex );
	SAFE_RELEASE( m_pRTVOffScreen );
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexMainRenderTarget) );
	V( pd3dDevice->CreateShaderResourceView(m_pTexMainRenderTarget, NULL, &m_pSRVOffScreenRenderTargetTex) );
	V( pd3dDevice->CreateRenderTargetView(m_pTexMainRenderTarget, NULL, &m_pRTVOffScreen) );

	d.Width = MICRO_RENDER_SIZE;
	d.Height = MICRO_RENDER_SIZE;
	V( pd3dDevice->CreateTexture2D(&d, 0, &m_pTexMicroRenderDepth) );
	V( pd3dDevice->CreateDepthStencilView(m_pTexMicroRenderDepth, &descDSV, &m_pDSVMicroRender) );

	return hr;
}

HRESULT
CGRenderer::MakeOtherRenderTargets( ID3D10Device* pd3dDevice )
{
	HRESULT hr;

	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;	
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.BindFlags			= D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texDesc.Usage				= D3D10_USAGE_DEFAULT;
	texDesc.CPUAccessFlags		= 0;	
	texDesc.MiscFlags			= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;

	D3D10_SHADER_RESOURCE_VIEW_DESC descSRV;
	memset( &descSRV, 0, sizeof(D3D10_SHADER_RESOURCE_VIEW_DESC) );
	descSRV.Format						= texDesc.Format;
	descSRV.ViewDimension				= D3D10_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MostDetailedMip	= 0;
	descSRV.Texture2D.MipLevels			= 1;
	
	//////////////////////////////////////////////////////////////////////////
	// DWT

	/*
	texDesc.Width = 2048;
	texDesc.Height = 4096;	

	int nLevels = 11;
	for( int i=0; i < nLevels; i++ )
	{
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTThin[i][0]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTThin[i][1]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTThin[i][2]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTThin[i][3]) );

		texDesc.BindFlags = 0;
		texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		texDesc.Usage = D3D10_USAGE_STAGING;

		texDesc.BindFlags			= D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		texDesc.Usage				= D3D10_USAGE_DEFAULT;
		texDesc.CPUAccessFlags		= 0;	

		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTThin[i][0], NULL, &m_pSRVDWTThin[i][0]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTThin[i][1], NULL, &m_pSRVDWTThin[i][1]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTThin[i][2], NULL, &m_pSRVDWTThin[i][2]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTThin[i][3], NULL, &m_pSRVDWTThin[i][3]) );

		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTThin[i][0], NULL, &m_pRTVDWTThin[i][0]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTThin[i][1], NULL, &m_pRTVDWTThin[i][1]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTThin[i][2], NULL, &m_pRTVDWTThin[i][2]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTThin[i][3], NULL, &m_pRTVDWTThin[i][3]) );

		texDesc.Height /= 2;

		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquare[i][0]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquare[i][1]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquare[i][2]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquare[i][3]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquare[i][4]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquare[i][5]) );

		texDesc.BindFlags = 0;
		texDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		texDesc.Usage = D3D10_USAGE_STAGING;

		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquareST[i][0]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquareST[i][1]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquareST[i][2]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquareST[i][3]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquareST[i][4]) );
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexDWTSquareST[i][5]) );

		texDesc.BindFlags			= D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
		texDesc.Usage				= D3D10_USAGE_DEFAULT;
		texDesc.CPUAccessFlags		= 0;	

		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTSquare[i][0], NULL, &m_pSRVDWTSquare[i][0]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTSquare[i][1], NULL, &m_pSRVDWTSquare[i][1]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTSquare[i][2], NULL, &m_pSRVDWTSquare[i][2]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTSquare[i][3], NULL, &m_pSRVDWTSquare[i][3]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTSquare[i][4], NULL, &m_pSRVDWTSquare[i][4]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexDWTSquare[i][5], NULL, &m_pSRVDWTSquare[i][5]) );

		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTSquare[i][0], NULL, &m_pRTVDWTSquare[i][0]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTSquare[i][1], NULL, &m_pRTVDWTSquare[i][1]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTSquare[i][2], NULL, &m_pRTVDWTSquare[i][2]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTSquare[i][3], NULL, &m_pRTVDWTSquare[i][3]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTSquare[i][4], NULL, &m_pRTVDWTSquare[i][4]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexDWTSquare[i][5], NULL, &m_pRTVDWTSquare[i][5]) );

		texDesc.Width /= 2;
	}
	*/

	texDesc.Format				= DXGI_FORMAT_R32_FLOAT;
	texDesc.Width				= SHADOWMAP_SIZE;
	texDesc.Height				= SHADOWMAP_SIZE;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.MipLevels			= 0;

	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexShadowRenderTarget) );
	V( pd3dDevice->CreateShaderResourceView(m_pTexShadowRenderTarget, NULL, &m_pSRVShadowRenderTarget) );
	V( pd3dDevice->CreateRenderTargetView(m_pTexShadowRenderTarget, NULL, &m_pRTVShadowRenderTarget) );

	texDesc.Format				= DXGI_FORMAT_R32_FLOAT;
	texDesc.Width				= SHADOWMAP_SIZE;
	texDesc.Height				= SHADOWMAP_SIZE;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.MipLevels			= 0;

	SAFE_RELEASE( m_pTexShadowRenderTarget );
	SAFE_RELEASE( m_pSRVShadowRenderTarget );
	SAFE_RELEASE( m_pRTVShadowRenderTarget );
	V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexShadowRenderTarget) );
	V( pd3dDevice->CreateShaderResourceView(m_pTexShadowRenderTarget, NULL, &m_pSRVShadowRenderTarget) );
	V( pd3dDevice->CreateRenderTargetView(m_pTexShadowRenderTarget, NULL, &m_pRTVShadowRenderTarget) );

	//////////////////////////////////////////////////////////////////////////
	// Micro rendering
	/*
	texDesc.Width				= 2048;
	texDesc.Height				= 2048;
	texDesc.BindFlags			= D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
	texDesc.Format				= DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.ArraySize			= m_iRenderWidth * m_iRenderHeight * 32 / 2048 / 2048 + 1; 
	texDesc.MipLevels			= 0;
	texDesc.MiscFlags			= 0;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D10_USAGE_DEFAULT;
	texDesc.CPUAccessFlags		= 0;

	for( int i=0; i < MAX_RENDERTARGET; i++ )
	{
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexMicroRenders[i]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexMicroRenders[i], NULL, &m_pSRVMicroRenders[i]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexMicroRenders[i], NULL, &m_pRTVMicroRenders[i]) );
	}

	texDesc.Width				= MICRO_RENDER_SIZE;
	texDesc.Height				= MICRO_RENDER_SIZE;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_R32G32B32A32_FLOAT;

	for( int i=0; i < MAX_RENDERTARGET; i++ )
	{
		V( pd3dDevice->CreateTexture2D(&texDesc, NULL, &m_pTexMicroRender[i]) );
		V( pd3dDevice->CreateShaderResourceView(m_pTexMicroRender[i], NULL, &m_pSRVMicroRender[i]) );
		V( pd3dDevice->CreateRenderTargetView(m_pTexMicroRender[i], NULL, &m_pRTVMicroRender[i]) );
	}
	*/

	return hr;
}

HRESULT CGRenderer::Initialize( UINT width, UINT height )
{
	HRESULT hr = S_OK;

	m_hWnd = ((CMainFrame *)AfxGetMainWnd())->GetActiveView()->m_hWnd;	
	V_RETURN( DXUTSetWindow(m_hWnd, m_hWnd, m_hWnd) );	
	V_RETURN( DXUTCreateDevice(TRUE, FALSE) );	
	m_pd3dDevice = DXUTGetD3D10Device();
	
	// Initialze Shader Object
	V_RETURN( m_Shader.Init(m_pd3dDevice) );

	MakeMainRenderTargets( m_pd3dDevice, width, height, 1 );
	MakeOtherRenderTargets( m_pd3dDevice );

	D3D10_INPUT_ELEMENT_DESC iaElemDesc[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D10_PASS_DESC PassDesc;
	ID3D10EffectPass *p = m_Shader.GetDWTEffect()->GetTechniqueByName( "FastWaveletAnalysis" )->GetPassByIndex( 0 );
	p->GetDesc( &PassDesc );
	V_RETURN( m_pd3dDevice->CreateInputLayout(iaElemDesc, 4, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pVertexLayout) );

	m_pd3dDevice->IASetInputLayout( m_pVertexLayout );

	float texToPix = 0.5f / (FLOAT)m_iRenderWidth;
	StaticVertex arQuadVertices[] =
	{
		-1.0f,	 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-1.0f,	-1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		 1.0f,	 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		 1.0f,	-1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	};
	WORD arIndices[] =
	{
		0, 1, 3,
		0, 3, 2
	};

	D3D10_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage			= D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth		= 6 * sizeof( WORD );
	indexBufferDesc.BindFlags		= D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags	= 0;
	indexBufferDesc.MiscFlags		= 0;

	D3D10_SUBRESOURCE_DATA initData;
	initData.pSysMem = arIndices;

	m_pd3dDevice->CreateBuffer( &indexBufferDesc, &initData, &m_pIBQuad );

	indexBufferDesc.ByteWidth		= 4 * sizeof( StaticVertex );
	indexBufferDesc.BindFlags		= D3D10_BIND_VERTEX_BUFFER;

	initData.pSysMem = arQuadVertices;

	m_pd3dDevice->CreateBuffer( &indexBufferDesc, &initData, &m_pVBQuad );

	m_pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	D3D10_SAMPLER_DESC sd;
	memset(&sd, 0, sizeof(sd));

	sd.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR;
	sd.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR;
	sd.AddressW = D3D10_TEXTURE_ADDRESS_MIRROR;
	sd.MaxLOD = FLT_MAX;
	sd.MaxAnisotropy = 1; 
	sd.ComparisonFunc = D3D10_COMPARISON_NEVER;

	ID3D10SamplerState* pSS;
	V( m_pd3dDevice->CreateSamplerState(&sd, &pSS) );

	//m_pd3dDevice->PSSetSamplers( 0, 1, &pSS );

	return hr;
}

VOID
CGRenderer::CleanUp( VOID )
{
	SAFE_RELEASE( m_pd3dDevice );
}

VOID
CGRenderer::LoadCacheFile( const CString& lpszCacheFilename )
{

	char	szBuf[MAX_PATH];
	char	szCacheFile[MAX_PATH];
	FILE*	pFile;

	UINT	i, iNumOfCache;
	GCamera camera;

	m_Cameras.push_back( camera );
	m_iCurCamIndex = 0;
	m_pCurrentCam = &m_Cameras[0];

	// 장면 로드
	((CMainFrame*)AfxGetMainWnd())->SetOpenFile( lpszCacheFilename );
	

	/*
	char	szBuf[MAX_PATH];
	char	szCacheFile[MAX_PATH];
	FILE*	pFile;

	UINT	i, iNumOfCache;
	GCamera camera;

	WideCharToMultiByte( CP_ACP, 0, lpszCacheFilename.GetString(), -1, szCacheFile, MAX_PATH, 0, 0 );

	for(i = 0; i < strlen(szCacheFile); i++)
	{
		if(szCacheFile[i] == '\\')
		{
			szCacheFile[i] = '/';
		}
	}
	szCacheFile[i] = '\0';

	// 상대경로 사용
	pFile = fopen(szCacheFile,"r");
	if(!pFile)
		printf("캐시 파일을 찾을 수 없습니다. 파일의 경로를 확인하세요.\n");

	fscanf(pFile,"[Shader_dir]\n");	
	fgets(szBuf,MAX_PATH,pFile);

	for(i = 0; i < strlen(szBuf); i++)
	{
		if(szBuf[i] == '\\')
		{
			szBuf[i] = '/';
		}
	}
	szBuf[i-1] = '\0';
	CString curDir( szBuf );

	fscanf(pFile,"[GIRL_cache_file]\n");	
	fgets(szBuf,MAX_PATH,pFile);

	for(i = 0; i < strlen(szBuf); i++)
	{
		if(szBuf[i] == '\\')
		{
			szBuf[i] = '/';
		}
	}
	szBuf[i-1] = '\0';
	std::string girlCacheFilePath( szBuf );

	char szPathMayaObjectFile[MAX_PATH];
	szBuf[strlen(szBuf)-1] = '\0';
	fscanf(pFile,"[obj_dir]\n");
	fgets(szBuf,MAX_PATH,pFile);
	szBuf[strlen(szBuf)-1] = '\0';
	strcpy( szPathMayaObjectFile, szBuf );
	m_lpszSceneFilename = curDir + CString( szPathMayaObjectFile );

	fscanf(pFile,"[Num_of_cache]\n");
	fscanf(pFile,"%d\n",&iNumOfCache);
	fscanf(pFile,"[cache1]\n");
	fscanf(pFile,"[camera_name]\n");
	fgets(szBuf,MAX_PATH,pFile);
	szBuf[strlen(szBuf)-1] = '\0';
	camera.SetName( CString(szBuf).GetString() );
	fscanf(pFile,"[camera_pers]\n");
	fscanf(pFile,"%f\t%f\t%f\t%f\t%f\t%f\n",
		&camera.m_fNear,
		&camera.m_fFar,
		&camera.m_fLeft,
		&camera.m_fRight,
		&camera.m_fBottom,
		&camera.m_fTop);
	fscanf(pFile,"[camera_matrix]\n");
	fscanf(pFile,"%f\t%f\t%f\t%f\n",
		&camera.m_matWorldInv[0],
		&camera.m_matWorldInv[1],
		&camera.m_matWorldInv[2],
		&camera.m_matWorldInv[3]);
	fscanf(pFile,"%f\t%f\t%f\t%f\n",
		&camera.m_matWorldInv[4],
		&camera.m_matWorldInv[5],
		&camera.m_matWorldInv[6],
		&camera.m_matWorldInv[7]);
	fscanf(pFile,"%f\t%f\t%f\t%f\n",
		&camera.m_matWorldInv[8],
		&camera.m_matWorldInv[9],
		&camera.m_matWorldInv[10],
		&camera.m_matWorldInv[11]);
	fscanf(pFile,"%f\t%f\t%f\t%f\n",
		&camera.m_matWorldInv[12],
		&camera.m_matWorldInv[13],
		&camera.m_matWorldInv[14],
		&camera.m_matWorldInv[15]);

	m_Cameras.push_back( camera );
	m_iCurCamIndex = 0;
	m_pCurrentCam = &m_Cameras[0];

	((CMainFrame*)AfxGetMainWnd())->SetOpenFile( CString(girlCacheFilePath.c_str()) );
	*/
}

VOID
CGRenderer::SetupScene()
{
	m_SceneGeometry.LoadFromGIRL( m_pd3dDevice, m_pTechnique );
	//RenderDWT();
}

VOID
CGRenderer::OnFileOpen( LPCWSTR lpszCacheFilename )
{
	GetCurrentDirectory( MAX_PATH, m_lpszWorkingDir );
	LoadCacheFile( lpszCacheFilename );
}

bool	
CGRenderer::IsDeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType, DXGI_FORMAT BufferFormat, bool bWindowed, VOID* pUserContext )
{
	return TRUE;
}

HRESULT
CGRenderer::OnCreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBufferSurfaceDesc, VOID* pUserContext )
{
	HRESULT hr;

	return S_OK;
}

HRESULT
CGRenderer::OnResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* pBufferSurfaceDesc, VOID* pUserContext )
{
	HRESULT hr = S_OK;

	//if( IsRenderStart() ) 
		V_RETURN( MakeMainRenderTargets(pd3dDevice, pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height, 1) );
	return hr;
}

VOID
CGRenderer::OnReleasingSwapChain( VOID* pUserContext )
{

}

HRESULT
CGRenderer::OnResetDevice( ID3D10Device* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, VOID* pUserContext )
{
	return S_OK;
}

VOID 
CGRenderer::OnFrameMove( double fTime, float fElapsedTime, VOID* pUserContext )
{
	// Update the camera's position based on user input 
	//m_pCurrentCam->FrameMove( fElapsedTime );
}

VOID
CGRenderer::RenderSubset( UINT iSubset )
{
	HRESULT hr;

	CGMaterial* pMaterial = m_MeshLoader.GetSubsetGMaterial( iSubset );
		
	D3D10_TECHNIQUE_DESC techDesc;
	pMaterial->m_pTechnique->GetDesc( &techDesc );
	CGMaterial* pGMaterial;
	for ( UINT p = 0; p < techDesc.Passes; ++p )
	{
		//pMaterial->m_pTechnique->GetPassByIndex( p )->Apply( 0 );				
		//m_Shader.SetShaderVariables( pMaterial );
		m_MeshLoader.GetMesh()->DrawSubset( iSubset );		
	}
}

VOID
CGRenderer::RenderDepthMap( GLight* pLight, D3DXMATRIX* pMatView, D3DXMATRIX* pMatProj )
{
	GCameraPacket dummy;
	dummy.fFar = pLight->GetFarClip();

	D3DXVECTOR3 vEye( 0, 0, 0 );
	D3DXMATRIX matWorld;

	D3DXMatrixIdentity( &matWorld );
	
	D3DXMATRIX matOldView = *m_pCurrentCam->GetViewMatrix();
	D3DXMATRIX matOldProj = *m_pCurrentCam->GetProjMatrix();
	D3DXMATRIX matOldViewProj = matOldView * matOldProj;

	m_Shader.SetupDepthRenderCamera( *pMatView, *pMatProj, pLight->GetNearClip(), pLight->GetFarClip() );

	ID3D10EffectTechnique* pTechnique = m_Shader.GetTechnique( "DepthRender" );	

	D3D10_TECHNIQUE_DESC techDesc;
	pTechnique->GetDesc( &techDesc );
	for ( UINT p = 0; p < techDesc.Passes; ++p )
	{		
		pTechnique->GetPassByIndex( p )->Apply( 0 );							
		m_SceneGeometry.DrawSimple( m_pd3dDevice, pTechnique );
	}	

	//m_Shader.SetupCamera( matWorld, matOldView, matOldProj, m_pCurrentCam->m_stCameraInfo );

	// Copy render target to texture
	/*
	ID3D10Texture2D* pTex;
	ID3D10Resource* pRes;
	pLight->GetDSV()->GetResource( &pRes );
	pRes->QueryInterface( __uuidof(ID3D10Texture2D), (LPVOID*)&pTex );
	m_pd3dDevice->CopyResource( pLight->GetShadowMapTex(), pTex );
	if( FAILED(D3DX10SaveTextureToFile(pLight->GetShadowMapTex(), D3DX10_IFF_DDS, L"c:\\shadow.dds")) )
		exit(1);
		*/
			
}

VOID CGRenderer::RenderDWT( )
{
	D3DXMATRIX matOldView = *m_pCurrentCam->GetViewMatrix();
	D3DXMATRIX matOldProj = *m_pCurrentCam->GetProjMatrix();
	D3DXMATRIX matOldViewProj = matOldView * matOldProj;
	D3DXMatrixIdentity( &matOldViewProj );

	UINT nSubset = m_SceneGeometry.GetNumSubsets();
	map<string, GMeshSubset>::iterator iter;
	map<string, GMeshSubset>* pSubsets = m_SceneGeometry.GetSubsets();
	GMeshSubset* pSubset = 0;

	UINT nLevels = 8;
	UINT iStride = sizeof( StaticVertex );
	UINT iOffsets = 0;

	D3D10_VIEWPORT vp;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
		
	D3D10_TEXTURE2D_DESC texDesc;

	m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
	m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	

	for( iter = pSubsets->begin(); iter != pSubsets->end(); iter++ )
	{
		pSubset = (GMeshSubset*)&iter->second;
		
		for( int i=0; i < 8; i++ )
		{
			CGAttribute& attr = pSubset->pGMaterial->GetAttribute( i );			
			
			if( !attr.IsConstant() )
			{
				attr.texture.pTex->GetDesc( &texDesc );
				attr.texture.width = texDesc.Width;
				attr.texture.height = texDesc.Height;

				UINT startLevel = 0;
				vp.Width = 2048;
				vp.Height = 4096;

				ID3D10EffectTechnique* pTechnique = m_Shader.GetDWTEffect()->GetTechniqueByName( "FastWaveletAnalysis" );		
				D3D10_TECHNIQUE_DESC techDesc;
				pTechnique->GetDesc( &techDesc );

				bool bContd = FALSE;
				for( int j=0; j < nLevels; j++ )
				{		
					//////////////////////////////////////////////////////////////////////////
					// horizontal analysis
					
					if( texDesc.Width <= vp.Width )
					{
						vp.Width /= 2;
						vp.Height /= 2;
						startLevel = j;						
						bContd = TRUE;
						continue;
					}
					m_pd3dDevice->RSSetViewports( 1, &vp );

					m_pd3dDevice->OMSetRenderTargets( 2, m_pRTVDWTThin[j], NULL );	
					for( int k = 0; k < 2; k++ )
						m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][k], ClearColor );

					if( bContd || j == 0 )
					{
						bContd = FALSE;						
						m_Shader.SetDWTSRVs( attr.texture.pTexSRV, NULL, NULL, vp.Width, vp.Height );
					}
					else
						m_Shader.SetDWTSRVs( m_pSRVDWTSquare[j - 1][0], NULL, NULL, vp.Width, vp.Height );

					pTechnique->GetPassByIndex( 0 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\thin1.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\thin2.dds" );

					//////////////////////////////////////////////////////////////////////////
					// two vertical analysis(high, low)

					vp.Height /= 2;
					m_pd3dDevice->RSSetViewports( 1, &vp );

					m_pd3dDevice->OMSetRenderTargets( 2, &m_pRTVDWTSquare[j][0], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j][0], ClearColor );
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j][1], ClearColor );
					
					m_Shader.SetDWTSRVs( m_pSRVDWTThin[j][0], NULL, NULL, vp.Width, vp.Height );

					pTechnique->GetPassByIndex( 1 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					m_pd3dDevice->OMSetRenderTargets( 2, &m_pRTVDWTSquare[j][2], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j][2], ClearColor );
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j][3], ClearColor );

					m_Shader.SetDWTSRVs( m_pSRVDWTThin[j][1], NULL, NULL, vp.Width, vp.Height );

					pTechnique->GetPassByIndex( 2 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					// test					
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\square1.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\square2.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][2], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\square3.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][3], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\square4.dds" );
					
					// delete original texture

					vp.Width /= 2;
				}

				vp.Width *= 2;

				// release the original texture
				//attr.texture.pTex->Release();

				/************************************************************************/
				/* Packing Coefficients                                                 */
				/************************************************************************/

				SWC waveletCoeff;				
				D3D10_MAPPED_TEXTURE2D mappedTex;
				FLOAT* pPixels;
				FLOAT fTemp;
				FLOAT fThreshold = 0;
				UINT rtWidth = vp.Width;
				UINT rtHeight = vp.Height;

				for( int j=nLevels - 1; j > startLevel; j-- )
				{
					for( int sq=1; sq < 4; sq++ )
					{
						//////////////////////////////////////////////////////////////////////////
						// access the texture

						m_pd3dDevice->CopyResource( m_pTexDWTSquareST[j][sq], m_pTexDWTSquare[j][sq] );						
						m_pTexDWTSquareST[j][sq]->Map( 0, D3D10_MAP_READ, 0, &mappedTex );
						pPixels = (FLOAT*)mappedTex.pData;

						for( int h=0; h < rtHeight; h++ )
							for( int w=0; w < rtWidth; w++ )
							{
								for( int rgb=0; rgb < 3; rgb++ )
								{
									fTemp = pPixels[rtWidth * h * 4 + w * 4 + rgb];

									waveletCoeff.index[0] = w;
									waveletCoeff.index[1] = h;

									//if( fabs(fTemp) > fThreshold )
									{
										switch( sq )
										{
										case 0:
											break;
										case 1:
											waveletCoeff.index[1] += 2048 / powf( 2, j );
											break;
										case 2:
											waveletCoeff.index[0] += 2048 / powf( 2, j );
											break;
										case 3:
											waveletCoeff.index[0] += 2048 / powf( 2, j );
											waveletCoeff.index[1] += 2048 / powf( 2, j );
											break;
										}

										waveletCoeff.value = fTemp;
										attr.texture.coeffs[rgb].push_back( waveletCoeff );
									}
								}							
							}

						m_pTexDWTSquareST[j][sq]->Unmap( 0 );
						pPixels = NULL;
					}			
					
					rtWidth *= 2;
					rtHeight *= 2;
				}

				//////////////////////////////////////////////////////////////////////////
				// sort and thresholding

				std::sort( attr.texture.coeffs[0].begin(), attr.texture.coeffs[0].end() );
				std::sort( attr.texture.coeffs[1].begin(), attr.texture.coeffs[1].end() );
				std::sort( attr.texture.coeffs[2].begin(), attr.texture.coeffs[2].end() );

				int numCoeffs = 65536;
				attr.texture.numCoeffs = numCoeffs;
				attr.texture.coeffs[0].resize( numCoeffs );
				attr.texture.coeffs[1].resize( numCoeffs );
				attr.texture.coeffs[2].resize( numCoeffs );

				//////////////////////////////////////////////////////////////////////////
				// generate coeff textures

				ID3D10Texture2D* pTexTemp;
				ID3D10Texture2D* pTexTemp2;

				D3D10_TEXTURE2D_DESC texDescWC;
				texDescWC.ArraySize			= 1;
				texDescWC.BindFlags			= D3D10_BIND_SHADER_RESOURCE;
				texDescWC.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
				texDescWC.Width				= 1024;
				texDescWC.Height			= Round2Int( numCoeffs / 1024.f );
				texDescWC.MipLevels			= 0;
				texDescWC.SampleDesc.Count	= 1;
				texDescWC.SampleDesc.Quality= 0;
				texDescWC.MiscFlags			= NULL;
				texDescWC.Usage				= D3D10_USAGE_DEFAULT;
				texDescWC.CPUAccessFlags	= 0;
				m_pd3dDevice->CreateTexture2D( &texDescWC, NULL, &attr.texture.pTexCoeffs );
				texDescWC.Format			= DXGI_FORMAT_R16G16_UINT;
				m_pd3dDevice->CreateTexture2D( &texDescWC, NULL, &attr.texture.pTexIndices );

				m_pd3dDevice->CreateShaderResourceView( attr.texture.pTexCoeffs, NULL, &attr.texture.pSRVCoeffs );
				m_pd3dDevice->CreateShaderResourceView( attr.texture.pTexIndices, NULL, &attr.texture.pSRVIndices );

				texDescWC.BindFlags			= 0;
				texDescWC.Usage				= D3D10_USAGE_STAGING;
				texDescWC.CPUAccessFlags	= D3D10_CPU_ACCESS_WRITE;
				texDescWC.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
				m_pd3dDevice->CreateTexture2D( &texDescWC, NULL, &pTexTemp );
				texDescWC.Format			= DXGI_FORMAT_R16G16_UINT;
				m_pd3dDevice->CreateTexture2D( &texDescWC, NULL, &pTexTemp2 );
				
				pTexTemp->Map( 0, D3D10_MAP_WRITE, 0, &mappedTex );
				pPixels = (FLOAT*)mappedTex.pData;
				for( int h=0; h < Round2Int(numCoeffs / 1024.f); h++ )
				for( int w=0; w < 1024; w++ )
				{
					if( 1024 * h + w > attr.texture.numCoeffs - 1 ) break;
					pPixels[1024 * h * 4 + w * 4] = attr.texture.coeffs[0][1024 * h + w].value;
					pPixels[1024 * h * 4 + w * 4 + 1] = attr.texture.coeffs[1][1024 * h + w].value;
					pPixels[1024 * h * 4 + w * 4 + 2] = attr.texture.coeffs[2][1024 * h + w].value;
					pPixels[1024 * h * 4 + w * 4 + 3] = 32768;
				}				
				pTexTemp->Unmap( 0 );
				m_pd3dDevice->CopyResource( attr.texture.pTexCoeffs, pTexTemp );

				WORD* pPixels2;
				pTexTemp2->Map( 0, D3D10_MAP_WRITE, 0, &mappedTex );
				pPixels2 = (WORD*)mappedTex.pData;
				for( int h=0; h < Round2Int(numCoeffs / 1024.f); h++ )
				for( int w=0; w < 1024; w++ )
				{
					if( 1024 * h + w > attr.texture.numCoeffs - 1 ) break;
					pPixels2[1024 * h * 2 + w * 2] = attr.texture.coeffs[0][1024 * h + w].index[0];
					pPixels2[1024 * h * 2 + w * 2 + 1] = attr.texture.coeffs[1][1024 * h + w].index[1];
				}
				pTexTemp2->Unmap( 0 );
				m_pd3dDevice->CopyResource( attr.texture.pTexIndices, pTexTemp2 );

				D3D10_TEXTURE2D_DESC texDesc2D;
				texDesc2D.ArraySize			= 1;
				texDesc2D.BindFlags			= D3D10_BIND_SHADER_RESOURCE;
				texDesc2D.Format			= DXGI_FORMAT_R32G32B32A32_FLOAT;
				texDesc2D.Width				= 16;
				texDesc2D.Height			= 16;
				texDesc2D.MipLevels			= 1;
				texDesc2D.SampleDesc.Count	= 1;
				texDesc2D.SampleDesc.Quality= 0;
				texDesc2D.MiscFlags			= NULL;
				texDesc2D.Usage				= D3D10_USAGE_DEFAULT;
				texDesc2D.CPUAccessFlags	= NULL;
				m_pd3dDevice->CreateTexture2D( &texDesc2D, NULL, &attr.texture.pTexScale );

				m_pd3dDevice->CopyResource( attr.texture.pTexScale, m_pTexDWTSquare[7][0] );

				m_pd3dDevice->CreateShaderResourceView( attr.texture.pTexScale, NULL, &attr.texture.pSRVScale );

				D3DX10SaveTextureToFile( pTexTemp, D3DX10_IFF_DDS, L"c:\\coeffs2.dds" );
				D3DX10SaveTextureToFile( pTexTemp2, D3DX10_IFF_DDS, L"c:\\indices2.dds" );
				D3DX10SaveTextureToFile( attr.texture.pTexCoeffs, D3DX10_IFF_DDS, L"c:\\coeffs.dds" );
				D3DX10SaveTextureToFile( attr.texture.pTexScale, D3DX10_IFF_DDS, L"c:\\scale.dds" );
				D3DX10SaveTextureToFile( attr.texture.pTexIndices, D3DX10_IFF_DDS, L"c:\\indices.dds" );
				

				attr.texture.coeffs[0].resize( 0 );
				attr.texture.coeffs[1].resize( 0 );
				attr.texture.coeffs[2].resize( 0 );

				attr.texture.startLevel = startLevel;
				
				/*
				// reconstruct
				pTechnique = m_Shader.GetDWTEffect()->GetTechniqueByName( "FastWaveletSynthesis" );		

				for( int j=nLevels - 1; j > startLevel; j-- )
				{				
					m_pd3dDevice->RSSetViewports( 1, &vp );

					// even
					m_Shader.SetDWTSRVs( NULL, m_pSRVDWTSquare[j][1], m_pSRVDWTSquare[j][0], vp.Width, vp.Height, vp.Width, vp.Height, matOldViewProj );

					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][0], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][0], ClearColor );			

					pTechnique->GetPassByIndex( 0 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					
					
					// odd
					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][1], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][1], ClearColor );

					pTechnique->GetPassByIndex( 1 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source1.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][2], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source2.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_even.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_odd.dds" );

					vp.Height *= 2;
					m_pd3dDevice->RSSetViewports( 1, &vp );

					// add
					m_Shader.SetDWTSRVs( NULL, m_pSRVDWTThin[j][0], m_pSRVDWTThin[j][1], vp.Width, vp.Height, vp.Width, vp.Height, matOldViewProj );

					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][2], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][2], ClearColor );

					pTechnique->GetPassByIndex( 2 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );						

					m_pd3dDevice->CopyResource( m_pTexDWTThin[j][0], m_pTexDWTThin[j][2] );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_add.dds" );

					vp.Height /= 2;
					m_pd3dDevice->RSSetViewports( 1, &vp );

					// even
					m_Shader.SetDWTSRVs( NULL, m_pSRVDWTSquare[j][3], m_pSRVDWTSquare[j][2], vp.Width, vp.Height, vp.Width, vp.Height, matOldViewProj );

					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][1], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][1], ClearColor );			

					pTechnique->GetPassByIndex( 3 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					// odd
					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][2], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][2], ClearColor );

					pTechnique->GetPassByIndex( 4 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][3], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source3.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source4.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_even.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][2], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_odd.dds" );

					vp.Height *= 2;
					m_pd3dDevice->RSSetViewports( 1, &vp );

					// add
					m_Shader.SetDWTSRVs( NULL, m_pSRVDWTThin[j][1], m_pSRVDWTThin[j][2], vp.Width, vp.Height, vp.Width, vp.Height, matOldViewProj );

					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][3], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][3], ClearColor );

					pTechnique->GetPassByIndex( 5 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );		

					m_pd3dDevice->CopyResource( m_pTexDWTThin[j][1], m_pTexDWTThin[j][3] );
					//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_add2.dds" );

					//////////////////////////////////////////////////////////////////////////
					// horizontal

					vp.Width *= 2;
					m_pd3dDevice->RSSetViewports( 1, &vp );

					// even
					m_Shader.SetDWTSRVs( NULL, m_pSRVDWTThin[j][1], m_pSRVDWTThin[j][0], vp.Width / 2, vp.Height, vp.Width, vp.Height, matOldViewProj );

					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j - 1][4], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j - 1][4], ClearColor );			

					pTechnique->GetPassByIndex( 6 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					// odd
					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j - 1][5], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j - 1][5], ClearColor );			

					pTechnique->GetPassByIndex( 7 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j - 1][4], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_heven.dds" );
					//D3DX10SaveTextureToFile( m_pTexDWTSquare[j - 1][5], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_hodd.dds" );

					// add
					m_Shader.SetDWTSRVs( NULL, m_pSRVDWTSquare[j - 1][4], m_pSRVDWTSquare[j - 1][5], vp.Width, vp.Height, vp.Width, vp.Height, matOldViewProj );

					m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j - 1][0], NULL );	
					m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j - 1][0], ClearColor );			

					pTechnique->GetPassByIndex( 8 )->Apply( 0 );			
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

					D3DX10SaveTextureToFile( m_pTexDWTSquare[j - 1][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_.dds" );
				}
				
				D3DX10SaveTextureToFile( m_pTexDWTSquare[startLevel][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed.dds" );
				*/
			}			
		}
	}
}

VOID CGRenderer::ApplyLight( GLight* pLight, ID3D10RenderTargetView** pOldRTV, ID3D10DepthStencilView* pOldDSV )
{	
	HRESULT hr = S_OK;

	D3DXMATRIX matView, matProj;

	pLight->Setup( *m_pCurrentCam->GetViewMatrix(), m_iLightFlag );	

	if( pLight->HasShadow() && pLight->NeedDepthRendering() )
	{
		pLight->SetNeedDepthRendering( FALSE );		

		m_pd3dDevice->IASetInputLayout( m_pVertexLayout );

		D3DXMATRIX matView, matProj, matViewProj;

		D3DXVECTOR3 vBoundMax = m_SceneGeometry.GetBoundMax();
		D3DXVECTOR3 vBoundCtr = m_SceneGeometry.GetBoundCtr();
		D3DXVECTOR3 vOrth = vBoundMax - vBoundCtr;
		
		V( pLight->SetToDepthRendering(m_pd3dDevice, D3DXVec3Length( &vOrth ), 1, vBoundCtr, &matView, &matProj) );

		m_Shader.SetLighting( pLight, m_pCurrentCam->GetViewMatrix(), m_iLightFlag );

		D3DXMatrixMultiply( &matViewProj, &matView, &matProj );
		//D3DXMatrixTranspose( &matViewProj, &matViewProj );
		//D3DXMatrixTranspose( &matView, &matView );

		ID3D10Effect* pEffectSSMap = m_Shader.GetMainEffect();
		pEffectSSMap->GetVariableByName( "mViewProj" )->AsMatrix()->SetMatrix( matViewProj );
		pEffectSSMap->GetVariableByName( "mLightView" )->AsMatrix()->SetMatrix( matView );
		pEffectSSMap->GetVariableByName( "g_fLightFar" )->AsScalar()->SetFloat( pLight->GetFarClip() );
		pEffectSSMap->GetVariableByName( "g_fLightNear" )->AsScalar()->SetFloat( pLight->GetNearClip() );

		m_pd3dDevice->IASetInputLayout( m_pVertexLayout );

		pLight->GetSSMap()->Render( m_pd3dDevice, &matViewProj, &matProj, &m_SceneGeometry, pEffectSSMap );

		m_pd3dDevice->IASetInputLayout( m_pVertexLayout );		
		

		const D3DXMATRIX* pMatCamView = m_pCurrentCam->GetViewMatrix();
		const D3DXMATRIX* pMatCamProj = m_pCurrentCam->GetProjMatrix();
		D3DXMATRIX matCamViewProj;
		D3DXMatrixMultiply( &matCamViewProj, pMatCamView, pMatCamProj );
		pEffectSSMap->GetVariableByName( "mViewProj" )->AsMatrix()->SetMatrix( (float *)&matCamViewProj );

		//m_SceneGeometry.DrawSimple( m_pd3dDevice, pEffectSSMap->GetTechniqueByName("RenderDepth") );

		m_pd3dDevice->OMSetRenderTargets( 1, pOldRTV, pOldDSV );

		IDXGISwapChain* pSwapChain = DXUTGetDXGISwapChain();

		// Get the back buffer and desc
		ID3D10Texture2D* pBackBuffer;
		hr = pSwapChain->GetBuffer( 0, __uuidof( *pBackBuffer ), ( LPVOID* )&pBackBuffer );
		D3D10_TEXTURE2D_DESC backBufferSurfaceDesc;
		pBackBuffer->GetDesc( &backBufferSurfaceDesc );

		D3D10_VIEWPORT vp;
		vp.TopLeftX = vp.TopLeftY = 0;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.Width = backBufferSurfaceDesc.Width;
		vp.Height = backBufferSurfaceDesc.Height;
		m_pd3dDevice->RSSetViewports( 1, &vp );

		/*
		D3D10_VIEWPORT vp;
		vp.Width = SHADOWMAP_SIZE;
		vp.Height = SHADOWMAP_SIZE;
		vp.TopLeftX = vp.TopLeftY = 0;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		m_pd3dDevice->RSSetViewports( 1, &vp );

		D3DXMATRIX matView, matProj;
		
		D3DXVECTOR3 vBoundMax = m_SceneGeometry.GetBoundMax();
		D3DXVECTOR3 vBoundCtr = m_SceneGeometry.GetBoundCtr();
		D3DXVECTOR3 vOrth = vBoundMax - vBoundCtr;
		V( pLight->SetToDepthRendering(m_pd3dDevice, D3DXVec3Length( &vOrth ), 1, vBoundCtr, &matView, &matProj) );

		m_Shader.SetLighting( pLight, m_pCurrentCam->GetViewMatrix(), m_iLightFlag );

		ID3D10RenderTargetView* pSavedRTV;
		ID3D10DepthStencilView* pSavedDSV;
		m_pd3dDevice->OMGetRenderTargets( 1, &pSavedRTV, &pSavedDSV );
		ID3D10RenderTargetView* pNullView[] = { NULL };
		ID3D10RenderTargetView* pRTV = m_pRTVShadowRenderTarget;
		m_pd3dDevice->OMSetRenderTargets( 0, NULL, pLight->GetDSV() );		

		//m_pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );
		m_pd3dDevice->ClearDepthStencilView( pLight->GetDSV(), D3D10_CLEAR_DEPTH, 1.0, 0 );
		

		RenderDepthMap( pLight, &matView, &matProj );

		//D3DX10SaveTextureToFile( pLight->GetShadowMapTex(), D3DX10_IFF_DDS, L"c:\\shadow.dds");
		//ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		//ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
		//m_pd3dDevice->OMSetRenderTargets( 1, &pRTV, pDSV );
		
		m_pd3dDevice->OMSetRenderTargets( 1, &pSavedRTV, pSavedDSV );

		IDXGISwapChain* pSwapChain = DXUTGetDXGISwapChain();

		// Get the back buffer and desc
		ID3D10Texture2D* pBackBuffer;
		hr = pSwapChain->GetBuffer( 0, __uuidof( *pBackBuffer ), ( LPVOID* )&pBackBuffer );
		D3D10_TEXTURE2D_DESC backBufferSurfaceDesc;
		pBackBuffer->GetDesc( &backBufferSurfaceDesc );

		vp.Width = backBufferSurfaceDesc.Width;
		vp.Height = backBufferSurfaceDesc.Height;
		m_pd3dDevice->RSSetViewports( 1, &vp );
		*/
	}	
}

VOID
CGRenderer::DrawQuad( VOID )
{
	ID3D10EffectTechnique* pTechnique = m_Shader.GetTechnique( "DrawQuad" );
	D3D10_TECHNIQUE_DESC techDesc;	
	pTechnique->GetDesc( &techDesc );

	for ( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pTechnique->GetPassByIndex( p )->Apply( 0 );							
		m_pd3dDevice->DrawIndexed( 6, 0, 0 );
	}
}

VOID
CGRenderer::OnFrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, VOID* pUserContext )
{	
	ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
	IDXGISwapChain* pSC = DXUTGetDXGISwapChain();
	ID3D10DepthStencilState* pOldDSS;
	
	D3D10_TECHNIQUE_DESC techDesc;
	ID3D10EffectTechnique* pTechnique;

	UINT iStride = sizeof( StaticVertex );
	UINT iOffsets = 0;

	D3D10_VIEWPORT vp;
	vp.Width = m_iRenderWidth;
	vp.Height = m_iRenderHeight;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_pd3dDevice->RSSetViewports( 1, &vp );

	if( m_bCameraChanged ) 
	{		
		/* SSAA
		D3D10_VIEWPORT vp;
		vp.Width = m_iRenderWidth * 2;
		vp.Height = m_iRenderHeight * 2;
		vp.TopLeftX = vp.TopLeftY = 0;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		m_pd3dDevice->RSSetViewports( 1, &vp );
		*/

		m_Shader.SetupCamera( *m_pCurrentCam->GetWorldMatrix(), *m_pCurrentCam->GetViewMatrix(), *m_pCurrentCam->GetProjMatrix(), m_pCurrentCam->GetCamInfo() );
		m_bCameraChanged = FALSE;

		//////////////////////////////////////////////////////////////////////////
		// Render the mesh into MRTs

		pTechnique = m_Shader.GetTechnique( "FillMRT" );		
		pTechnique->GetDesc( &techDesc );

		GMeshSubset* pSubset = 0;
		map<string, GMeshSubset>::iterator iter;

		for ( UINT p = 0; p < techDesc.Passes; ++p )
		{				
			//m_SceneGeometry.Draw( m_pd3dDevice, &m_Shader, pTechnique, p );

			// DWT
			
			UINT nLevels = 8;
			m_pd3dDevice->OMSetRenderTargets( MAX_RENDERTARGET, m_pRTVShaderParams, m_pMainDSV );	

			for( int i = 0; i < MAX_RENDERTARGET; i++ )
				pd3dDevice->ClearRenderTargetView( m_pRTVShaderParams[i], ClearColor );
			pd3dDevice->ClearDepthStencilView( m_pMainDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

			m_SceneGeometry.Draw( pd3dDevice, &m_Shader, pTechnique, p );

			/*
			for( iter = m_SceneGeometry.GetSubsets()->begin(); iter != m_SceneGeometry.GetSubsets()->end(); iter++ )
			{	
				pSubset = (GMeshSubset*)&iter->second;
				
				
				m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
				m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	

				//////////////////////////////////////////////////////////////////////////
				// unpack

				//pTechnique = m_Shader.GetDWTEffect()->GetTechniqueByName( "FastWaveletSynthesis" );		

				for( UINT iAttr = 0; iAttr < 8; iAttr++ )
				{
					CGAttribute attr = pSubset->pGMaterial->GetAttribute( iAttr );
					if( !attr.IsConstant() )
					{
						vp.Width = 16;
						vp.Height = 16;

						for( int j=nLevels - 1; j > attr.texture.startLevel; j-- )
						{				
							//////////////////////////////////////////////////////////////////////////
							// unpack

							m_pd3dDevice->RSSetViewports( 1, &vp );

							pTechnique = m_Shader.GetDWTEffect()->GetTechniqueByName( "UnpackCoeffs" );		
							if( j == 7 )
								m_pd3dDevice->CopyResource( m_pTexDWTSquare[j][0], attr.texture.pTexScale );			

							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source1.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source2.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][2], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source3.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][3], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source4.dds" );
							for( int sq=1; sq < 4; sq++ )
							{
								m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j][sq], NULL );	
								m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j][sq], ClearColor );	

								m_Shader.SetDWTSRVs( attr.texture.pSRVScale, attr.texture.pSRVCoeffs, attr.texture.pSRVIndices, Round2Int(attr.texture.numCoeffs / 1024.f), j, sq );

								pTechnique->GetPassByIndex( 0 )->Apply( 0 );			
								m_pd3dDevice->DrawIndexed( 6, 0, 0 );					
							}
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source1.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source2.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][2], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source3.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][3], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source4.dds" );

							pTechnique = m_Shader.GetDWTEffect()->GetTechniqueByName( "FastWaveletSynthesis" );		

							// even
							m_Shader.SetDWTSRVs( NULL, m_pSRVDWTSquare[j][1], m_pSRVDWTSquare[j][0], vp.Width, vp.Height );

							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][0], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][0], ClearColor );			

							pTechnique->GetPassByIndex( 0 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							// odd
							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][1], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][1], ClearColor );

							pTechnique->GetPassByIndex( 1 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							//D3DX10SaveTextureToFile( m_pTexDWTThin[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_even.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_odd.dds" );

							vp.Height *= 2;
							m_pd3dDevice->RSSetViewports( 1, &vp );

							// add
							m_Shader.SetDWTSRVs( NULL, m_pSRVDWTThin[j][0], m_pSRVDWTThin[j][1], vp.Width, vp.Height );

							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][2], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][2], ClearColor );

							pTechnique->GetPassByIndex( 2 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );						

							m_pd3dDevice->CopyResource( m_pTexDWTThin[j][0], m_pTexDWTThin[j][2] );
							//D3DX10SaveTextureToFile( m_pTexDWTThin[j][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_add.dds" );

							vp.Height /= 2;
							m_pd3dDevice->RSSetViewports( 1, &vp );

							// even
							m_Shader.SetDWTSRVs( NULL, m_pSRVDWTSquare[j][3], m_pSRVDWTSquare[j][2], vp.Width, vp.Height );

							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][1], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][1], ClearColor );			

							pTechnique->GetPassByIndex( 3 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							// odd
							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][2], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][2], ClearColor );

							pTechnique->GetPassByIndex( 4 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][3], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source3.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_source4.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_even.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTThin[j][2], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_odd.dds" );

							vp.Height *= 2;
							m_pd3dDevice->RSSetViewports( 1, &vp );

							// add
							m_Shader.SetDWTSRVs( NULL, m_pSRVDWTThin[j][1], m_pSRVDWTThin[j][2], vp.Width, vp.Height );

							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTThin[j][3], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTThin[j][3], ClearColor );

							pTechnique->GetPassByIndex( 5 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );		

							m_pd3dDevice->CopyResource( m_pTexDWTThin[j][1], m_pTexDWTThin[j][3] );
							//D3DX10SaveTextureToFile( m_pTexDWTThin[j][1], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_add2.dds" );

							//////////////////////////////////////////////////////////////////////////
							// horizontal

							vp.Width *= 2;
							m_pd3dDevice->RSSetViewports( 1, &vp );

							// even
							m_Shader.SetDWTSRVs( NULL, m_pSRVDWTThin[j][1], m_pSRVDWTThin[j][0], vp.Width, vp.Height );

							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j - 1][4], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j - 1][4], ClearColor );			

							pTechnique->GetPassByIndex( 6 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							// odd
							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j - 1][5], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j - 1][5], ClearColor );			

							pTechnique->GetPassByIndex( 7 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j - 1][4], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_heven.dds" );
							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j - 1][5], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_hodd.dds" );

							// add
							m_Shader.SetDWTSRVs( NULL, m_pSRVDWTSquare[j - 1][4], m_pSRVDWTSquare[j - 1][5], vp.Width, vp.Height );

							m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVDWTSquare[j - 1][0], NULL );	
							m_pd3dDevice->ClearRenderTargetView( m_pRTVDWTSquare[j - 1][0], ClearColor );			

							pTechnique->GetPassByIndex( 8 )->Apply( 0 );			
							m_pd3dDevice->DrawIndexed( 6, 0, 0 );					

							//D3DX10SaveTextureToFile( m_pTexDWTSquare[j - 1][0], D3DX10_IMAGE_FILE_FORMAT::D3DX10_IFF_DDS, L"c:\\reconstructed_.dds" );
						}

						m_Shader.SetVShaderParams( iAttr, D3DXVECTOR4(-9999, -9999, -9999, -9999) );
						m_Shader.SetSRVShaderParams( iAttr, m_pSRVDWTSquare[attr.texture.startLevel + 1][0] );
					}
					else
					{
						m_Shader.SetVShaderParams( iAttr, D3DXVECTOR4(attr.constant.c[0], attr.constant.c[1], attr.constant.c[2], attr.constant.alpha) );
						m_Shader.SetSRVShaderParams( iAttr, NULL );
					}
				}
				
				m_Shader.SetQuadTexSRV( pSubset->pGMaterial );
				pTechnique = m_Shader.GetTechnique( "FillMRT" );	
				pTechnique->GetPassByIndex( p )->Apply( 0 );			

				pSubset->Draw( pd3dDevice );
				
			}
			*/
			
		}
		//////////////////////////////////////////////////////////////////////////
		//D3DX10SaveTextureToFile( m_pTexShaderParams[1], D3DX10_IFF_PNG, L"c:\\color.png" );

		m_bMakeBuffer = TRUE;

		if( m_bFirstRender )
		{
			m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
			m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	

			m_pd3dDevice->OMSetRenderTargets( 2, m_pRTVNormalDepth, m_pMainDSV );	

			for( int i = 0; i < 2; i++ )
				pd3dDevice->ClearRenderTargetView( m_pRTVNormalDepth[i], ClearColor );
			pd3dDevice->ClearDepthStencilView( m_pMainDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

			pTechnique = m_Shader.GetTechnique( "DrawNormalDepth" );
			pTechnique->GetDesc( &techDesc );
			for ( UINT p = 0; p < techDesc.Passes; ++p )
			{
				m_Shader.SetQuadTexSRV( m_pSRVShaderParams );
				pTechnique->GetPassByIndex( p )->Apply( 0 );							
				m_pd3dDevice->DrawIndexed( 6, 0, 0 );
			}

			
			// render micro-renders
			/*
			pd3dDevice->CopyResource( m_pTexNormalDepthST[0], m_pTexNormalDepth[0] );
			pd3dDevice->CopyResource( m_pTexNormalDepthST[1], m_pTexNormalDepth[1] );
			D3DX10SaveTextureToFile( m_pTexNormalDepth[0], D3DX10_IFF_DDS, L"c:\\normal.dds" );
			D3DX10SaveTextureToFile( m_pTexNormalDepth[1], D3DX10_IFF_DDS, L"c:\\pos.dds" );


			D3D10_MAPPED_TEXTURE2D mappedTex;
			m_pTexNormalDepthST[0]->Map( 0, D3D10_MAP_READ, 0, &mappedTex );
			FLOAT* pPixels = (FLOAT*)mappedTex.pData;
			m_pTexNormalDepthST[1]->Map( 0, D3D10_MAP_READ, 0, &mappedTex );
			FLOAT* pPixelsPos = (FLOAT*)mappedTex.pData;

			D3DXVECTOR3 vNormal;
			FLOAT fDepth;

			D3DXMATRIX matWorld, matView, matProj, mWorldInvTrans, matPers;
			D3DXMatrixIdentity( &matWorld );

			D3DXVECTOR3 vCamRay, vEyePos, P, vDir;
			D3DXVECTOR4 vWorldPos;

			D3DXMatrixPerspectiveFovLH( &matPers, 1.57f, 1, 0.01f, 1000 );

			GCameraPacket cp;

			pTechnique = m_Shader.GetTechnique( "MicroRender" );
			pTechnique->GetDesc( &techDesc );

			for( int i = 0; i < MAX_RENDERTARGET; i++ )
				pd3dDevice->ClearRenderTargetView( m_pRTVMicroRender[i], ClearColor );
			pd3dDevice->ClearDepthStencilView( m_pMainDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

			int texIndex = 0;

			for( UINT h=0; h < m_iRenderHeight; h++ )
				for( UINT w=0; w < m_iRenderWidth; w++ )
				{
					vNormal.x = pPixels[m_iRenderWidth * h * 4 + w * 4];
					vNormal.y = pPixels[m_iRenderWidth * h * 4 + w * 4 + 1];
					vNormal.z = pPixels[m_iRenderWidth * h * 4 + w * 4 + 2];
					P.x = pPixelsPos[m_iRenderWidth * h * 4 + w * 4];
					P.y = pPixelsPos[m_iRenderWidth * h * 4 + w * 4 + 1];
					P.z = pPixelsPos[m_iRenderWidth * h * 4 + w * 4 + 2];

					vEyePos = P;

					D3DXMatrixLookAtLH( &matView, &vEyePos, &D3DXVECTOR3(vEyePos + vNormal), &D3DXVECTOR3(0, 1, 0) );

					m_Shader.SetupCamera( matWorld, matView, matPers, cp );

					vp.Width = MICRO_RENDER_SIZE;
					vp.Height = MICRO_RENDER_SIZE;
					pd3dDevice->RSSetViewports( 1, &vp );

					m_pd3dDevice->OMSetRenderTargets( MAX_RENDERTARGET, m_pRTVMicroRender, m_pDSVMicroRender );	
					for( int i = 0; i < MAX_RENDERTARGET; i++ )
						pd3dDevice->ClearRenderTargetView( m_pRTVMicroRender[i], ClearColor );
					pd3dDevice->ClearDepthStencilView( m_pDSVMicroRender, D3D10_CLEAR_DEPTH, 1.0, 0 );

					

					m_SceneGeometry.Draw( pd3dDevice, &m_Shader, pTechnique, 0 );			

					m_pd3dDevice->OMSetRenderTargets( MAX_RENDERTARGET, m_pRTVMicroRenders, NULL );	
					//pd3dDevice->ClearDepthStencilView( m_pMainDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

					D3DX10SaveTextureToFile( m_pTexMicroRender[1], D3DX10_IFF_DDS, L"c:\\micro.dds" );

					// draw micro-render into the big texture
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtAmbientM" )->AsShaderResource()->SetResource( m_pSRVMicroRender[0] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtColorM" )->AsShaderResource()->SetResource( m_pSRVMicroRender[1] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtSpecularM" )->AsShaderResource()->SetResource( m_pSRVMicroRender[2] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtSpecularAttr1M" )->AsShaderResource()->SetResource( m_pSRVMicroRender[3] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtTransparencyM" )->AsShaderResource()->SetResource( m_pSRVMicroRender[4] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtCommon1M" )->AsShaderResource()->SetResource( m_pSRVMicroRender[5] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtCommon2M" )->AsShaderResource()->SetResource( m_pSRVMicroRender[6] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtNormalM" )->AsShaderResource()->SetResource( m_pSRVMicroRender[7] );						
					m_Shader.GetMainEffect()->GetVariableByName( "g_iMicroTexIndex" )->AsScalar()->SetInt( texIndex );						
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtAmbientMB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[0] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtColorMB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[1] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtSpecularMB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[2] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtSpecularAttr1MB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[3] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtTransparencyMB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[4] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtCommon1MB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[5] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtCommon2MB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[6] );
					m_Shader.GetMainEffect()->GetVariableByName( "g_rtNormalMB" )->AsShaderResource()->SetResource( m_pSRVMicroRenders[7] );						

					m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
					m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	

					vp.Width = 2048;
					vp.Height = 2048;
					pd3dDevice->RSSetViewports( 1, &vp );

					m_Shader.SetQuadTexSRV( m_pSRVShaderParams );
					pTechnique->GetPassByName( "RenderBig" )->Apply( 0 );							
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );
					
					texIndex++;
				}

				m_pTexNormalDepthST[0]->Unmap( 0 );
				m_pTexNormalDepthST[1]->Unmap( 0 );
				*/
		}		
	}
	else
		m_Shader.SetNumUseSample( 4 );

	/* Draw per mesh for deformable scene
	for ( UINT p = 0; p < techDesc.Passes; ++p )
	{		
		for( UINT iSubset = 0; iSubset < m_MeshLoader.GetNumSubsets(); ++iSubset )
		{
			CGMaterial* pMaterial = m_MeshLoader.GetSubsetGMaterial( iSubset );
			m_Shader.SetShaderVariables( pMaterial );

			pTechnique->GetPassByIndex( p )->Apply( 0 );			
			m_MeshLoader.GetMesh()->DrawSubset( iSubset );	
		}
	}
	*/
	
	//m_Shader.SetDepthTexture( pDSV );
	

	//////////////////////////////////////////////////////////////////////////
	// Deferred Lighting 

	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();

	UINT numLights = m_Lights.size();
	
	/* SSAA
	D3D10_VIEWPORT vp;
	vp.Width = m_iRenderWidth;
	vp.Height = m_iRenderHeight;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_pd3dDevice->RSSetViewports( 1, &vp );
	pd3dDevice->ClearRenderTargetView( m_pRTVMainRenderTarget2X, ClearColor );
	m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVMainRenderTarget2X, NULL );	
	*/

	m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
	m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	

	if( m_bMakeBuffer )
	{
		m_pd3dDevice->OMSetRenderTargets( 1, &m_pRTVOffScreen, NULL );		
		m_pd3dDevice->ClearRenderTargetView( m_pRTVOffScreen, ClearColor );	
		m_pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );
		
		for( int i=0; i < numLights; i++ )
		{
			if( m_Lights[i]->IsDirty() )
				m_Lights[i]->InBaseTex( FALSE );                    
			else
			{				
				ApplyLight( m_Lights[i], &m_pRTVOffScreen, NULL );
				m_Shader.SetLighting( m_Lights[i], m_pCurrentCam->GetViewMatrix(), m_iLightFlag );

				m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
				m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	
			
				pTechnique = m_Shader.GetTechnique( "ResolveMRT" );
				pTechnique->GetDesc( &techDesc );
				for ( UINT p = 0; p < techDesc.Passes; ++p )
				{
					m_Shader.SetQuadTexSRV( m_pSRVShaderParams );
					pTechnique->GetPassByIndex( p )->Apply( 0 );							
					m_pd3dDevice->DrawIndexed( 6, 0, 0 );
				}

				m_Lights[i]->InBaseTex( TRUE );

				m_bMakeBuffer = FALSE;
			}			
		}		
	}

	m_pd3dDevice->OMSetRenderTargets( 1, &pRTV, pDSV );		
	m_pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );
	m_pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

	m_Shader.SetLighting( NULL, m_pCurrentCam->GetViewMatrix(), m_iLightFlag );
	m_Shader.SetQuadTexSRV( m_pSRVOffScreenRenderTargetTex );
	DrawQuad();
		
	pTechnique = m_Shader.GetTechnique( "ResolveMRT" );
	pTechnique->GetDesc( &techDesc );
	m_Shader.SetLighting( NULL, m_pCurrentCam->GetViewMatrix(), 50 );
	m_Shader.SetQuadTexSRV( m_pSRVShaderParams );
	pTechnique->GetPassByIndex( 0 )->Apply( 0 );			
	m_pd3dDevice->DrawIndexed( 6, 0, 0 );

	for( int i=0; i < numLights; i++ )
	{
		if( m_Lights[i]->IsInBaseTex() )
		{
			m_BitmaskLightsInBaseTex.BitSet( i );
			m_BitmaskObjectLink.BitSet( i );			
		}
		else
		{
			ApplyLight( m_Lights[i], &pRTV, pDSV );
			m_Shader.SetLighting( m_Lights[i], m_pCurrentCam->GetViewMatrix(), m_iLightFlag );			

			m_pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVBQuad, &iStride, &iOffsets );
			m_pd3dDevice->IASetIndexBuffer( m_pIBQuad, DXGI_FORMAT_R16_UINT, 0 );	

			pTechnique = m_Shader.GetTechnique( "ResolveMRT" );
			pTechnique->GetDesc( &techDesc );
			for ( UINT p = 0; p < techDesc.Passes; ++p )
			{
				m_Shader.SetQuadTexSRV( m_pSRVShaderParams );
				pTechnique->GetPassByIndex( p )->Apply( 0 );							
				m_pd3dDevice->DrawIndexed( 6, 0, 0 );
			}

			m_BitmaskLightsInBaseTex.BitClear( i );
			m_BitmaskObjectLink.BitClear( i );
		}
		m_Lights[i]->SetDirty( FALSE );
	}

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Sobel edge-detect AA

	/*
	pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );
	m_pd3dDevice->OMSetRenderTargets( 1, &pRTV, pDSV );

	pTechnique = m_Shader.GetTechnique( "DrawSimpleQuad" );
	pTechnique->GetDesc( &techDesc );
	m_Shader.SetShaderVariables( m_pSRVRenderTarget2XTex );
	pTechnique->GetPassByIndex( 0 )->Apply( 0 );							

	m_pd3dDevice->DrawIndexed( 6, 0, 0 );
	*/

	/*
	ID3D10Texture2D* pRTTex = NULL;
	ID3D10Resource* pRTRes;
	ID3D10ShaderResourceView* pSRV;
	pRTV->GetResource( &pRTRes );
	pRTRes->QueryInterface( __uuidof(ID3D10Texture2D), (LPVOID*)&pRTTex );
	m_pd3dDevice->CopyResource( m_pTexMainRenderTarget, pRTTex );
	m_Shader.SetShaderVariables( m_pSRVRenderTargetTex );

	D3D10_VIEWPORT vp;
	vp.Width = m_iRenderWidth * 2;
	vp.Height = m_iRenderHeight * 2;
	vp.TopLeftX = vp.TopLeftY = 0;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	m_pd3dDevice->RSSetViewports( 1, &vp );
	IDXGISwapChain* pSC = DXUTGetDXGISwapChain();
	pSC->ResizeBuffers( 1, vp.Width, vp.Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );
	*/

	//ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	
	
	/*
	vp.Width = m_iRenderWidth;
	vp.Height = m_iRenderHeight;
	m_pd3dDevice->RSSetViewports( 1, &vp );
	//pSC->ResizeBuffers( 1, m_iRenderWidth, m_iRenderHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0 );

	m_pd3dDevice->OMSetRenderTargets( 1, &pRTV, pDSV );
	m_Shader.SetShaderVariables( m_pSRVRenderTarget2XTex );
	pTechnique->GetPassByIndex( 0 )->Apply( 0 );							
	m_pd3dDevice->DrawIndexed( 6, 0, 0 );
	*/
}

LRESULT
CGRenderer::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, VOID* pUserContext )
{	
	if( m_pCurrentCam && m_pCurrentCam->HandleMessages( hWnd, uMsg, wParam, lParam ) ) return TRUE;

	return TRUE;
}

VOID 
CGRenderer::OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, VOID* pUserContext )
{

}

bool 
CGRenderer::ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, VOID* pUserContext )
{
	return TRUE;
}

VOID
CGRenderer::OnLostDevice( VOID* pUserContext )
{

}

VOID	
CGRenderer::OnDestroyDevice( VOID* pUserContext )
{	
	SAFE_RELEASE( m_pd3dDevice );
	m_pd3dDevice = 0;	
}

VOID
CGRenderer::OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, VOID* pUserContext )
{

}

