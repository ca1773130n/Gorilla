//----------------------------------------------------------------------------------
// File:   SoftShadowsMap.h
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

/// soft shadow map
#pragma once

#include "stdafx.h"

class CGGeometry;

class SSMap
{
	ID3D10Texture2D *m_pDepthTex[1], *m_pDepthMip2, *m_pBigDepth2; ///< textures for rendering
	ID3D10DepthStencilView *m_pDepthDSView[1]; ///< depth stencil view
	ID3D10RenderTargetView **m_pDepthMip2RTViews, *m_pBigDepth2RTView;
	ID3D10StateBlock *m_pOldRenderState; ///< we save rendering state here
	ID3D10RasterizerState *m_pRasterState; ///< render state we use to render shadow map
	ID3D10DepthStencilState *m_pDSState; ///< render state we use to render shadow map
	ID3D10ShaderResourceView *m_pDepthSRView[1], *m_pDepthMip2SRView, **m_pDepthMip2SRViews, *m_pBigDepth2SRView;
	int nMips; ///< number of depth mips (depends on the depth map resolution)

public:
	ID3D10InputLayout *m_pDepthLayout; ///< layout with only POSITION semantic in it
	ID3D10EffectTechnique *m_pDRenderTechnique;
	SSMap();
	void OnDestroy();
	void OnWindowResize();
	ID3D10ShaderResourceView* GetDepthSRV() { return m_pDepthSRView[0]; }
	float m_fFilterSize;
	bool bAccurateShadow;
	D3DXMATRIX mLightViewProj, mLightProj;

	void Render(ID3D10Device *, D3DXMATRIX* pMatViewProj, D3DXMATRIX* pMatProj, CGGeometry*, ID3D10Effect *);
};
