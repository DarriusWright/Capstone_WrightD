/*

% Description of my shader.
% Second line of description for my shader.

keywords: material classic

date: YYMMDD

float4x4 WorldViewProj : WorldViewProjection;

float4 mainVS(float3 pos : POSITION) : POSITION{
	return mul(float4(pos.xyz, 1.0), WorldViewProj);
}

float4 mainPS() : COLOR {
	return float4(1.0, 1.0, 1.0, 1.0);
}

technique technique0 {
	pass p0 {
		CullMode = None;
		VertexShader = compile vs_3_0 mainVS();
		PixelShader = compile ps_3_0 mainPS();
	}
}

*/

cbuffer CBufferPerObject
{
	float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
}

RasterizerState DisableCulling
{
	CullMode = NONE;
};

float4 vertex_shader(float3 objectPosition: POSITION): SV_POSITION
{
	return mul(float4 (objectPosition,1), WorldViewProjection);
}

float4 pixel_shader():SV_Target
{
	return float4(1,0,0,1);
}

technique10 main10
{
	pass p0
	{
		SetVertexShader(CompileShader(vs_4_0,vertex_shader()));
		SetPixelShader(CompileShader(ps_4_0,pixel_shader()));
		SetRasterizerState(DisableCulling);
	}
}
