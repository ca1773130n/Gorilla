//----------------------------------------------------------------------------------
// File:   SoftShadowsMap.cpp
// Author: Kirill Dmitriev
// Email:  sdkfeedback@nvidia.com
// 
// Copyright (c) 2007 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include "stdafx.h"
//#include <dxut.h>
//#include <dxutgui.h>
//#include <dxutsettingsdlg.h>
#include <sdkmisc.h>
#include "gmesh.h"
#include <D3D10effect.h>

#include "SoftShadowMap.h"

#define DEPTH_RES 1024

SSMap::SSMap()
{
	HRESULT hr;

	ZeroMemory(this, sizeof(*this));
	m_fFilterSize = 0.1f;
	bAccurateShadow = FALSE;
	nMips = (int)(log((double)DEPTH_RES) / M_LN2);
	m_pOldRenderState = NULL;
	m_pDepthTex[0] = NULL;

	ID3D10Device *pDev10 = DXUTGetD3D10Device();

	if (m_pDepthTex[0] == NULL)
	{
		m_pDepthMip2SRViews = new ID3D10ShaderResourceView *[nMips];
		m_pDepthMip2RTViews = new ID3D10RenderTargetView *[nMips];
		// create render targets
		D3D10_TEXTURE2D_DESC rtDesc =
		{
			DEPTH_RES, //UINT Width;
			DEPTH_RES, //UINT Height;
			1,//UINT MipLevels;
			1,//UINT ArraySize;
			DXGI_FORMAT_R32_TYPELESS,//DXGI_FORMAT Format;
			{1, 0}, //DXGI_SAMPLE_DESC SampleDesc;
			D3D10_USAGE_DEFAULT, //D3D10_USAGE Usage;
			D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_DEPTH_STENCIL,//UINT BindFlags;
			0,//UINT CPUAccessFlags;
			0,//UINT MiscFlags;
		};
		V(pDev10->CreateTexture2D(&rtDesc, NULL, &m_pDepthTex[0]));
		rtDesc.MipLevels = nMips;
		rtDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
		rtDesc.Format = DXGI_FORMAT_R32G32_TYPELESS;
		V(pDev10->CreateTexture2D(&rtDesc, NULL, &m_pDepthMip2));
		rtDesc.Width = (rtDesc.Width * 3) / 2;
		rtDesc.MipLevels = 1;
		V(pDev10->CreateTexture2D(&rtDesc, NULL, &m_pBigDepth2));

		D3D10_DEPTH_STENCIL_VIEW_DESC dsViewDesc;
		D3D10_SHADER_RESOURCE_VIEW_DESC srViewDesc;
		dsViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		srViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		dsViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		srViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		dsViewDesc.Texture2D.MipSlice = 0;
		srViewDesc.Texture2D.MostDetailedMip = 0;
		srViewDesc.Texture2D.MipLevels = 1;
		V(pDev10->CreateDepthStencilView(m_pDepthTex[0], &dsViewDesc, &m_pDepthDSView[0]));
		V(pDev10->CreateShaderResourceView(m_pDepthTex[0], &srViewDesc, &m_pDepthSRView[0]));
		srViewDesc.Texture2D.MipLevels = nMips;
		srViewDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		V(pDev10->CreateShaderResourceView(m_pDepthMip2, &srViewDesc, &m_pDepthMip2SRView));

		srViewDesc.Texture2D.MipLevels = 1;
		D3D10_RENDER_TARGET_VIEW_DESC rtViewDesc;
		rtViewDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
		for (int im = 0; im < nMips; ++im)
		{
			srViewDesc.Texture2D.MostDetailedMip = im;
			srViewDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			V(pDev10->CreateShaderResourceView(m_pDepthMip2, &srViewDesc, &m_pDepthMip2SRViews[im]));
			rtViewDesc.Texture2D.MipSlice = im;
			rtViewDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			V(pDev10->CreateRenderTargetView(m_pDepthMip2, &rtViewDesc, &m_pDepthMip2RTViews[im]));
		}
		rtViewDesc.Texture2D.MipSlice = 0;
		V(pDev10->CreateRenderTargetView(m_pBigDepth2, &rtViewDesc, &m_pBigDepth2RTView));
		srViewDesc.Texture2D.MostDetailedMip = 0;
		V(pDev10->CreateShaderResourceView(m_pBigDepth2, &srViewDesc, &m_pBigDepth2SRView));

		/*
		static const D3D10_INPUT_ELEMENT_DESC depth_layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};
		D3D10_PASS_DESC PassDesc;
		V(m_pDRenderTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));		
		V(pDev10->CreateInputLayout(depth_layout, 4, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pDepthLayout));
		*/

		SAFE_RELEASE(m_pRasterState);
		D3D10_RASTERIZER_DESC RasterState;
		RasterState.FillMode = D3D10_FILL_SOLID;
		RasterState.CullMode = D3D10_CULL_BACK;
		RasterState.FrontCounterClockwise = true;
		RasterState.DepthBias = false;
		RasterState.DepthBiasClamp = 0;
		RasterState.SlopeScaledDepthBias = 0;
		RasterState.DepthClipEnable = true;
		RasterState.ScissorEnable = false;
		RasterState.MultisampleEnable = false;
		RasterState.AntialiasedLineEnable = false;
		V(pDev10->CreateRasterizerState(&RasterState, &m_pRasterState));

		SAFE_RELEASE(m_pDSState);
		D3D10_DEPTH_STENCIL_DESC DSState;
		ZeroMemory(&DSState, sizeof(DSState));
		DSState.DepthEnable = true;
		DSState.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		DSState.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
		V(pDev10->CreateDepthStencilState(&DSState, &m_pDSState));
	}
}
void SSMap::OnWindowResize()
{
	SAFE_RELEASE(m_pOldRenderState);
}
void SSMap::Render( ID3D10Device *pDev10, D3DXMATRIX* pMatViewProj, D3DXMATRIX* pMatProj, CGGeometry* pMesh, ID3D10Effect *pEffect )
{
	HRESULT hr;
	mLightViewProj = *pMatViewProj;
	mLightProj = *pMatProj;
	
	m_pDRenderTechnique = pEffect->GetTechniqueByName("RenderDepth");
	ID3D10EffectTechnique *pDReworkTechnique2 = pEffect->GetTechniqueByName("ReworkDepth2");
	
	if (m_pOldRenderState == NULL)
	{
		D3D10_STATE_BLOCK_MASK SBMask;
		ZeroMemory(&SBMask, sizeof(SBMask));
		SBMask.RSViewports = true;
		SBMask.OMRenderTargets = true;
		SBMask.RSRasterizerState = true;
		V(D3D10CreateStateBlock(pDev10, &SBMask, &m_pOldRenderState));
	}
	V(m_pOldRenderState->Capture());

	float fTmp = bAccurateShadow ? (FLOAT)(10 + 300 * m_fFilterSize) : (FLOAT)(0.5 + 30 * m_fFilterSize);
	V(pEffect->GetVariableByName("g_fFilterSize")->AsScalar()->SetFloat(fTmp));
	V(pEffect->GetVariableByName("g_fDoubleFilterSizeRev")->AsScalar()->SetFloat((FLOAT)(1.0 / (2 * fTmp))));

	D3D10_VIEWPORT vp;
	vp.Height = DEPTH_RES;
	vp.Width = DEPTH_RES;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDev10->RSSetViewports(1, &vp);

	// render depth
	
	pDev10->RSSetState(m_pRasterState);
	pDev10->OMSetDepthStencilState(m_pDSState, 0);
	ID3D10RenderTargetView *pNullRTView = NULL;
	pDev10->OMSetRenderTargets(1, &pNullRTView, m_pDepthDSView[0]);
	//pDev10->IASetInputLayout(m_pDepthLayout);
	pDev10->ClearDepthStencilView(m_pDepthDSView[0], D3D10_CLEAR_DEPTH, 1.0, 0);

	pMesh->DrawSimple( pDev10, m_pDRenderTechnique );
	//pMesh->Render(pDev10, m_pDRenderTechnique);
	

	// needed because we will use LightViewProj to compute texture coordinates, but not clip-space coordinates
	D3DXMATRIX mClip2Tex;
	if (bAccurateShadow)
	{
		mClip2Tex = D3DXMATRIX( 0.5,    0, 0,   0,
			0, -0.5, 0,   0,
			0,    0, 1,   0,
			0.5,  0.5, 0,   1 );
	}
	else
	{
		mClip2Tex = D3DXMATRIX(0.5 * DEPTH_RES, 0, 0, 0,
			0, -0.5 * DEPTH_RES, 0, 0,
			0, 0, 1, 0,
			DEPTH_RES * 0.5, DEPTH_RES * 0.5, 0, 1 );
	}
	D3DXMATRIX mLightViewProjClip2Tex, mLightProjClip2TexInv, mTmp;
	D3DXMatrixMultiply(&mLightViewProjClip2Tex, &mLightViewProj, &mClip2Tex);
	V(pEffect->GetVariableByName("mLightViewProjClip2Tex")->AsMatrix()->SetMatrix((float *)&mLightViewProjClip2Tex));
	D3DXMatrixMultiply(&mTmp, &mLightProj, &mClip2Tex);
	D3DXMatrixInverse(&mLightProjClip2TexInv, NULL, &mTmp);
	V(pEffect->GetVariableByName("mLightProjClip2TexInv")->AsMatrix()->SetMatrix((float *)&mLightProjClip2TexInv));

	// create mipmap pyramid
	//D3DX10SaveTextureToFile( m_pDepthTex[0], D3DX10_IFF_DDS, L"c:\\aaa.dds");

	V(pEffect->GetVariableByName("DepthTex0")->AsShaderResource()->SetResource(m_pDepthSRView[0]));
	pDev10->OMSetRenderTargets(1, &m_pDepthMip2RTViews[0], NULL);
	ID3D10Buffer *pNullVBuf[] = { NULL };
	unsigned pStrides[] = { 0 };
	unsigned pOffsets[] = { 0 };
	pDev10->IASetVertexBuffers(0, 1, pNullVBuf, pStrides, pOffsets);
	pDev10->IASetInputLayout(NULL);
	V(pDReworkTechnique2->GetPassByName("ConvertDepth")->Apply(0));
	for (int im = 0; ; )
	{
		pDev10->Draw(3, 0);
		if (++im == nMips)
		{ break; }
		vp.Width = (vp.Height /= 2);
		pDev10->RSSetViewports(1, &vp);
		V(pEffect->GetVariableByName("DepthMip2")->AsShaderResource()->SetResource(m_pDepthMip2SRViews[im - 1]));
		pDev10->OMSetRenderTargets(1, &m_pDepthMip2RTViews[im], NULL);
		V(pDReworkTechnique2->GetPassByName("CreateMip")->Apply(0));
	}

	pDev10->OMSetRenderTargets(1, &pNullRTView, NULL);
	V(pEffect->GetVariableByName("DepthMip2")->AsShaderResource()->SetResource(m_pDepthMip2SRView));
	V(pDReworkTechnique2->GetPassByName("ConvertToBig")->Apply(0));
	vp.Height = DEPTH_RES;
	vp.Width = (DEPTH_RES * 3) / 2;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pDev10->RSSetViewports(1, &vp);
	pDev10->OMSetRenderTargets(1, &m_pBigDepth2RTView, NULL);

	pDev10->Draw(3, 0);

	pDev10->OMSetRenderTargets(1, &pNullRTView, NULL);

	static bool bSaved = true;
	if (!bSaved)
	{
		bSaved = true;
		ID3D10Texture2D *pTexture = NULL;
		D3D10_TEXTURE2D_DESC textureDesc;
		m_pDepthTex[0]->GetDesc(&textureDesc);
		textureDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		textureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
		textureDesc.Usage = D3D10_USAGE_STAGING;
		textureDesc.BindFlags = 0;
		V(pDev10->CreateTexture2D(&textureDesc, NULL, &pTexture));
		pDev10->CopyResource(pTexture, m_pDepthTex[0]);
		D3DX10SaveTextureToFile(pTexture, D3DX10_IFF_DDS, L"c:\\fff.dds");
	}

	V(pEffect->GetVariableByName("DepthMip2")->AsShaderResource()->SetResource(m_pBigDepth2SRView));

	V(m_pOldRenderState->Apply());
}
void SSMap::OnDestroy()
{
	SAFE_RELEASE(m_pDepthTex[0]);
	SAFE_RELEASE(m_pDepthSRView[0]);
	SAFE_RELEASE(m_pDepthDSView[0]);

	SAFE_RELEASE(m_pDepthMip2);
	SAFE_RELEASE(m_pDepthMip2SRView);

	if (m_pDepthMip2RTViews)
	{
		for (int im = 0; im < nMips; ++im)
		{
			SAFE_RELEASE(m_pDepthMip2RTViews[im]);
			SAFE_RELEASE(m_pDepthMip2SRViews[im]);
		}
	}

	delete []m_pDepthMip2RTViews;
	delete []m_pDepthMip2SRViews;

	SAFE_RELEASE(m_pOldRenderState); ///< we save rendering state here
	SAFE_RELEASE(m_pDepthLayout); ///< layout with only POSITION semantic in it
	SAFE_RELEASE(m_pRasterState); ///< render state we use to render shadow map
	SAFE_RELEASE(m_pDSState); ///< render state we use to render shadow map

	SAFE_RELEASE(m_pBigDepth2);
	SAFE_RELEASE(m_pBigDepth2SRView);
	SAFE_RELEASE(m_pBigDepth2RTView);

	ZeroMemory(this, sizeof(*this));
	m_fFilterSize = 0.1f;
	bAccurateShadow = true;
	nMips = (int)(log((double)DEPTH_RES) / M_LN2);
}
