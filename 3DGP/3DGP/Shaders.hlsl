


float4 tri(uint vertexID)
{
	float3 output = float3(0.0f, 0.0f, 0.0f);
	switch (vertexID)
	{
		case 0:
			output = float3(+0.0f, +0.5f, +0.5f);
			break;
		case 1:
			output = float3(+0.5f, -0.5f, +0.5f);
			break;
		case 2:
			output = float3(-0.5f, -0.5f, +0.5f);
			break;
		default:
			break;
	}
	return float4(output, 1.0f);
}

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

float4 VSMain(uint vertexID : SV_VertexID) : SV_POSITION
{
	return full(vertexID);
}




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
		color.r += sin(input.y - H / 1000.0f) * (length(input.xy)/1000.0f);
		color.g += sin(input.x - W / 1000.0f) * (length(input.xy)/1000.0f);
	}

	return color;
}

float4 focus(float4 input)
{
	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
	color.rgb = distance(float2(0.5f, 0.5f), input.xy / float2(W, H));
	return color;
}


float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
	return hundred(input);
}