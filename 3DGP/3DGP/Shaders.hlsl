



float4 VSMain(uint vertexID : SV_VertexID) : SV_POSITION
{
	float4 output = float4(1.0f, 1.0f, 1.0f, 1.0f);
	switch (vertexID)
	{
	case 0:
		output = float4(0.0f, 0.5f, 0.5f, 1.0f);
		break;
	case 1:
		output = float4(0.5f, -0.5f, 0.5f, 1.0f);
		break;
	case 2:
		output = float4(-0.5f, -0.5f, 0.5f, 1.0f);
		break;
	default:break;
	}
	return output;
}

float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}