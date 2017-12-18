#define PI 3.141592653f

Texture2DMS<float4, 4> g_rtAmbient;
Texture2DMS<float4, 4> g_rtColor;
Texture2DMS<float4, 4> g_rtSpecular;
Texture2DMS<float4, 4> g_rtSpecularAttr1;
Texture2DMS<float4, 4> g_rtTransparency;
Texture2DMS<float4, 4> g_rtCommon1;
Texture2DMS<float4, 4> g_rtCommon2;
Texture2DMS<float4, 4> g_rtNormal;
/*
Texture2D g_rtAmbient;
Texture2D g_rtColor;
Texture2D g_rtSpecular;
Texture2D g_rtSpecularAttr1;
Texture2D g_rtTransparency;
Texture2D g_rtCommon1;
Texture2D g_rtCommon2;
Texture2D g_rtNormal;
*/

float4x4	g_mWorld;
float4x4	g_mView;           
float4x4	g_mProjection; 
float4x4	g_mViewProjection; 
float4x4	g_mWorldInverseTranspose;
float4x4	g_mPersInv;

float3		g_vEyePos;
float3		g_vLightPos;
float3		g_vLightDir;
float3		g_vLightColor;
float3		g_vShadowColor;

float	g_fCosPenumbra;
float	g_fCosUmbra;
float	g_fAmbientShade;
float	g_fDropOff;
int		g_iDecay;
float	g_fNear;
float	g_fFar;
float	g_fLeft;
float	g_fTop;

uniform float4	g_vAmbient;
uniform float4	g_vColor;
uniform float4	g_vSpecular;
uniform float4	g_vSpecularAttr1;
uniform float4	g_vTransparency;
uniform float4	g_vCommon1;
uniform float4	g_vCommon2;
uniform float4	g_vCommon3;

int		g_iShaderID;
int		g_iLightFlag;
int		g_iShadowFlag;

Texture2D		g_txAmbient;
Texture2D		g_txColor;
Texture2D		g_txSpecular;
Texture2D		g_txSpecularAttr1;
Texture2D		g_txTransparency;
Texture2D		g_txCommon1;
Texture2D		g_txCommon2;
Texture2D		g_txQuad;

//=================================================================================================================================
// States
//=================================================================================================================================

RasterizerState DisableCulling
{
    CullMode = NONE;
};

RasterizerState EnableMultisampling
{
    MultisampleEnable = TRUE;
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
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
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

//--------------------------------------------------------------------------------------
// Texture sampler
//--------------------------------------------------------------------------------------
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// Vertex shader input structure
//--------------------------------------------------------------------------------------
struct MRT_VS_INPUT
{
    float3 vPos			: POSITION;
    float3 vNormal		: NORMAL;
    float2 Tex			: TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Pixel shader input structure
//--------------------------------------------------------------------------------------
struct MRT_PS_INPUT
{
	float4 vPos		: SV_POSITION;
	float3 vNormal	: TEXCOORD0;
    float2 Tex		: TEXCOORD1;    
    float2 depth	: TEXCOORD2;	
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

struct VS_INPUT
{
	float3 Pos		: POSITION;	
	float3 vNormal	: NORMAL;
	float2 Tex		: TEXCOORD;		
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;	
	float2 Tex		: TEXCOORD0;
	float4 toFar	: TEXCOORD1;
};

struct DQ_PS_INPUT
{
	float4 Pos		: SV_POSITION;	
	float2 Tex		: TEXCOORD;
};

//-----------------------------------------------------------------------------
// Texture samplers
//-----------------------------------------------------------------------------
MRT_PS_INPUT FillMRT_VS( MRT_VS_INPUT input )
{
	MRT_PS_INPUT output;
	
	output.vPos = mul( float4(input.vPos.xyz, 1), g_mViewProjection );
    output.Tex = input.Tex;
    output.vNormal = normalize( mul(float4(input.vNormal, 1), g_mWorldInverseTranspose).xyz );
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
	output.color3 = output.color3 / 100.0;
	output.color4 = g_vTransparency.x == -9999.f ? g_txTransparency.Sample( samLinear, input.Tex ) : g_vTransparency;

	output.color5 = g_vCommon1.x == -9999.f ? g_txCommon1.Sample( samLinear, input.Tex ) : g_vCommon1;
	output.color6 = g_vCommon2.x == -9999.f ? g_txCommon2.Sample( samLinear, input.Tex ) : g_vCommon2;

	// Normal pass has NDC z value in the alpha channel
	output.color7 = float4( input.vNormal, input.depth.x / g_fFar );
	
	return output;
}

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
	float RV;
	
	float3 H = normalize( V + L );
	float NH = dot( N, H );
	
	if( NH < 0.0 || cPower < 2.0 )
	{
		return float3( 0.0, 0.0, 0.0 );
	}
	else
	{    
		return LightOut * SpecularColor * pow( abs(NH), cPower );
	}
}
/*
float GetDepth( float2 texCoord, int iSample )
{
	int2 coord;
	coord.x = (int)(texCoord.x * 640);
	coord.y = (int)(texCoord.y * 480);

	return 0;// g_rtNormal.Load( int3(coord, 0), 0 ).w;
}

float4 LoadV( float2 texCoord )
{
	float cal_depth = GetDepth( texCoord );
	float4 depth_temp;
	depth_temp.x = ( texCoord.x * 2.0 ) - 1.0;
	depth_temp.y = ( (1 - texCoord.y) * 2.0 ) - 1.0;
	depth_temp.z = cal_depth;
	depth_temp.w = 1.0;	

	return depth_temp;	
}
*/

PS_INPUT ResolveMRT_VS( VS_INPUT Input ) 
{
	PS_INPUT Out;
	Out.Tex = Input.Tex;
	Out.Pos = float4( Input.Pos, 1 );
	Out.toFar = mul( g_mPersInv, float4( Input.Pos, 1 ) );
	Out.toFar /= Out.toFar.w;
	return Out;
}

float4 ResolveMRT_PS( PS_INPUT Input ) : SV_Target
{			
	int3 msCoord = int3( int2(Input.Pos.xy), 0 );

	float4 NN = g_rtNormal.Load( msCoord, 0 ) + g_rtNormal.Load( msCoord, 1 ) + g_rtNormal.Load( msCoord, 2 ) + g_rtNormal.Load( msCoord, 3 );
	NN /= 4.0;
	float3 N = NN.xyz;
	//N = normalize( N );
	float Z = NN.w;	

	float3 P;		
	P.x = -g_fLeft;
	P.y = g_fTop;
	P.z = Z;

	P.x = P.x * ( Input.Tex.x * 2.0 - 1.0 ) / g_fNear;
	P.y = P.y * ( (1 - Input.Tex.y) * 2.0  - 1.0 ) / g_fNear;	
	P.z = P.z * g_fFar;
	P.xy = P.xy * P.z;
		
	float3 V = normalize( -P );
	
	float4 CC = g_rtColor.Load( msCoord, 0 );// + g_rtColor.Load( msCoord, 1 ) + g_rtColor.Load( msCoord, 2 ) + g_rtColor.Load( msCoord, 3 );
	float3 Color = CC.xyz;
	float fShading = CC.w;
	float3 Ambient = g_rtAmbient.Load( int3(Input.Tex.x * 640, Input.Tex.y * 480, 0), 0 ).xyz;
	float3 Specular = g_rtSpecular.Load( int3(Input.Tex.x * 640, Input.Tex.y * 480, 0), 0 ).xyz;
	float3 SpecularAttr1 = g_rtSpecularAttr1.Load( int3(Input.Tex.x * 640, Input.Tex.y * 480, 0), 0 ).xyz;
	float3 Transparency = g_rtTransparency.Load( int3(Input.Tex.x * 640, Input.Tex.y * 480, 0), 0 ).xyz;
	
	float4 Common1 = g_rtCommon1.Load( int3(Input.Tex.x * 640, Input.Tex.y * 480, 0), 0 );
	float Bump = Common1.x;
	float Diffuse = Common1.y;	
	float RefractiveIndex = Common1.z;
	float SurfaceThickness = Common1.w;		
	
	float4 Common2 = g_rtCommon2.Load( int3(Input.Tex.x * 640, Input.Tex.y * 480, 0), 0 );

	bool bLightOff = false;
	float Attenuation = 0;	
	float Alpha = 1;
	float3 L;
	float Distance;
	float NL;

	float4 OutColor = float4( 0, 0, 0, Alpha );

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

	if( !bLightOff )
	{
		NL = dot( N, L );
		if( NL < 0.0 ) NL = 0.0;

		float3 Lambert = Color;// Color * vLightColor * float3( Diffuse, Diffuse, Diffuse ) * NL;	

		// Lambert
		if( fShading == 1.0 || NL < 0.0001 )
		{
			OutColor.rgb = Lambert;
		}
		// Blinn
		else if( fShading == (1.0 / 2.0) )
		{
			float Eccentricity = SpecularAttr1.x * 100.0;
			float Rolloff = SpecularAttr1.y * 100.0;
			OutColor.rgb = Lambert + BlinnSpecular( Eccentricity, Rolloff, N, L, V, vLightColor, Specular );		
		}
		// Phong
		else if( fShading == (1.0 / 3.0) )
		{
			float cPower = SpecularAttr1.x * 100.0;
			float Reflectivity = SpecularAttr1.y * 100.0;
			
			OutColor.rgb = Lambert + PhongSpecular( cPower, Reflectivity, N, L, V, vLightColor, Specular );
		}	
		else
		{			
			float Eccentricity = SpecularAttr1.x * 100.0;
			float Rolloff = SpecularAttr1.y * 100.0;
			OutColor.rgb = Lambert + BlinnSpecular( Eccentricity, Rolloff, N, L, V, vLightColor, Specular );		
		}
	}	
	
	return OutColor;
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------
technique10 FillMRT
{
    pass P0
    {
		SetDepthStencilState( DepthLessEqual, 0 );
		//SetRasterizerState( EnableMultisampling );
        //SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
		
        SetVertexShader( CompileShader( vs_4_0, FillMRT_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, FillMRT_PS() ) );
    }    
}

technique10 DrawQuad
{
    pass P0
    {		
        SetVertexShader( CompileShader( vs_4_0, ResolveMRT_VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ResolveMRT_PS() ) );  
    }        	
}


