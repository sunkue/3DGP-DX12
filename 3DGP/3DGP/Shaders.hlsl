


float4 tri(uint vertexID)
{
	float3 output = float3(0.0f,0.0f,0.0f);
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
	default:break;
	}
	return float4(output, 1.0f);
}

float4 fullscreen(uint vertexID)
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
	default:break;
	}
	return float4(output, 1.0f);
}

float4 VSMain(uint vertexID : SV_VertexID) : SV_POSITION
{
	return fullscreen(vertexID);
}

#define FRAME_BUFFER_WIDTH	640.0f;
#define FRAME_BUFFER_HEIGHT	480.0f;

float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
	float4 color = float4(1.0f, 1.0f, 0.0f, 1.0f);
	color.r = input.x / FRAME_BUFFER_WIDTH;

	return color;
}