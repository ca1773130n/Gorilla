//----------------------------------------------------------------------------------
// File:   SoftShadows.fx
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

//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------

RasterizerState RStateMSAAON
{
	MultisampleEnable = FALSE; // performance hit is too high with MSAA for this sample
};

Texture2D<float> DepthTex0;
Texture2D<float2> DepthMip2;
Texture2D DiffuseTex;
SamplerComparisonState DepthCompare;
SamplerState DepthSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
};
SamplerState DiffuseSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
cbuffer cb0 : register(b0)
{
    float4 g_vMaterialKd;
    float3 g_vLightPos; ///< light in world CS
    float4 g_vLightFlux;
    float g_fFilterSize, g_fDoubleFilterSizeRev;
    row_major float4x4 mViewProj;
    row_major float4x4 mLightView;
    row_major float4x4 mLightViewProjClip2Tex;
    row_major float4x4 mLightProjClip2TexInv;
    bool bTextured;
};

#define N_LEVELS 10
#define DEPTH_RES 1024
cbuffer cb1 : register(b1)
{
    float g_fResRev[N_LEVELS] = { 1./1024, 1./512, 1./256, 1./128, 1./64, 1./32, 1./16, 1./8, 1./4, 1./2 };
};

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_IN
{
    float3 vPos : POSITION; ///< vertex position
    float3 vNorm : NORMAL; ///< vertex diffuse color (note that COLOR0 is clamped from 0..1)
	float3 vTangent : TANGENT;
    float2 vTCoord : TEXCOORD; ///< vertex texture coords
};
struct VS_OUT0
{
    float4 vPos : SV_Position; ///< vertex position
    float4 vDiffColor : COLOR0; ///< vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 vTCoord : TEXCOORD0; ///< vertex texture coords 
    float4 vLightPos : TEXCOORD2;
};
struct VS_OUT1
{
    float4 vPos : SV_Position; ///< vertex position
    float4 vDiffColor : TEXCOORD0; ///< vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 vTCoord : TEXCOORD1; ///< vertex texture coords 
    float3 vNorm : TEXCOORD2;
};

VS_OUT1 RenderSceneNoShadowsVS(VS_IN invert)
{
    VS_OUT1 outvert;

    // transform the position from object space to clip space
    outvert.vPos = mul(float4(invert.vPos, 1), mViewProj);
    outvert.vNorm = invert.vNorm;
    outvert.vTCoord = invert.vTCoord;

    return outvert;
}
VS_OUT0 RenderSceneFastVS(VS_IN invert)
{
    VS_OUT0 outvert;

    // transform the position from object space to clip space
    outvert.vPos = mul(float4(invert.vPos, 1), mViewProj);
    outvert.vLightPos = mul(float4(invert.vPos, 1), mLightViewProjClip2Tex);
    // compute light direction
    float3 vLightDir = normalize(g_vLightPos - invert.vPos);
    // compute lighting
    outvert.vDiffColor = (g_vMaterialKd * g_vLightFlux);
    outvert.vDiffColor.xyz *= max(0, dot(invert.vNorm, vLightDir));
    outvert.vTCoord = invert.vTCoord;

    return outvert;
}
VS_OUT0 RenderSceneAccVS(VS_IN invert)
{
    VS_OUT0 outvert;

    // transform the position from object space to clip space
    outvert.vPos = mul(float4(invert.vPos, 1), mViewProj);
    outvert.vLightPos = mul(float4(invert.vPos, 1), mLightView);
    // compute light direction
    float3 vLightDir = normalize(g_vLightPos - invert.vPos);
    // compute lighting
    outvert.vDiffColor = (g_vMaterialKd * g_vLightFlux);
    outvert.vDiffColor.xyz *= max(0, dot(invert.vNorm, vLightDir));
    outvert.vTCoord = invert.vTCoord;

    return outvert;
}
float4 RenderDepthVS(VS_IN invert) : SV_Position
{
    return mul(float4(invert.vPos, 1), mViewProj);
}
float4 ConvertDepthVS(uint iv : SV_VertexID) : SV_Position
{
    return float4((iv << 1) & 2, iv & 2, 0.5, 1) * 2 - 1;
}
//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
float4 RenderSceneNoShadowsPS(VS_OUT1 infragm) : SV_Target0
{
    return 1;
}
float4 FastShadow(float3 vLightPos, float4 vDiffColor)
{
    // this is our white rectangle
    float2 vLightMax = vLightPos + g_fFilterSize;
    vLightPos.xy -= g_fFilterSize;
    uint pStackLow = 0xffffffff, pStackHigh;
    uint iPix = 0;
    uint3 iLevel = uint3(0, 0, N_LEVELS - 1);
    float fTotShadow = 1; ///< completely unshadowed in the beginning

    [loop] for ( ; ; )
    {
        uint2 iPixel = uint2(iLevel.x + (iPix & 1), iLevel.y + (iPix >> 1));
        float fDiag = (float)(1 << iLevel.z);
        float2 vTexMin = iPixel * fDiag;

        // shrink texel to the white rectangle size
        float2 vTexMax = min(vTexMin + fDiag, vLightMax);
        vTexMax -= max(vTexMin, vLightPos.xy);

        // fetch the depth map
        float2 vPixel = iPixel + 0.5;
        [flatten] if (iLevel.z != 0)


        { vPixel += float2(DEPTH_RES, 1 << (N_LEVELS - iLevel.z)); }
        vPixel /= float2(DEPTH_RES * 3 / 2, DEPTH_RES);
        float2 fMapMinMax = DepthMip2.SampleLevel(DepthSampler, vPixel, 0);

        // compute shadowing
        float fPotentialShadow = saturate(vTexMax.x * g_fDoubleFilterSizeRev) * saturate(vTexMax.y * g_fDoubleFilterSizeRev);
        bool bShadowed = (vLightPos.z >= fMapMinMax.y); // our z larger or equal than shadow map max
        [flatten] if (bShadowed) fTotShadow -= fPotentialShadow;
        if (fTotShadow <= 0)return float4(0, 0, 0, 1);

        // decide if we must go lower
        bool bNextLevel = (fPotentialShadow > 0) && (!bShadowed) && (vLightPos.z > fMapMinMax.x);
        [flatten] if (bNextLevel) iLevel.xy = iPixel + iPixel;
        // new values only rewrite old ones if we actually go to the next level
        bool bPushToStack = bNextLevel && (iPix < 3);
        [flatten] if (bPushToStack)
        {
            pStackHigh = (pStackLow & 0xfc000000) | (pStackHigh >> 6);
            pStackLow = (pStackLow << 6) | (iLevel.z << 2) | (iPix + 1);
        }
        iLevel.z -= bNextLevel; // go to more detailed level
        [flatten] if (bNextLevel) iPix = 0;
        else iPix += 1;

        // now get values from stack if necessary
        uint iPrevLevel = iLevel.z;
        [branch] if (iPix >= 4)
        {
            iLevel.z = (pStackLow >> 2) & 0xf;
            iPix = pStackLow & 3;
            pStackLow = (pStackLow >> 6) | (pStackHigh & 0xfc000000);
            pStackHigh <<= 6;
        }
        iLevel.xy = (iLevel.xy >> (iLevel.z - iPrevLevel)) & 0xfffffffe;
        if (iLevel.z == 0xf)
        {
            vDiffColor.xyz *= saturate(fTotShadow);
            return vDiffColor;
        }
    }
    return float4(1, 0, 0, 1);  // this is never reached, but compiler curses if the line is not here
}
float4 RenderSceneFastPS(VS_OUT0 In) : SV_Target0
{
    if (dot(In.vDiffColor.xyz, In.vDiffColor.xyz) == 0)
        return float4(0, 0, 0, 1);
    if (bTextured) In.vDiffColor.xyz *= DiffuseTex.Sample(DiffuseSampler, In.vTCoord);
    return FastShadow(In.vLightPos.xyz / In.vLightPos.w, In.vDiffColor);
}
float Shadow2D(float2 vMin, float2 vMax)
{
    vMin = max(vMin, -g_fFilterSize);
    vMax = min(vMax,  g_fFilterSize);
    return saturate((vMax.x - vMin.x) * g_fDoubleFilterSizeRev) * saturate((vMax.y - vMin.y) * g_fDoubleFilterSizeRev);
}
float4 AccurateShadow(float3 vLightPos, float4 vDiffColor)
{
    uint pStackLow = 0xffffffff, pStackHigh;
    uint iPix = 0;
    uint3 iLevel = uint3(0, 0, N_LEVELS - 1);

    float fTotShadow = 1;
    [loop] for (int ireps = 0; ireps < 4096; ++ireps)
    {
        uint2 iPixel = uint2(iLevel.x + (iPix & 1), iLevel.y + (iPix >> 1));
        float2 vPixel = iPixel + 0.5;
        [flatten] if (iLevel.z != 0)
        { vPixel += float2(DEPTH_RES, 1 << (N_LEVELS - iLevel.z)); }
        vPixel /= float2(DEPTH_RES * 3 / 2, DEPTH_RES);
        float2 vMinMax = DepthMip2.SampleLevel(DepthSampler, vPixel, 0);
        // we need to assure that there will be no gaps between this shadow texel and it's neighbours
        // to do that we will compute minimum depth on the edges as average between neighbours
        float4 vDepthMin = vMinMax.x;
        // convert depth to linear space
        vMinMax.y = 1. / (vMinMax.y * mLightProjClip2TexInv[2][3] + mLightProjClip2TexInv[3][3]);
        // here we remove light leaks by extending/shrinking shadow texel depending on the neighbours dept
        // this makes sense only at the finest mip level and for texels that are closer to light than the fragment is
        [branch] if (iLevel.z == 0 && vMinMax.y < vLightPos.z)
        {
            // we use here DepthTex0 because it is one float instead of two (should be cheaper to fetch)
            vPixel.x = (float)iPixel.x / DEPTH_RES;
            vDepthMin.x = min(vMinMax.x, DepthTex0.SampleLevel(DepthSampler, float2(vPixel.x - 1. / DEPTH_RES, vPixel.y), 0));
            vDepthMin.y = min(vMinMax.x, DepthTex0.SampleLevel(DepthSampler, float2(vPixel.x, vPixel.y + 1. / DEPTH_RES), 0));
            vDepthMin.z = min(vMinMax.x, DepthTex0.SampleLevel(DepthSampler, float2(vPixel.x + 1. / DEPTH_RES, vPixel.y), 0));
            vDepthMin.w = min(vMinMax.x, DepthTex0.SampleLevel(DepthSampler, float2(vPixel.x, vPixel.y - 1. / DEPTH_RES), 0));
        }
        // convert depth to linear space
        vDepthMin = 1. / (vDepthMin * mLightProjClip2TexInv[2][3] + mLightProjClip2TexInv[3][3]);

        // these are coordinates of shadow texel hanging in light space at their own depth
        float2 vMin = 0.5 - float2(iPixel.x, iPixel.y + 1) * g_fResRev[iLevel.z];
        float2 vMax = float2(vMin.x - g_fResRev[iLevel.z], vMin.y + g_fResRev[iLevel.z]);
        vMin *= 2 * float2(mLightProjClip2TexInv[3][0], mLightProjClip2TexInv[3][1]);
        vMax *= 2 * float2(mLightProjClip2TexInv[3][0], mLightProjClip2TexInv[3][1]);
        // vMin, vMax is shadow texel located at unit depth. now we project shadow texel to plane of light source
        vMin = (vMin * vLightPos.zz - vLightPos.xy) / (vLightPos.zz / vDepthMin.xy - 1);
        vMax = (vMax * vLightPos.zz - vLightPos.xy) / (vLightPos.zz / vDepthMin.zw - 1);

        float fShadow = Shadow2D(vMin, vMax);
        bool bNextLevel = (iLevel.z > 0 && fShadow > 0);
        [flatten] if (!bNextLevel) fTotShadow -= fShadow;
        if (fTotShadow <= 0 || (fShadow == 1 && vMinMax.y < vLightPos.z))
        { return float4(0, 0, 0, 1); } // the point is completely in shadow

        [flatten] if (bNextLevel) iLevel.xy = iPixel + iPixel;
        // new values only rewrite old ones if we actually go to the next level
        bool bPushToStack = bNextLevel & (iPix < 3);
        [flatten] if (bPushToStack)
        {
            pStackHigh = (pStackLow & 0xfc000000) | (pStackHigh >> 6);
            pStackLow = (pStackLow << 6) | iLevel.z * 4 | (iPix + 1);
        }
        iLevel.z -= bNextLevel; // go to more detailed level
        [flatten] if (bNextLevel) iPix = 0;
        else iPix += 1;

        // now get values from stack if necessary
        uint iPrevLevel = iLevel.z;
        [branch] if (iPix >= 4)
        {
            iLevel.z = (pStackLow >> 2) & 0xf;
            iPix = pStackLow & 3;
            pStackLow = (pStackLow >> 6) | (pStackHigh & 0xfc000000);
            pStackHigh *= 64;
        }
        iLevel.xy = (iLevel.xy >> (iLevel.z - iPrevLevel)) & 0xfffffffe;
        if (iLevel.z == 0xf)
        {
            vDiffColor.xyz *= saturate(fTotShadow);
            return vDiffColor;
        }
    }
    return float4(1, 0, 0, 1);
}
float4 RenderSceneAccPS(VS_OUT0 In) : SV_Target0
{
    if (dot(In.vDiffColor.xyz, In.vDiffColor.xyz) == 0)
        return float4(0, 0, 0, 1);
    [flatten] if (bTextured) In.vDiffColor *= DiffuseTex.Sample(DiffuseSampler, In.vTCoord);
    return AccurateShadow(In.vLightPos.xyz, In.vDiffColor);
}
float2 ConvertDepth2PS(float4 vPos : SV_Position) : SV_Target0
{
    float fDepth = DepthTex0.Load(uint3(vPos.x, vPos.y, 0));
    return float2(fDepth, fDepth);
}
float2 CreateMip2PS(float4 vPos : SV_Position) : SV_Target0
{
    uint3 iPos = uint3((int)vPos.x << 1, (int)vPos.y << 1, 0);
    float2 vDepth = DepthMip2.Load(iPos), vDepth1;
    ++iPos.x;
    vDepth1 = DepthMip2.Load(iPos);
    vDepth = float2(min(vDepth.x, vDepth1.x), max(vDepth.y, vDepth1.y));
    ++iPos.y;
    vDepth1 = DepthMip2.Load(iPos);
    vDepth = float2(min(vDepth.x, vDepth1.x), max(vDepth.y, vDepth1.y));
    --iPos.x;
    vDepth1 = DepthMip2.Load(iPos);
    vDepth = float2(min(vDepth.x, vDepth1.x), max(vDepth.y, vDepth1.y));
    return vDepth;
}
float2 ConvertToBigPS(float4 vPos : SV_Position) : SV_Target0
{
    uint3 iPos;
    if (vPos.x < DEPTH_RES)
    { // we fetch from the most detailed mip level
        iPos.xy = uint2(vPos.x, vPos.y);
        iPos.z = 0;
    }
    else
    { // compute the level from which we fetch
      iPos.z = N_LEVELS - (int)log2(vPos.y);
      iPos.x = (vPos.x - DEPTH_RES);
      iPos.y = (vPos.y - pow(2, N_LEVELS - iPos.z));
    }
    return DepthMip2.Load(iPos);
}

technique10 RenderNoShadows
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, RenderSceneNoShadowsVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, RenderSceneNoShadowsPS()));
		SetRasterizerState(RStateMSAAON);
    }
}
technique10 RenderFast
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, RenderSceneFastVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, RenderSceneFastPS()));
		SetRasterizerState(RStateMSAAON);
    }
}
technique10 RenderAcc
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, RenderSceneAccVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, RenderSceneAccPS()));
		SetRasterizerState(RStateMSAAON);
    }
}
// This technique renders depth
technique10 RenderDepth
{
    pass RenderDepth
    {
        SetVertexShader(CompileShader(vs_4_0, RenderDepthVS()));
        SetGeometryShader(NULL);
        SetPixelShader(NULL);
    }
}
// This technique creates depth mip
technique10 ReworkDepth2
{
    pass ConvertDepth
    {
        SetVertexShader(CompileShader(vs_4_0, ConvertDepthVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, ConvertDepth2PS()));
    }
    pass CreateMip
    {
        SetVertexShader(CompileShader(vs_4_0, ConvertDepthVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, CreateMip2PS()));
    }
    pass ConvertToBig
    {
        SetVertexShader(CompileShader(vs_4_0, ConvertDepthVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, ConvertToBigPS()));
    }
}
