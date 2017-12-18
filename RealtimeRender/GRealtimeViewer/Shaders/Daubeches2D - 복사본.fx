float4x4 mvpMatrix;
float texelWidth;
float texelHeight;
// From wikipedia
// Daubechies 9/7-CDF-wavelet
float analysisLowKernel[] = { 0.026748757411f, -0.016864118443f, -0.078223266529f, 0.266864118443f, 0.602949018236f, 0.266864118443f,-0.078223266529f, -0.016864118443f, 0.026748757411f };
float analysisHighKernel[] = { 0.0f, 0.091271763114f, -0.057543526229f, -0.591271763114f, 1.11508705f, -0.591271763114f, -0.057543526229f, 0.091271763114f, 0.0f };
// From wikipedia
// NOTE: These are "interleaved".
float evenSynthesisFilter[] = { 0, 0.016864118443, -0.057543526229,-0.266864118443,1.11508705,-0.266864118443, -0.057543526229, 0.016864118443,0}; 
float oddSynthesisFilter[] = { 0.026748757411, -0.091271763114, -0.078223266529, 0.591271763114,0.602949018236, 0.591271763114, -0.078223266529, -0.091271763114, 0.026748757411 };

texture SourceTexture;

sampler2D SourceSampler = sampler_state {
	Texture = (SourceTexture);
	MipFilter = NONE;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};

texture LowPassSourceTexture;

sampler2D LowPassSource = sampler_state {
	Texture = (LowPassSourceTexture);
	MipFilter = NONE;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};

texture HighPassSourceTexture;

sampler2D HighPassSource = sampler_state {
	Texture = (HighPassSourceTexture);
	MipFilter = NONE;
	MinFilter = Point;
	MagFilter = Point;
	AddressU = Mirror;
	AddressV = Mirror;
	MaxANISOTROPY = 1;
};
struct PS_OUT {
	float4 Color : COLOR0;
};
struct PS_DUAL_OUT {
	float4 Output1 : COLOR0;
	float4 Output2 : COLOR1;
};
struct TEX_VS_IN {
	float4 Position : POSITION;
	float2 Tex : TEXCOORD0;
};
// The synthesis uses 5 taps
struct SYNTHESIS_TEX {
	float4 Position : POSITION;
	float2 Tap1 : TEXCOORD0; // Min two texels
	float2 Tap2 : TEXCOORD1; // Min one texels
	float2 Tap3 : TEXCOORD2; // Base position
	float2 Tap4 : TEXCOORD3; // Pluss one texels
	float2 Tap5 : TEXCOORD4; // Pluss two texels
};
SYNTHESIS_TEX HorisontalSynthesisVS(TEX_VS_IN input) {
	SYNTHESIS_TEX output = (SYNTHESIS_TEX)0;
	output.Position = mul(input.Position, mvpMatrix);
	output.Tap1 = float2(input.Tex.x - 2 * texelWidth, input.Tex.y);
	output.Tap2 = float2(input.Tex.x - texelWidth, input.Tex.y );
	output.Tap3 = input.Tex;
	output.Tap4 = float2(input.Tex.x + texelWidth, input.Tex.y );
	output.Tap5 = float2(input.Tex.x + 2 * texelWidth, input.Tex.y );
	return output;
}
SYNTHESIS_TEX VerticalSynthesisVS(TEX_VS_IN input) {
	SYNTHESIS_TEX output = (SYNTHESIS_TEX)0;
	output.Position = mul(input.Position, mvpMatrix);
	output.Tap1 = float2(input.Tex.x, input.Tex.y - 2 * texelHeight);
	output.Tap2 = float2(input.Tex.x, input.Tex.y - texelHeight);
	output.Tap3 = input.Tex;
	output.Tap4 = float2(input.Tex.x, input.Tex.y + texelHeight);
	output.Tap5 = float2(input.Tex.x, input.Tex.y + 2 * texelHeight);
	return output;
}
TEX_VS_IN NormalVS(TEX_VS_IN input) {
	TEX_VS_IN output = (TEX_VS_IN)0;
	output.Position = mul(input.Position, mvpMatrix);
	output.Tex = input.Tex;
	return output;
}
PS_DUAL_OUT HorizontalAnalysis(TEX_VS_IN input) {
	PS_DUAL_OUT output = (PS_DUAL_OUT)0;
	float3 vals[10];for(int i=0;i<10;i++) {
		vals[i] = tex2D(SourceSampler, float2(input.Tex.x + (i-4) * texelWidth,
			input.Tex.y));
	}
	for(int i=0;i<9;i++) {
		output.Output1.rgb += vals[i] * analysisLowKernel[i];
		output.Output2.rgb += (vals[i+1] * analysisHighKernel[i]);
	}
	output.Output1.a = 1.0f;
	output.Output2.a = 1.0f;
	return output;
}
PS_DUAL_OUT VerticalAnalysis(TEX_VS_IN input) {
	PS_DUAL_OUT output = (PS_DUAL_OUT)0;
	float3 vals[10];
	for(int i=0;i<10;i++) {
		vals[i] = tex2D(SourceSampler, float2(input.Tex.x, input.Tex.y + (i-4) *
			texelHeight));
	}
	for(int i=0;i<9;i++) {
		output.Output1.rgb += vals[i] * analysisLowKernel[i];
		output.Output2.rgb += (vals[i+1] * analysisHighKernel[i]);
	}
	output.Output1.a = 1.0f;
	output.Output2.a = 1.0f;
	return output;
}
// This uses approx 9 texture and 10 arithmetic
PS_OUT SynthesisEven(SYNTHESIS_TEX input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = evenSynthesisFilter[0] * tex2D(LowPassSource, input.
		Tap1)
		+ evenSynthesisFilter[1] * tex2D(HighPassSource, input.Tap1)
		+ evenSynthesisFilter[2] * tex2D(LowPassSource, input.Tap2)
		+ evenSynthesisFilter[3] * tex2D(HighPassSource, input.Tap2)
		+ evenSynthesisFilter[4] * tex2D(LowPassSource, input.Tap3)
		+ evenSynthesisFilter[5] * tex2D(HighPassSource, input.Tap3)
		+ evenSynthesisFilter[6] * tex2D(LowPassSource, input.Tap4)
		+ evenSynthesisFilter[7] * tex2D(HighPassSource, input.Tap4)
		+ evenSynthesisFilter[8] * tex2D(LowPassSource, input.Tap5);
	output.Color.a = 1;
	return output;
}
PS_OUT SynthesisOdd(SYNTHESIS_TEX input) {
	PS_OUT output = (PS_OUT)0;output.Color.rgb = oddSynthesisFilter[0] * tex2D(HighPassSource, input.
		Tap1);
	output.Color.rgb += oddSynthesisFilter[1] * tex2D(LowPassSource, input.
		Tap2);
	output.Color.rgb += oddSynthesisFilter[2] * tex2D(HighPassSource, input.
		Tap2);
	output.Color.rgb += oddSynthesisFilter[3] * tex2D(LowPassSource, input.
		Tap3);
	output.Color.rgb += oddSynthesisFilter[4] * tex2D(HighPassSource, input.
		Tap3);
	output.Color.rgb += oddSynthesisFilter[5] * tex2D(LowPassSource, input.
		Tap4);
	output.Color.rgb += oddSynthesisFilter[6] * tex2D(HighPassSource, input.
		Tap4);
	output.Color.rgb += oddSynthesisFilter[7] * tex2D(LowPassSource, input.
		Tap5);
	output.Color.rgb += oddSynthesisFilter[8] * tex2D(HighPassSource, input.
		Tap5);
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
technique FastWavelet {
	pass P0 {
		PixelShader = compile ps_2_0 HorizontalAnalysis();
		VertexShader = compile vs_2_0 NormalVS();
	}
	pass P1 {
		PixelShader = compile ps_2_0 VerticalAnalysis();
		VertexShader = compile vs_2_0 NormalVS();
	}
	pass P2 {
		PixelShader = compile ps_2_0 VerticalAnalysis();
		VertexShader = compile vs_2_0 NormalVS();
	}pass P3 {
		PixelShader = compile ps_2_0 SynthesisEven();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P4 {
		PixelShader = compile ps_2_0 SynthesisOdd();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P5 {
		PixelShader = compile ps_2_0 SynthesisEven();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P6 {
		PixelShader = compile ps_2_0 SynthesisOdd();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P7 {
		PixelShader = compile ps_2_0 SynthesisEven();
		VertexShader = compile vs_2_0 HorisontalSynthesisVS();
	}
	pass P8 {
		PixelShader = compile ps_2_0 SynthesisOdd();
		VertexShader = compile vs_2_0 HorisontalSynthesisVS();
	}
}
technique FastWaveletAnalysis {
	pass P0 {
		PixelShader = compile ps_2_0 HorizontalAnalysis();
		VertexShader = compile vs_2_0 NormalVS();
	}
	pass P1 {
		PixelShader = compile ps_2_0 VerticalAnalysis();
		VertexShader = compile vs_2_0 NormalVS();
	}
	pass P2 {
		PixelShader = compile ps_2_0 VerticalAnalysis();
		VertexShader = compile vs_2_0 NormalVS();
	}
}
technique FastWaveletSynthesis {
	pass P0 {
		PixelShader = compile ps_2_0 SynthesisEven();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P1 {
		PixelShader = compile ps_2_0 SynthesisOdd();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P2 {
		PixelShader = compile ps_2_0 SynthesisEven();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P3 {
		PixelShader = compile ps_2_0 SynthesisOdd();
		VertexShader = compile vs_2_0 VerticalSynthesisVS();
	}
	pass P4 {
		PixelShader = compile ps_2_0 SynthesisEven();
		VertexShader = compile vs_2_0 HorisontalSynthesisVS();
	}
	pass P5 {
		PixelShader = compile ps_2_0 SynthesisOdd();
		VertexShader = compile vs_2_0 HorisontalSynthesisVS();
	}
}
PS_OUT VisualizeHighPassPS(TEX_VS_IN input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = tex2D(SourceSampler, input.Tex) + 0.5;
	output.Color.a = 1.0f;
	return output;
}
PS_OUT VisualizeHighLowPS(TEX_VS_IN input) {
	PS_OUT output = (PS_OUT)0;
	output.Color.rgb = tex2D(SourceSampler, input.Tex);
	output.Color.a = 1.0f;
	return output;
}
technique VisualizeHighPassTechnique {
	pass P0 {
		PixelShader = compile ps_2_0 VisualizeHighPassPS();
		VertexShader = compile vs_2_0 NormalVS();
	}
}
technique VisualizeLowPassTechnique {
	pass P0 {
		PixelShader = compile ps_2_0 VisualizeHighLowPS();
		VertexShader = compile vs_2_0 NormalVS();
	}
}