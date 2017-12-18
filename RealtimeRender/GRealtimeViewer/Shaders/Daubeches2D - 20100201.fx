float4x4 mvpMatrix;
float texelWidth;
float texelHeight;
uint width;
uint height;
// From wikipedia
// Daubechies 9/7-CDF-wavelet
float analysisLowKernel[] = { 0.026748757411f, -0.016864118443f, -0.078223266529f, 0.266864118443f, 0.602949018236f, 0.266864118443f,-0.078223266529f, -0.016864118443f, 0.026748757411f };
float analysisHighKernel[] = { 0.0f, 0.091271763114f, -0.057543526229f, -0.591271763114f, 1.11508705f, -0.591271763114f, -0.057543526229f, 0.091271763114f, 0.0f };
// From wikipedia
// NOTE: These are "interleaved".
float evenSynthesisFilter[] = { 0, 0.016864118443, -0.057543526229,-0.266864118443,1.11508705,-0.266864118443, -0.057543526229, 0.016864118443,0}; 
float oddSynthesisFilter[] = { 0.026748757411, -0.091271763114, -0.078223266529, 0.591271763114,0.602949018236, 0.591271763114, -0.078223266529, -0.091271763114, 0.026748757411 };


half3 RGBToLAB(half3 RGB)
{
	half3 LAB;
	half3 XYZ;
	half3 xyz = {0.98072f, 1.00000f, 1.18225f};
		
	// RGB to XYZ
	XYZ.x = RGB.x * 0.6070f + RGB.y * 0.1740f + RGB.z * 0.2000f;
	XYZ.y = RGB.x * 0.2990f + RGB.y * 0.5870f + RGB.z * 0.1440f;
	XYZ.z = RGB.x * 0.0000f + RGB.y * 0.0660f + RGB.z * 1.1120f;	
	
	// XYZ to LAB
	xyz.x = XYZ.x / xyz.x > 0.008856 ?
		pow((XYZ.x / xyz.x), 1.0f / 3.0f) : 
		7.787 * (XYZ.x / xyz.x) + (16.0f / 116.0f);
	xyz.y = XYZ.y / xyz.y > 0.008856 ? 
		pow((XYZ.y / xyz.y), 1.0f / 3.0f) :
		7.787 * (XYZ.y / xyz.y) + (16.0f / 116.0f);
	xyz.z = XYZ.z / xyz.z > 0.008856 ? 
		pow((XYZ.z / xyz.z), 1.0f / 3.0f) :
		7.787 * (XYZ.z / xyz.z) + (16.0f / 116.0f);
	LAB.x = XYZ.y / 1.00000f > 0.008856 ? 
		116.0f * xyz.y - 16.0f : 
		903.3f * xyz.y - 16.0f;
	LAB.y = 504.3f * (xyz.x - xyz.y);
	LAB.z = 201.7f * (xyz.y - xyz.z);
	
	return LAB;
}

half3 LABToRGB(half3 LAB)
{
	half x, y, z;
	half L, A, B;
	L = LAB.x;
	A = LAB.y;
	B = LAB.z;

	// LAB to XYZ
	half var_Y = ( L + 16 ) / 116;
	half var_X = A / 500 + var_Y;
	half var_Z = var_Y - B / 200;

	if ( pow(var_Y,3) > 0.008856 ) var_Y = pow(var_Y,3);
	else var_Y = ( var_Y - 16.0 / 116.0 ) / 7.787;

	if ( pow(var_X,3) > 0.008856 ) var_X = pow(var_X,3);
	else var_X = ( var_X - 16.0 / 116.0 ) / 7.787;

	if ( pow(var_Z,3) > 0.008856 ) var_Z = pow(var_Z,3);
	else var_Z = ( var_Z - 16.0 / 116.0 ) / 7.787;
	const half ref_X = 0.9505;
	const half ref_Y = 1.000;
	const half ref_Z = 1.089;
	
	x = ref_X * var_X;     //ref_X = 0.9505  Observer= 2 deg Illuminant= D65
	y = ref_Y * var_Y;     //ref_Y = 1.000
	z = ref_Z * var_Z;     //ref_Z = 1.089

	half r, g, b;

	r = x *  3.2406 + y * -1.5372 + z * -0.4986;
	g = x * -0.9689 + y *  1.8758 + z *  0.0415;
	b = x *  0.0557 + y * -0.2040 + z *  1.0570;

	if (r > 0.0031308) r = 1.055 * (pow(r, ( 1 / 2.4 ))) - 0.055;
	else r = 12.92 * r;
	if (g > 0.0031308) g = 1.055 * (pow(g ,( 1 / 2.4 ))) - 0.055;
	else g = 12.92 * g;
	if (b > 0.0031308) b = 1.055 * (pow(b, ( 1 / 2.4 ))) - 0.055;
	else b = 12.92 * b;

	half maxVal = r;
	if (maxVal < g) maxVal = g;
	if (maxVal < b) maxVal = b;
	if (maxVal > 1.0)
	{
		r /= maxVal;
		g /= maxVal;
		b /= maxVal;
	}
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	return half3( r, g, b );
}



Texture2D SourceTexture;

SamplerState SamPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};

SamplerState SourceSampler{
	//Texture = (SourceTexture);
	Filter = MIN_MAG_MIP_POINT;
	//MipFilter = NONE;
	//MinFilter = Point;
	//MagFilter = Point;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};

Texture2D LowPassSourceTexture;

SamplerState LowPassSource{
	//Texture = (LowPassSourceTexture);
	Filter = MIN_MAG_MIP_POINT;
	//MipFilter = NONE;
	//MinFilter = Point;
	//MagFilter = Point;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};

Texture2D HighPassSourceTexture;

SamplerState HighPassSource{
	//Texture = (HighPassSourceTexture);
	Filter = MIN_MAG_MIP_POINT;
	//MipFilter = NONE;
	//MinFilter = Point;
	//MagFilter = Point;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};
struct PS_OUT {
	half4 Color : SV_Target;
};
struct PS_DUAL_OUT {
	half4 Output1 : SV_TARGET0;
	half4 Output2 : SV_TARGET1;
};
struct TEX_VS_IN {
	float3 Position : POSITION;
	float3 vNorm	: NORMAL; 
	float3 vTangent : TANGENT;
	float2 Tex		: TEXCOORD;
};

struct TEX_VS_OUT {
	float4 Position	: SV_Position;
	float2 Tex		: TEXCOORD;
};

// The synthesis uses 5 taps
struct SYNTHESIS_TEX {
	float4 Position : SV_Position;
	float2 Tex		: TEXCOORD0;
	half2 Tap1 : TEXCOORD1; // Min two texels
	half2 Tap2 : TEXCOORD2; // Min one texels
	half2 Tap3 : TEXCOORD3; // Base position
	half2 Tap4 : TEXCOORD4; // Pluss one texels
	half2 Tap5 : TEXCOORD5; // Pluss two texels
};
SYNTHESIS_TEX HorisontalSynthesisVS(TEX_VS_IN input) {
	SYNTHESIS_TEX output = (SYNTHESIS_TEX)0;
	output.Position = float4(input.Position, 1);
	output.Tex = input.Tex + float2(texelWidth / 2.f, texelHeight / 2.f);
	output.Tap1 = half2(input.Tex.x - 2 * texelWidth, input.Tex.y);
	output.Tap2 = half2(input.Tex.x - 1 * texelWidth, input.Tex.y );
	output.Tap3 = input.Tex;
	output.Tap4 = half2(input.Tex.x + 1 * texelWidth, input.Tex.y );
	output.Tap5 = half2(input.Tex.x + 2 * texelWidth, input.Tex.y );
	return output;
}
SYNTHESIS_TEX VerticalSynthesisVS(TEX_VS_IN input) {
	SYNTHESIS_TEX output = (SYNTHESIS_TEX)0;
	output.Position = float4(input.Position, 1);
	output.Tex = input.Tex + float2(texelWidth / 2.f, texelHeight / 2.f);
	output.Tap1 = half2(input.Tex.x, input.Tex.y - 2 * texelHeight );
	output.Tap2 = half2(input.Tex.x, input.Tex.y - 1 * texelHeight);
	output.Tap3 = input.Tex;
	output.Tap4 = half2(input.Tex.x, input.Tex.y + 1 * texelHeight );
	output.Tap5 = half2(input.Tex.x, input.Tex.y + 2 * texelHeight);
	return output;
}
TEX_VS_OUT NormalVS(TEX_VS_IN input) {
	TEX_VS_OUT output = (TEX_VS_OUT)0;
	output.Position = float4(input.Position, 1);
	output.Tex = input.Tex;
	return output;
}
PS_DUAL_OUT HorizontalAnalysis(TEX_VS_OUT input) {
	PS_DUAL_OUT output = (PS_DUAL_OUT)0;
	uint3 msCoord;	

	int i;
	half3 vals[10];
	
	for( i=0; i < 10; i++ )  
	{
		msCoord = int3( (int(input.Position.x) * 2 + (i-4)), int(input.Position.y), 0 );
		
		if( msCoord.x < 0 ) 
		{
			msCoord.x += 2*width;
		}
		else if( msCoord.x > width * 2 - 1 )
		{
			msCoord.x -= 2*width;
		}
		
		vals[i] = SourceTexture.Load( msCoord, 0 ).xyz; 
	}
	
	
	for(i=0;i<9;i++) {
		output.Output1.rgb += vals[i] * analysisLowKernel[i];
		output.Output2.rgb += (vals[i+1] * analysisHighKernel[i]);
	}
	output.Output1.a = 1;
	output.Output2.a = 1;
	
	return output;
}
PS_DUAL_OUT VerticalAnalysis(TEX_VS_OUT input) {
	PS_DUAL_OUT output = (PS_DUAL_OUT)0;
	uint3 msCoord;	

	int i;
	half3 vals[10];
	
	for( i=0; i < 10; i++ )  
	{
		msCoord = int3( int(input.Position.x), (int(input.Position.y) * 2 + (i-4)), 0 );
		
		if( msCoord.y < 0 ) 
		{
			msCoord.y += 2*height;
		}
		else if( msCoord.y > height * 2 - 1 )
		{
			msCoord.y -= 2*height;		
		}
		vals[i] = SourceTexture.Load( msCoord, 0 ).xyz; 
	}
	
	for(i=0;i<9;i++) {
		output.Output1.rgb += vals[i] * analysisLowKernel[i];
		output.Output2.rgb += (vals[i+1] * analysisHighKernel[i]);
	}
	output.Output1.a = 1.0f;
	output.Output2.a = 1.0f;
	return output;
}
// This uses approx 9 texture and 10 arithmetic
PS_OUT SynthesisEvenV(SYNTHESIS_TEX input) {
	PS_OUT output = (PS_OUT)0;

	half2 pos = input.Position.xy;
	int k = int(pos.y);
	uint3 Tap1 = uint3(pos.x, k - 2, 0);
	uint3 Tap2 = uint3(pos.x, k - 1, 0);
	uint3 Tap3 = uint3(pos.x, k, 0);
	uint3 Tap4 = uint3(pos.x, k + 1, 0);
	uint3 Tap5 = uint3(pos.x, k + 2, 0);

	if( Tap1.y < 0 )
		Tap1.y += height;
	if( Tap2.y < 0 )
		Tap2.y += height;
	if( Tap4.y > height - 1 )
		Tap4.y -= height;
	if( Tap5.y > height - 1 )
		Tap5.y -= height;
	
	output.Color.rgb = evenSynthesisFilter[0] * LowPassSourceTexture.Load( Tap1, 0 )
		+ evenSynthesisFilter[1] * HighPassSourceTexture.Load( Tap1, 0 )
		+ evenSynthesisFilter[2] * LowPassSourceTexture.Load( Tap2, 0 )
		+ evenSynthesisFilter[3] * HighPassSourceTexture.Load( Tap2, 0 )
		+ evenSynthesisFilter[4] * LowPassSourceTexture.Load( Tap3, 0 )
		+ evenSynthesisFilter[5] * HighPassSourceTexture.Load( Tap3, 0 )
		+ evenSynthesisFilter[6] * LowPassSourceTexture.Load( Tap4, 0 )
		+ evenSynthesisFilter[7] * HighPassSourceTexture.Load( Tap4, 0 )
		+ evenSynthesisFilter[8] * LowPassSourceTexture.Load( Tap5, 0 );
	
	output.Color.a = 1;
	return output;
}
PS_OUT SynthesisEvenH(SYNTHESIS_TEX input) {
	PS_OUT output = (PS_OUT)0;

	half2 pos = input.Position.xy;	

	int k = int(pos.x);
	uint3 Tap1 = uint3(k - 2, pos.y, 0);
	uint3 Tap2 = uint3(k - 1, pos.y, 0);
	uint3 Tap3 = uint3(k, pos.y, 0);
	uint3 Tap4 = uint3(k + 1, pos.y, 0);
	uint3 Tap5 = uint3(k + 2, pos.y, 0);

	if( Tap1.x < 0 )
		Tap1.x += width / 2;
	if( Tap2.x < 0 )
		Tap2.x += width / 2;
	if( Tap4.x > width / 2 - 1 )
		Tap4.x -= width / 2;
	if( Tap5.x > width / 2 - 1 )
		Tap5.x -= width / 2;

	output.Color.rgb = evenSynthesisFilter[0] * LowPassSourceTexture.Load( Tap1, 0 )
		+ evenSynthesisFilter[1] * HighPassSourceTexture.Load( Tap1, 0 )
		+ evenSynthesisFilter[2] * LowPassSourceTexture.Load( Tap2, 0 )
		+ evenSynthesisFilter[3] * HighPassSourceTexture.Load( Tap2, 0 )
		+ evenSynthesisFilter[4] * LowPassSourceTexture.Load( Tap3, 0 )
		+ evenSynthesisFilter[5] * HighPassSourceTexture.Load( Tap3, 0 )
		+ evenSynthesisFilter[6] * LowPassSourceTexture.Load( Tap4, 0 )
		+ evenSynthesisFilter[7] * HighPassSourceTexture.Load( Tap4, 0 )
		+ evenSynthesisFilter[8] * LowPassSourceTexture.Load( Tap5, 0 );
	
	output.Color.a = 1;
	return output;
}
PS_OUT SynthesisOddV(SYNTHESIS_TEX input) {	
	PS_OUT output = (PS_OUT)0;

	half2 pos = input.Position.xy;

	int k = int(pos.y);
	uint3 Tap1 = uint3(pos.x, k - 2, 0);
	uint3 Tap2 = uint3(pos.x, k - 1, 0);
	uint3 Tap3 = uint3(pos.x, k, 0);
	uint3 Tap4 = uint3(pos.x, k + 1, 0);
	uint3 Tap5 = uint3(pos.x, k + 2, 0);

	if( Tap1.y < 0 )
		Tap1.y += height;
	if( Tap2.y < 0 )
		Tap2.y += height;
	if( Tap4.y > height - 1 )
		Tap4.y -= height;
	if( Tap5.y > height - 1 )
		Tap5.y -= height;

	output.Color.rgb = oddSynthesisFilter[0] * HighPassSourceTexture.Load( Tap1, 0 );
	output.Color.rgb += oddSynthesisFilter[1] * LowPassSourceTexture.Load( Tap2, 0 );
	output.Color.rgb += oddSynthesisFilter[2] * HighPassSourceTexture.Load( Tap2, 0 );
	output.Color.rgb += oddSynthesisFilter[3] * LowPassSourceTexture.Load( Tap3, 0 );
	output.Color.rgb += oddSynthesisFilter[4] * HighPassSourceTexture.Load( Tap3, 0 );
	output.Color.rgb += oddSynthesisFilter[5] * LowPassSourceTexture.Load( Tap4, 0 );
	output.Color.rgb += oddSynthesisFilter[6] * HighPassSourceTexture.Load( Tap4, 0 );
	output.Color.rgb += oddSynthesisFilter[7] * LowPassSourceTexture.Load( Tap5, 0 );
	output.Color.rgb += oddSynthesisFilter[8] * HighPassSourceTexture.Load( Tap5, 0 );
	
	output.Color.a = 1;
	return output;
}
PS_OUT SynthesisOddH(SYNTHESIS_TEX input) {	
	PS_OUT output = (PS_OUT)0;

	half2 pos = input.Position.xy;
	
	int k = int(pos.x);
	uint3 Tap1 = uint3(k - 2, pos.y, 0);
	uint3 Tap2 = uint3(k - 1, pos.y, 0);
	uint3 Tap3 = uint3(k, pos.y, 0);
	uint3 Tap4 = uint3(k + 1, pos.y, 0);
	uint3 Tap5 = uint3(k + 2, pos.y, 0);

	if( Tap1.x < 0 )
		Tap1.x += width / 2;
	if( Tap2.x < 0 )
		Tap2.x += width / 2;
	if( Tap4.x > width / 2 - 1 )
		Tap4.x -= width / 2;
	if( Tap5.x > width / 2 - 1 )
		Tap5.x -= width / 2;

	output.Color.rgb = oddSynthesisFilter[0] * HighPassSourceTexture.Load( Tap1, 0 );
	output.Color.rgb += oddSynthesisFilter[1] * LowPassSourceTexture.Load( Tap2, 0 );
	output.Color.rgb += oddSynthesisFilter[2] * HighPassSourceTexture.Load( Tap2, 0 );
	output.Color.rgb += oddSynthesisFilter[3] * LowPassSourceTexture.Load( Tap3, 0 );
	output.Color.rgb += oddSynthesisFilter[4] * HighPassSourceTexture.Load( Tap3, 0 );
	output.Color.rgb += oddSynthesisFilter[5] * LowPassSourceTexture.Load( Tap4, 0 );
	output.Color.rgb += oddSynthesisFilter[6] * HighPassSourceTexture.Load( Tap4, 0 );
	output.Color.rgb += oddSynthesisFilter[7] * LowPassSourceTexture.Load( Tap5, 0 );
	output.Color.rgb += oddSynthesisFilter[8] * HighPassSourceTexture.Load( Tap5, 0 );
	
	output.Color.a = 1;
	return output;
}
PS_OUT SynthesisAddV(TEX_VS_OUT input) {
	PS_OUT output = (PS_OUT)0;
	
	uint2 coord = input.Position.xy;
	coord.y /= 2;

	if( uint(input.Position.y) % 2 != 0 ) 
		output.Color.rgb = LowPassSourceTexture.Load( int3(coord.x, coord.y, 0), 0 );	
	else
		output.Color.rgb = HighPassSourceTexture.Load( int3(coord.x, coord.y, 0), 0 );
	output.Color.a = 1;
	return output;
}
PS_OUT SynthesisAddH(TEX_VS_OUT input) {
	PS_OUT output = (PS_OUT)0;
	
	uint2 coord = input.Position.xy;
	coord.x /= 2;

	if( uint(input.Position.x) % 2 != 0 ) 
		output.Color.rgb = LowPassSourceTexture.Load( int3(coord.x, coord.y, 0), 0 );	
	else
		output.Color.rgb = HighPassSourceTexture.Load( int3(coord.x, coord.y, 0), 0 );
	output.Color.a = 1;
	return output;
}
// These methods are used to visualize the result of a wavelet analysis.
PS_OUT WriteHigh(TEX_VS_IN input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = tex2D(HighPassSource, input.Tex);
	output.Color.a = 1.0f;
	return output;
}
PS_OUT WriteLow(TEX_VS_IN input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = tex2D(LowPassSource, input.Tex);
	output.Color.a = 1.0f;
	return output;
}
// Does back and forth but nothing more Can be used for testing speed
technique10 FastWavelet {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, HorizontalAnalysis() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
	pass P1 {
		SetPixelShader( CompileShader( ps_4_0, VerticalAnalysis() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
	pass P2 {
		SetPixelShader( CompileShader( ps_4_0, VerticalAnalysis() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}pass P3 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisEvenV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P4 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisOddV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P5 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisEvenV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P6 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisOddV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P7 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisEvenH() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, HorisontalSynthesisVS() ) );
	}
	pass P8 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisOddH() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, HorisontalSynthesisVS() ) );
	}
}
technique10 FastWaveletAnalysis {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, HorizontalAnalysis() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
	pass P1 {
		SetPixelShader( CompileShader( ps_4_0, VerticalAnalysis() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
	pass P2 {
		SetPixelShader( CompileShader( ps_4_0, VerticalAnalysis() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
}
technique10 FastWaveletSynthesis {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisEvenV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P1 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisOddV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) ); 
	}
	pass P2 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisAddV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) ); 
	}
	pass P3 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisEvenV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P4 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisOddV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, VerticalSynthesisVS() ) );
	}
	pass P5 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisAddV() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) ); 
	}
	pass P6 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisEvenH() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, HorisontalSynthesisVS() ) );
	}
	pass P7 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisOddH() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, HorisontalSynthesisVS() ) );
	}
	pass P8 {
		SetPixelShader( CompileShader( ps_4_0, SynthesisAddH() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) ); 
	}
}
PS_OUT VisualizeHighPassPS(TEX_VS_OUT input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = SourceTexture.Sample(SourceSampler, input.Tex) + 0.5;
	output.Color.a = 1.0f;
	return output;
}
PS_OUT VisualizeHighLowPS(TEX_VS_OUT input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = SourceTexture.Sample(SourceSampler, input.Tex);
	output.Color.a = 1.0f;
	return output;
}
technique10 VisualizeHighPassTechnique {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, VisualizeHighPassPS() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
}
technique10 VisualizeLowPassTechnique {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, VisualizeHighLowPS() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
}


PS_OUT CollectHighAndLow(TEX_VS_OUT input) 
{
	PS_OUT output = (PS_OUT)0;
	output.Color.r = LowPassSourceTexture.Sample(SamPoint, input.Tex).r;
	output.Color.gba = HighPassSourceTexture.Sample(SamPoint, input.Tex).gba;
	return output;
}
technique10 Passthrough {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, CollectHighAndLow() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
}

float threshold;
float thresholdLevelWeight;

PS_OUT SoftThresholdingPS(TEX_VS_OUT input) 
{
	PS_OUT output = (PS_OUT)0;
	// Low pass is not affected
	output.Color.r = LowPassSourceTexture.Sample(SamPoint, input.Tex).r;
	// Threshold high pass
	half3 val = HighPassSourceTexture.Sample(SamPoint, input.Tex).gba;
	output.Color.gba = sign(val) * max(abs(val)-threshold, 0);
	return output;
}
half3 hardThreshold(half3 input, half threshold) 
{
	// s will be 0 if we are within the threshold value, else 1
	half3 s = sign(max(abs(input)-threshold, 0));
	return input * s;
}

PS_OUT HardThresholdingPS(TEX_VS_OUT input) 
{
	PS_OUT output = (PS_OUT)0;
	// Low pass is not affected
	output.Color.r = LowPassSourceTexture.Sample(SamPoint, input.Tex).r;
	// Hard threshold on the high pass
	half3 val = HighPassSourceTexture.Sample(SamPoint, input.Tex).gba;
	output.Color.gba = hardThreshold(val, threshold);
	return output;
}

technique10 SoftThreshold {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, SoftThresholdingPS() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
}
technique10 HardThreshold {
	pass P0 {
		SetPixelShader( CompileShader( ps_4_0, HardThresholdingPS() ) );
		SetGeometryShader( NULL );
		SetVertexShader( CompileShader( vs_4_0, NormalVS() ) );
	}
}

