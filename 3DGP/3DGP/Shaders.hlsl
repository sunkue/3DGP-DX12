
//////////////////////////////////////
cbuffer cbPlayerInfo : register(b0)
{
	matrix worldMat : packoffset(c0);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix viewMat : packoffset(c0);
	matrix projMat : packoffset(c4);
	float2 viewport : packoffset(c8.x);
}

/////////////////////////////////////////
struct INSTANCED_GAMEOBJECT_INFO
{
	matrix mTransform;
	float4 mColor;
};
StructuredBuffer<INSTANCED_GAMEOBJECT_INFO> gameObjectInfos : register(t0);

struct EFFECT_INFO
{
	float mTime;
	float mLifeTime;
	float3 mPosition;
};
StructuredBuffer<EFFECT_INFO> effectInfos : register(t1);
//////////////////////////////////////////

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 originPosition : POSITION;
	float4 color : COLOR;
};

struct VS_INSTANCING_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_INSTANCING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 originPosition : POSITION;
	float4 color : COLOR;
};
//////////////////////////////////////

float4 WVP(float3 position)
{
	return mul(mul(mul(float4(position, 1.0f)
	, worldMat), viewMat), projMat);
}

float4 VP(float3 position)
{
	return mul(mul(float4(position, 1.0f)
	, viewMat), projMat);
}

///////////////////////////////////////
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), worldMat).xyz;
	
	output.position = WVP(input.position);

	output.color = input.color;

	return output;
}

VS_INSTANCING_OUTPUT VSInstancing(VS_INSTANCING_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), gameObjectInfos[instanceID].mTransform).xyz;
	
	output.position = mul(mul(mul(float4(input.position, 1.0f)
	, gameObjectInfos[instanceID].mTransform), viewMat), projMat);

	output.color = input.color + gameObjectInfos[instanceID].mColor;
	
	return output;
}

VS_OUTPUT VSTerrain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), worldMat).xyz;
	
	output.position = WVP(input.position);

	output.color = input.color;

	return output;
}
/////////////////////////////////////////

float4 Effect0Wall(float3 originPos)
{
	float t = effectInfos[0].mLifeTime - effectInfos[0].mTime;
	if (15.0f > distance(effectInfos[0].mPosition, originPos))
	{
		return float4(t, t, t, 0.0f);
	}
	else
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float4 Effect1Obj(float3 originPos)
{
	float t = (effectInfos[1].mLifeTime - effectInfos[1].mTime);
	if (15.0f > distance(effectInfos[1].mPosition, originPos) && t > 0.0f)
	{
		return float4(t, t, t, 0.0f);
	}
	else
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

/////////////////////////////////////////

//x : 1918
//y : 1061
// x : 0right1	y : 0down1

float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;

	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	
	return output;
}

float4 PSInstancing(VS_INSTANCING_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;
	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	//output = float4(input.position.x / viewport.x, input.position.y / viewport.y, 0.0f, 0.0f);
	
	return output;
}

float4 PSTerrain(VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;

	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	
	return output;
}