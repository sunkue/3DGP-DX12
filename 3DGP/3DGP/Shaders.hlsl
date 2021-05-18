
//////////////////////////////////////
cbuffer cbGameObjectInfo : register(b0)
{
	matrix worldMat : packoffset(c0);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix viewMat : packoffset(c0);
	matrix projMat : packoffset(c4);
}
/////////////////////////////////////////
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
//////////////////////////////////////
float4 full(uint vertexID)
{
	float3 output = float3(0.0f, 0.0f, 0.0f);
	switch (vertexID)
	{
		case 0:
			output = float3(-1.0f, +1.0f, 0.0f);
			break;
		case 1:
			output = float3(+1.0f, +1.0f, 0.0f);
			break;
		case 2:
			output = float3(+1.0f, -1.0f, 0.0f);
			break;
		case 3:
			output = float3(-1.0f, +1.0f, 0.0f);
			break;
		case 4:
			output = float3(+1.0f, -1.0f, 0.0f);
			break;
		case 5:
			output = float3(-1.0f, -1.0f, 0.0f);
			break;
		default:
			break;
	}
	return float4(output, 1.0f);
}

float4 WVP(float3 position)
{
	return mul(mul(mul(float4(position, 1.0f)
	, worldMat), viewMat), projMat);
}
///////////////////////////////////////
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	
	output.position = WVP(input.position);

	output.color = input.color;
	
	return output;
}
/////////////////////////////////////////
#define W	640.0f
#define H	480.0f

float4 hundred(float4 input)
{
	float4 color = float4(1.0f, 1.0f, 0.0f, 1.0f);
	if (input.y > 100.0f)
		color.b = 1.0f;
	color.r = input.x / W;
	if (input.x < 100.0f)
		color.g = 0.0f;
	return color;
}

float4 UFO(float4 input)
{
	float4 color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	float2 f2NDC = float2(input.x / W, input.y / H) - 0.5f;
	f2NDC.x *= (W / H);
	float fRadius = 0.3f;
	float fRadian = radians(360.0f / 30.0f);
	for (float f = 0; f < 30.0f; f += 1.0f)
	{
		float fAngle = fRadian * f;
		color.rgb += (0.0025f / length(f2NDC + float2(fRadius * cos(fAngle), fRadius * sin(fAngle))));
	}
	color.r += input.x / W;
	color.g += input.y / H;

	if (length(input.xy) > 1000.0f)
	{
		color.b -= length(float2(W * 2, H)) / 10000.0f;
		color.r += sin(input.y - H / 1000.0f) * (length(input.xy) / 1000.0f);
		color.g += sin(input.x - W / 1000.0f) * (length(input.xy) / 1000.0f);
	}

	if (input.x % 100 < 2 || input.y % 100 < 2)
		color.rgb = 1.0f;
	
	return color;
}

/////////////////////////////////////////
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;
	
	return output;
}