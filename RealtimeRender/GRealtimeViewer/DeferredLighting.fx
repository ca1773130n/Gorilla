#define CACHE_TEXTURE_SIZE		2048.0
#define PI 3.141592653f
#define MICRO_RENDER_SIZE 16

Texture2DMS<float4, 4> g_rtAmbient;
Texture2DMS<float4, 4> g_rtColor;
Texture2DMS<float4, 4> g_rtSpecular;
Texture2DMS<float4, 4> g_rtSpecularAttr1;
Texture2DMS<float4, 4> g_rtTransparency;
Texture2DMS<float4, 4> g_rtCommon1;
Texture2DMS<float4, 4> g_rtCommon2;
Texture2DMS<float4, 4> g_rtNormal;

Texture2D<float4> g_rtAmbientM;
Texture2D<float4> g_rtColorM;
Texture2D<float4> g_rtSpecularM;
Texture2D<float4> g_rtSpecularAttr1M;
Texture2D<float4> g_rtTransparencyM;
Texture2D<float4> g_rtCommon1M;
Texture2D<float4> g_rtCommon2M;
Texture2D<float4> g_rtNormalM;

Texture2DArray<float4> g_rtAmbientMB;
Texture2DArray<float4> g_rtColorMB;
Texture2DArray<float4> g_rtSpecularMB;
Texture2DArray<float4> g_rtSpecularAttr1MB;
Texture2DArray<float4> g_rtTransparencyMB;
Texture2DArray<float4> g_rtCommon1MB;
Texture2DArray<float4> g_rtCommon2MB;
Texture2DArray<float4> g_rtNormalMB;

float4x4	g_mWorld;
float4x4	g_mView;           
float4x4	g_mProjection; 
float4x4	g_mViewProjection; 
float4x4	g_mWorldInverseTranspose;
float4x4	g_mShadow;

float3		g_vEyePos;
float3		g_vLightPos;
float3		g_vLightDir;
float3		g_vLightColor;
float3		g_vShadowColor;

float		g_fCosPenumbra;
float		g_fCosUmbra;
float		g_fAmbientShade;
float		g_fDropOff;
int			g_iDecay;
float		g_fNear;
float		g_fFar;
float		g_fLeft;
float		g_fTop;
float		g_fLightNear;
float		g_fLightFar;


int			g_iShaderID;
int			g_iLightFlag;
int			g_iShadowFlag;

int			g_iNumUseSample;

uniform float4	g_vAmbient;
uniform float4	g_vColor;
uniform float4	g_vSpecular;
uniform float4	g_vSpecularAttr1;
uniform float4	g_vTransparency;
uniform float4	g_vCommon1;
uniform float4	g_vCommon2;
uniform float4	g_vNormal;

Texture2D		g_txAmbient;
Texture2D		g_txColor;
Texture2D		g_txSpecular;
Texture2D		g_txSpecularAttr1;
Texture2D		g_txTransparency;
Texture2D		g_txCommon1;
Texture2D		g_txCommon2;
Texture2D		g_txNormal;

Texture2D		g_txQuad;
//Texture2D<float>		g_txShadowMap;
Texture2D		g_txOffScreen;



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
        { return float4(1, 0, 0, 1); } // the point is completely in shadow

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
    return float4(0, 1, 0, 1);
}
float2 ConvertDepth2PS(float4 vPos : SV_Position) : SV_Target0
{
    float fDepth = DepthTex0.Load(uint3(vPos.x, vPos.y, 0));
	fDepth /= g_fLightFar;
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




//=================================================================================================================================
// States
//=================================================================================================================================

shared RasterizerState noCullRS
{
    CullMode = None;
    FillMode = Solid;
    MultisampleEnable = false;
};

shared RasterizerState backCullRS
{
    CullMode = Back;
    FillMode = Solid;
    MultisampleEnable = false;
};

shared RasterizerState frontCullRS
{
    CullMode = Front;
    FillMode = Solid;
    MultisampleEnable = false;
};

shared RasterizerState noCullMSAARS
{
    CullMode = None;
    FillMode = Solid;
    MultisampleEnable = True;
};

shared RasterizerState backCullMSAARS
{
    CullMode = Back;
    FillMode = Solid;
    MultisampleEnable = True;
};

shared DepthStencilState zWriteNoStencilDS
{
    DepthEnable = true;
    DepthFunc = Less_Equal;
    DepthWriteMask = All;
    StencilEnable = false;
};

shared BlendState RGBABS
{
    RenderTargetWriteMask[0] = 0x0f;
    BlendEnable[0] = false;
};

RasterizerState DisableCulling
{
    CullMode = NONE;
};

RasterizerState EnableMultisampling
{
	CullMode = BACK;
	MultiSampleEnable = TRUE;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState SrcAlphaBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = One;
    DestBlend = One;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState DisableDepthTestWrite
{
    DepthEnable = FALSE;
    DepthWriteMask = 0;
};

DepthStencilState DepthLessEqual
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
	
};
DepthStencilState DepthGreater
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = GREATER;
};
DepthStencilState DepthEqual
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = EQUAL;
};
DepthStencilState DepthDisable
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

//=================================================================================================================================
// Samplers
//=================================================================================================================================

SamplerState samLinear
{
    Filter = ANISOTROPIC;//MIN_MAG_MIP_LINEAR;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samAniso
{
    Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerComparisonState PCF_Sampler
{
   Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
   AddressU = MIRROR;
   AddressV = MIRROR;
   ComparisonFunc = LESS;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

/************************************************************************/
/* FillMRT                                                              */
/************************************************************************/

struct MRT_VS_INPUT
{
    float3 vPos			: POSITION;
    float3 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
    float2 Tex			: TEXCOORD;	
};

struct MRT_PS_INPUT
{
	float4 vPos		: SV_POSITION;
	float3 vNormal	: TEXCOORD0;
    float2 Tex		: TEXCOORD1;    
    float2 depth	: TEXCOORD2;
	float3 vTangent	: TEXCOORD3;
	float3 vBinormal: TEXCOORD4;
};

struct MRT_PS_OUTPUT
{	
	float4 color0	: SV_TARGET0;
	float4 color1	: SV_TARGET1;
	float4 color2	: SV_TARGET2;
	float4 color3	: SV_TARGET3;
	float4 color4	: SV_TARGET4;
	float4 color5	: SV_TARGET5;
	float4 color6	: SV_TARGET6;
	float4 color7	: SV_TARGET7;
};

struct MRTH_PS_OUTPUT
{	
	half4 color0	: SV_TARGET0;
	half4 color1	: SV_TARGET1;
	half4 color2	: SV_TARGET2;
	half4 color3	: SV_TARGET3;
	half4 color4	: SV_TARGET4;
	half4 color5	: SV_TARGET5;
	half4 color6	: SV_TARGET6;
	half4 color7	: SV_TARGET7;
};

struct DND_PS_OUTPUT
{	
	float4 color0	: SV_TARGET0;
	float4 color1	: SV_TARGET1;
};

struct VS_INPUT
{
	float3 Pos		: POSITION;	
	float3 vNormal	: NORMAL;
	float3 vTangent	: TANGENT;
	float2 Tex		: TEXCOORD;		
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;	
	float2 Tex		: TEXCOORD0;
};

/*
float3x3 matInvTranspose(float3x3 inMatrix)
{  	
   float fdet = determinant(inMatrix);//dot(cross(inMatrix[0], inMatrix[1]), inMatrix[2]);
   mat3 T = transpose(inMatrix);
   return float3x3(cross(T[1], T[2]), cross(T[2], T[0]), cross(T[0], T[1])) / fdet;
}
*/
MRT_PS_INPUT FillMRT_VS( MRT_VS_INPUT input )
{
	MRT_PS_INPUT output;
	
	output.vPos = mul( float4(input.vPos.xyz, 1), g_mViewProjection );
    output.Tex = input.Tex;
    output.vNormal = input.vNormal;
	output.vTangent = input.vTangent;
	output.vBinormal = cross( input.vTangent, input.vNormal );
    output.depth.x = mul( float4(input.vPos.xyz, 1), g_mView ).z;

    return output;
}

MRT_PS_OUTPUT FillMRT_PS( MRT_PS_INPUT input )
{
	MRT_PS_OUTPUT output;
	
	// Ambient color is indirect illumination on RT. alpha channel should be used for SSAO
	output.color0 = g_vAmbient.x == -9999.f ? g_txAmbient.Sample( samLinear, input.Tex ) : g_vAmbient;	

	// The alpha channel of Color pass is used for the shader ID
	output.color1 = g_vColor.x == -9999.f ? g_txColor.Sample( samLinear, input.Tex ) : g_vColor;		
	output.color1.w = 1.0 / float( g_iShaderID + 1 );

	output.color2 = g_vSpecular.x == -9999.f ? g_txSpecular.Sample( samLinear, input.Tex ) : g_vSpecular;
	output.color3 = g_vSpecularAttr1.x == -9999.f ? g_txSpecularAttr1.Sample( samLinear, input.Tex ) : g_vSpecularAttr1;
	output.color4 = g_vTransparency.x == -9999.f ? g_txTransparency.Sample( samLinear, input.Tex ) : g_vTransparency;

	output.color5 = g_vCommon1.x == -9999.f ? g_txCommon1.Sample( samLinear, input.Tex ) : g_vCommon1;
	output.color6 = g_vCommon2.x == -9999.f ? g_txCommon2.Sample( samLinear, input.Tex ) : g_vCommon2;

	// Bump process
	float3 vNormal = input.vNormal;
	if( g_vNormal.x == -9999.f )
	{
		float3x3 TBN = float3x3( input.vTangent, input.vBinormal, vNormal );
		float3 vReadNormal = g_txNormal.Sample( samLinear, float2(input.Tex.x, input.Tex.y) ).xyz;
		vReadNormal = 2.0 * vReadNormal - 1.0;
		vReadNormal.z = sqrt(1.0 - dot(vReadNormal.xy, vReadNormal.xy));
		vNormal = normalize( mul(vReadNormal, TBN) );
	}
	// Normal pass has NDC z value in the alpha channel
	
	output.color7 = float4( vNormal, input.depth.x / g_fFar );
	
	return output;
}

int g_iMicroTexIndex;
MRTH_PS_OUTPUT DrawToBigTexture_PS( PS_INPUT input )
{
	MRTH_PS_OUTPUT output;
	int3 msCoord = int3( int2(input.Pos.xy), 0 );

	int2 coord = int2( g_iMicroTexIndex % 128 * MICRO_RENDER_SIZE, g_iMicroTexIndex / 128 * MICRO_RENDER_SIZE );
	int2 innerCoord = int2( msCoord.x - coord.x, msCoord.y - coord.y );
	
	if( innerCoord.x >= 0 && innerCoord.x < MICRO_RENDER_SIZE && innerCoord.y >= 0 && innerCoord.y < MICRO_RENDER_SIZE )
	{
		output.color0 = g_rtAmbient.Load( int3(innerCoord, 0), 0 );
		output.color1 = g_rtColor.Load( int3(innerCoord, 0), 0 );
		output.color2 = g_rtSpecular.Load( int3(innerCoord, 0), 0 );
		output.color3 = g_rtSpecularAttr1.Load( int3(innerCoord, 0), 0 );
		output.color4 = g_rtTransparency.Load( int3(innerCoord, 0), 0 );
		output.color5 = g_rtCommon1.Load( int3(innerCoord, 0), 0 );
		output.color6 = g_rtCommon2.Load( int3(innerCoord, 0), 0 );
		output.color7 = g_rtNormal.Load( int3(innerCoord, 0), 0 );
	}
	else
	{
		output.color0 = half4( 0, 0, 0, 1 );
		output.color1 = half4( 0, 0, 0, 1 );
		output.color2 = half4( 0, 0, 0, 1 );
		output.color3 = half4( 0, 0, 0, 1 );
		output.color4 = half4( 0, 0, 0, 1 );
		output.color5 = half4( 0, 0, 0, 1 );
		output.color6 = half4( 0, 0, 0, 1 );
		output.color7 = half4( 0, 0, 0, 1 );
	}
	
	return output;
}

DND_PS_OUTPUT DrawNormalDepth_PS( PS_INPUT input )
{
	DND_PS_OUTPUT output;
	int3 msCoord = int3( int2(input.Pos.xy), 0 );
	float3 vNormal = float3( 0, 0, 0 );
	float3 vPos = float3( 0, 0, 0 );

	for( int i=0; i < 4; i++ )
	{
		float4 NN = g_rtNormal.Load( msCoord, i );		
		float3 N = NN.xyz;
		N = normalize( N );
		float Z = NN.w;	

		float3 P;		
		P.x = -g_fLeft;
		P.y = g_fTop;
		P.z = Z;

		P.x = P.x * ( input.Tex.x * 2.0 - 1.0 ) / g_fNear;
		P.y = P.y * ( (1 - input.Tex.y) * 2.0  - 1.0 ) / g_fNear;	
		P.z = P.z * g_fFar;
		P.xy = P.xy * P.z;

		P = mul(float4(P, 1), g_mWorldInverseTranspose).xyz;

		vNormal += N;
		vPos += P;
	}
	
	
	output.color0 = float4( vNormal/4, 1 );
	output.color1 = float4( vPos/4, 1 );
	
	return output;	
}

/************************************************************************/
/* ResolveMRT                                                           */
/************************************************************************/

float3 BlinnSpecular( float Eccentricity, float Rolloff, float3 N, float3 L, float3 V, float3 LightOut, float3 SpecularColor )
{
	float NL = dot( L, N );
	float NV = dot( V, N );
	float specularCoefficient;
	float3 spec_attr = float3( Eccentricity, Rolloff, 0 );
	
	if( NL > 0.0 )
	{
		float3 H = normalize(L+V);

		float HV = dot(H, V);
		float HN = dot(H, N);

		float computedEcc = spec_attr.x * spec_attr.x - 1.0;
		float Dd = ( computedEcc + 1.0) / ( 1.0 + computedEcc * HN * HN );
		Dd    = Dd * Dd;
		HN = 2.0 * HN;

		float Gg;
		if (NV < NL) 
			Gg = (NV*HN < HV) ? HN / HV : 1.0 / NV;
		else 
			Gg = (NL*HN < HV) ? NL * HN / (HV * NV) : 1.0 / NV;
		HV = 1.0 - HV;
		HV = HV * HV * HV;
		float Ff = HV + (1.0 - HV) * spec_attr.y;
		specularCoefficient = max(Dd * Gg * Ff, 0.0);
	}
	else
	{
		specularCoefficient = 0.0;
	}
	return LightOut * SpecularColor * specularCoefficient;
}

float3 PhongSpecular( float cPower, float Reflectivity, float3 N, float3 L, float3 V, float3 LightOut, float3 SpecularColor )
{
	float NL = dot( N, L );
	float3 R = normalize( (2.0 * NL * N) - L );
	float RV = dot( R, V );
	
	if( RV < 0.0 )
	{
		return float3( 0.0, 0.0, 0.0 );
	}
	else
	{    
		return LightOut * SpecularColor * pow( abs(RV), cPower );
	}
}


#define BLOCKER_SEARCH_NUM_SAMPLES 16
#define PCF_NUM_SAMPLES 16
#define NEAR_PLANE 9.5
#define LIGHT_WORLD_SIZE .5
#define LIGHT_FRUSTUM_WIDTH 3.75
	// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)
Texture2D<float> tDepthMap;
cbuffer POISSON_DISKS
{
	float2 poissonDisk[16] = {
		float2( -0.94201624, -0.39906216 ),
		float2( 0.94558609, -0.76890725 ),
		float2( -0.094184101, -0.92938870 ),
		float2( 0.34495938, 0.29387760 ),
		float2( -0.91588581, 0.45771432 ),
		float2( -0.81544232, -0.87912464 ),
		float2( -0.38277543, 0.27676845 ),
		float2( 0.97484398, 0.75648379 ),
		float2( 0.44323325, -0.97511554 ),
		float2( 0.53742981, -0.47373420 ),
		float2( -0.26496911, -0.41893023 ),
		float2( 0.79197514, 0.19090188 ),
		float2( -0.24188840, 0.99706507 ),
		float2( -0.81409955, 0.91437590 ),
		float2( 0.19984126, 0.78641367 ),
		float2( 0.14383161, -0.14100790 )
	};
};
float PenumbraSize(float zReceiver, float zBlocker) //Parallel plane estimation
{
	return (zReceiver - zBlocker) / zBlocker;
}
void FindBlocker(out float avgBlockerDepth,
	out float numBlockers,
	float2 uv, float zReceiver )
{
	//This uses similar triangles to compute what //area of the shadow map we should search
	float searchWidth = LIGHT_SIZE_UV * (zReceiver - NEAR_PLANE) / zReceiver;
	float blockerSum = 0;
	numBlockers = 0;
	for( int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i )
	{
		float shadowMapDepth = tDepthMap.SampleLevel(
			PointSampler,
			uv + poissonDisk[i] * searchWidth,
			0);
		if ( shadowMapDepth < zReceiver ) {
			blockerSum += shadowMapDepth;
			numBlockers++;
		}
	}
	avgBlockerDepth = blockerSum / numBlockers;
}
float PCF_Filter( float2 uv, float zReceiver, float filterRadiusUV )
{
	float sum = 0.0f;
	for ( int i = 0; i < PCF_NUM_SAMPLES; ++i )
	{
		float2 offset = poissonDisk[i] * filterRadiusUV;
		sum += tDepthMap.SampleCmpLevelZero(PCF_Sampler, uv + offset, zReceiver);
	}
	return sum / PCF_NUM_SAMPLES;
}
float PCSS ( float3 coords )
{
	float2 uv = coords.xy;
	float zReceiver = coords.z; // Assumed to be eye-space z in this code

	// STEP 1: blocker search
	float avgBlockerDepth = 0;
	float numBlockers = 0;
	FindBlocker( avgBlockerDepth, numBlockers, uv, zReceiver );
	if( numBlockers < 1 )
		//There are no occluders so early out (this saves filtering)
		return 1.0f;
	// STEP 2: penumbra size
	float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
	float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV * NEAR_PLANE / coords.z;
	// STEP 3: filtering
	return PCF_Filter( uv, zReceiver, filterRadiusUV );
}











bool ShadowTest( float3 P )
{
	/*
	bool bShadowFlag = false;
	bool flag = false;

	float4 posLightView = mul( g_mShadow, float4(P, 1) );
	float depthCam = posLightView.z / g_fLightFar;
	float3 rayLightView;
	rayLightView = posLightView.xyz;
	rayLightView /= g_fLightFar;
	//rayLightView *= g_fLightNear;
	rayLightView.x = rayLightView.x / 2.0 + 0.5;
	rayLightView.y = rayLightView.y / 2.0 + 0.5;//1.0 - (rayLightView.x / 2.0 + 0.5);

	if( rayLightView.x < 0 || rayLightView.x > 1 || rayLightView.y < 0 || rayLightView.y > 1 )
		return false;
	else
	{
		float depthLight = g_txShadowMap.Sample( samLinear, rayLightView.xy );
		if( depthLight - 0.00005 < depthCam )
			bShadowFlag = true;
	}
	

	

	
	float2 l_codi;
	l_codi[0] = l_p.x / l_p.w;
	l_codi[1] = l_p.y / l_p.w;
	float l_dep = l_p.z / l_p.w;
	l_codi[0] = (l_codi[0] + 1.0) * 0.5;
	l_codi[1] = (l_codi[1] + 1.0) * 0.5;

	if( l_codi[0] < 0.0 || l_codi[0] > 1.0 )
	{
		flag = true;
	}
	if( l_codi[1] < 0.0 || l_codi[1] > 1.0 )
	{
		flag = true;
	}
	if( !flag )
	{
		float cal_dep = g_txShadowMap.Sample( samLinear, l_codi ).x;
		cal_dep = (cal_dep * 2.0) - 1.0;
		if( cal_dep > 0.9999999 )
		{
			bShadowFlag = false;
		}
		else if( l_dep > 0.9999999 )
		{
			bShadowFlag = false;
		}
		else if( cal_dep < (l_dep) - 0.00005 && g_iLightFlag == 3 )
		{
			bShadowFlag = true;
		}
		else if( cal_dep < (l_dep) - 0.005 && g_iLightFlag == 2 )
		{
			bShadowFlag = true;
		}
		else
		{
			bShadowFlag = false;
		}
	}
	*/
	bool bShadowFlag = false;
	return bShadowFlag;
}

PS_INPUT ResolveMRT_VS( VS_INPUT Input ) 
{
	PS_INPUT Out;
	Out.Tex = Input.Tex;
	Out.Pos = float4( Input.Pos, 1 );
	return Out;
}

float4 ResolveMRT_PS( PS_INPUT Input ) : SV_Target
{		
	float4 FinalColor = float4( 0, 0, 0, 0 );
	int3 msCoord = int3( int2(Input.Pos.xy), 0 );

	for( int iSample = 0; iSample < 4; iSample++ )
	{
		//if( iSample >= g_iNumUseSample ) break;

		float4 NN = g_rtNormal.Load( msCoord, iSample );		
		float3 N = NN.xyz;
		N = normalize( N );
		float Z = NN.w;	

		float3 P;		
		P.x = -g_fLeft;
		P.y = g_fTop;
		P.z = Z;

		P.x = P.x * ( Input.Tex.x * 2.0 - 1.0 ) / g_fNear;
		P.y = P.y * ( (1 - Input.Tex.y) * 2.0  - 1.0 ) / g_fNear;	
		P.z = P.z * g_fFar;
		P.xy = P.xy * P.z;

		P = mul(float4(P, 1), g_mWorldInverseTranspose).xyz;
		float3 V = normalize( g_vEyePos - P );
		
		float4 CC = g_rtColor.Load( msCoord, iSample );
		float3 Color = CC.xyz;
		float fShading = CC.w;
		float3 Ambient = g_rtAmbient.Load( msCoord, iSample ).xyz;
		float3 Specular = g_rtSpecular.Load( msCoord, iSample ).xyz;
		float3 SpecularAttr1 = g_rtSpecularAttr1.Load( msCoord, iSample ).xyz;
		float3 Transparency = g_rtTransparency.Load( msCoord, iSample ).xyz;
		
		float4 Common1 = g_rtCommon1.Load( msCoord, iSample );
		float Diffuse = Common1.x;	
		float RefractiveIndex = Common1.y;
		float SurfaceThickness = Common1.z;		
		
		float4 Common2 = g_rtCommon2.Load( msCoord, iSample );

		bool bLightOff = false;
		float Attenuation = 0;	
		float Alpha = 1;
		float3 L;
		float Distance;
		float NL;

		float4 OutColor = float4( 0, 0, 0, Alpha );

		if( g_iLightFlag == 50 )
		{
			OutColor = float4( Ambient * Color, Alpha );
		}
		else
		{
			// Shadow Test
			bool bShadowFlag = false;

		
			/*
			if( g_iShadowFlag == 1 )
			{
				bShadowFlag = ShadowTest( P );
			}
			else
			{
				bShadowFlag = false;
			}

			if( bShadowFlag )
			{
				OutColor = float4( 0, 0, 0, Alpha );					
			}
			*/
			//else
			{
				bLightOff = false;
				if( g_iLightFlag == 0 )
				{
					float AmbientShade = 0.0;
					bLightOff = true;
					L = normalize( g_vLightPos - P );
					NL = dot( N, L );
					AmbientShade = g_fAmbientShade * ( NL - 1.0 );

					float3 tmpAmb;
					tmpAmb = float3( AmbientShade + 1.0, AmbientShade + 1.0, AmbientShade + 1.0 );
					tmpAmb = tmpAmb * g_vLightColor * Color;
					OutColor = float4( tmpAmb, Alpha );
				}
				else if( g_iLightFlag == 1 || g_iLightFlag == 3 )
				{
					Distance = distance( g_vLightPos, P );
					Attenuation = 1.0 / pow( abs(Distance), float(g_iDecay) );		
					L = normalize( g_vLightPos - P );

					if( g_iLightFlag == 3 )
					{			
						float co = dot( L, g_vLightDir );
						if( co < g_fCosPenumbra )
						{
							bLightOff = true;
							OutColor = float4( 0.0, 0.0, 0.0, Alpha );
						}
						else
						{
							float tmpSpot = pow( abs(co), g_fDropOff );
							if( co < g_fCosUmbra )
								tmpSpot = tmpSpot * (co - g_fCosPenumbra) / (g_fCosUmbra - g_fCosPenumbra);
							Attenuation = Attenuation * tmpSpot;
						}	
					}
				}
				else if( g_iLightFlag == 2 )
				{
					L = g_vLightDir;	
					Attenuation = 1.0;		
				}
				Attenuation = saturate( Attenuation );		

				float3 vLightColor = g_vLightColor * Attenuation;
				
				float3 Lambert = float3( 0, 0, 0 );

				if( !bLightOff )
				{
					NL = dot( N, L );
					if( NL < 0.0 ) NL = 0.0;			

					Lambert = Color * vLightColor * float3( Diffuse, Diffuse, Diffuse ) * NL;	

					// Lambert
					if( fShading == 1.0 || NL < 0.0001 )
					{
						OutColor.rgb = Lambert;
					}
					// Blinn
					else if( fShading == (1.0 / 2.0) )
					{
						float Eccentricity = SpecularAttr1.x;
						float Rolloff = SpecularAttr1.y;
						OutColor.rgb = Lambert + BlinnSpecular( Eccentricity, Rolloff, N, L, V, vLightColor, Specular );		
					}
					// Phong
					else if( fShading == (1.0 / 3.0) )
					{
						float cPower = SpecularAttr1.x * 100.0;
						float Reflectivity = SpecularAttr1.y;
						
						OutColor.rgb = Lambert + PhongSpecular( cPower, Reflectivity, N, L, V, vLightColor, Specular );
					}	
					else
					{			
						float Eccentricity = SpecularAttr1.x;
						float Rolloff = SpecularAttr1.y;
						OutColor.rgb = Lambert + BlinnSpecular( Eccentricity, Rolloff, N, L, V, vLightColor, Specular );		
					}
				}
				if( g_iShadowFlag == 1 )
				{
					
					float4 posLightView = mul( float4(P, 1), g_mShadow );
					float depthCam = posLightView.z / g_fLightFar;
					float3 rayLightView;
					rayLightView = posLightView.xyz;
					//rayLightView *= g_fLightNear;
					rayLightView.x = (rayLightView.x * 2.0 - 1.0) / g_fLightNear;
					rayLightView.y = ((1 - rayLightView.y) * 2.0 - 1.0) / g_fLightNear;
					rayLightView.z *= g_fLightFar;
					rayLightView.xy *= rayLightView.z;

					float depthLight = tDepthMap.Sample( samLinear, rayLightView.xy );
					//depthLight /= g_fLightFar;
					if( depthLight < depthCam )
					{
						return OutColor = float4(0,0,0,1);
					}
					
					//OutColor.rgb = float4(0,0,0,1);//*= PCSS( rayLightView );
					//return float4(1,1,1,PCSS(rayLightView));
					//return float4(1,1,1,1);//tDepthMap.Load( msCoord, iSample );
					
					return AccurateShadow( mul(float4(P, 1), g_mShadow).xyz, float4(Lambert, 1) );
				}
			}		
		}		
		FinalColor += OutColor;
	}

	FinalColor /= 4;
	return FinalColor;
}

/************************************************************************/
/* DepthRender                                                          */
/************************************************************************/

struct DepthRender_VS_INPUT
{
    float3 vPos			: POSITION;
    float3 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
    float2 Tex			: TEXCOORD;
};

struct DepthRender_PS_INPUT
{
	float4 vPos		: SV_POSITION;
    float  depth	: TEXCOORD0;	
};

struct DepthRender_PS_OUTPUT
{	
	float depth		: SV_Depth;
    //float  depth	: SV_Depth;	
};

DepthRender_PS_INPUT DepthRender_VS( DepthRender_VS_INPUT input )
{
	DepthRender_PS_INPUT output;
	
	output.vPos = mul( float4(input.vPos.xyz, 1), g_mViewProjection );
    output.depth = output.vPos.z / output.vPos.w;
    return output;
}

float DepthRender_PS( DepthRender_PS_INPUT input ) : SV_Depth
{
	return input.depth;
}

/************************************************************************/
/* DrawQuad                                                             */
/************************************************************************/

PS_INPUT DrawQuad_VS( VS_INPUT input )
{
	PS_INPUT Out;
	Out.Tex = input.Tex;
	Out.Pos = float4( input.Pos, 1 );
	return Out;
}

float4 DrawQuad_PS( PS_INPUT input ) : SV_Target
{
	return g_txQuad.Sample( samLinear, input.Tex );
}

//=================================================================================================================================
// Techniques
//=================================================================================================================================

technique10 FillMRT
{
    pass P0
    {		
        SetVertexShader( CompileShader( vs_4_0, FillMRT_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, FillMRT_PS() ) );

		SetDepthStencilState( DepthLessEqual, 0 );
    }    
}

technique10 DepthRender
{
    pass P0
    {		
        SetVertexShader( CompileShader( vs_4_0, RenderDepthVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( NULL );// CompileShader( ps_4_0, DepthRender_PS() ) );

		//SetDepthStencilState( DepthLessEqual, 0 );
        //SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 1.0f ), 0xFFFFFFFF );		
		 
        SetRasterizerState( noCullRS );
        SetBlendState( RGBABS, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xffffffff );
        SetDepthStencilState( zWriteNoStencilDS, 0xffffffff );
    }    
}

technique10 ResolveMRT
{
    pass P0
    {		
        SetVertexShader( CompileShader( vs_4_0, ResolveMRT_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ResolveMRT_PS() ) );  

		SetBlendState( SrcAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );	
		SetDepthStencilState( DisableDepthTestWrite, 0xffffffff );
    }        	
}

technique10 DrawNormalDepth
{
    pass P0
    {		
        SetVertexShader( CompileShader( vs_4_0, DrawQuad_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, DrawNormalDepth_PS() ) );  

		SetDepthStencilState( DepthLessEqual, 0 );
	}        	
}

technique10 DrawQuad
{
    pass P0
    {		
        SetVertexShader( CompileShader( vs_4_0, DrawQuad_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, DrawQuad_PS() ) );  

		SetBlendState( SrcAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );	
    }        	
}

technique10 MicroRender
{
    pass RenderSmall
    {		
        SetVertexShader( CompileShader( vs_4_0, FillMRT_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, FillMRT_PS() ) );

		SetDepthStencilState( DepthLessEqual, 0 );
    }        	
	pass RenderBig
	{
		SetVertexShader( CompileShader( vs_4_0, DrawQuad_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, DrawToBigTexture_PS() ) );

		SetBlendState( SrcAlphaBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );	
	}
}




































