
//////////////////////////////////////
struct Meterial
{
	float4 diffuse;
	float4 ambient;
	float4 specular;
	float4 emmesive;
	float specularPower;
	float3 _padd;
};

cbuffer cbObjInfo : register(b0)
{
	matrix worldMat : packoffset(c0);
	Meterial meterial : packoffset(c4);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix viewProj : packoffset(c0);
	float2 viewport : packoffset(c4.x);
}

struct INSTANCED_GAMEOBJECT_INFO
{
	matrix mTransform;
	Meterial meterial;
};
StructuredBuffer<INSTANCED_GAMEOBJECT_INFO> gameObjectInfos : register(t0);

struct EFFECT_INFO
{
	float mTime;
	float mLifeTime;
	float3 mPosition;
};
StructuredBuffer<EFFECT_INFO> effectInfos : register(t1);

struct UI_INFO
{
	matrix mTransform;
	float4 mColor;
};
StructuredBuffer<UI_INFO> UIInfos : register(t2);
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

//////////////////////////////////////

float4 VP(float3 position)
{
	return mul(float4(position, 1.0f), viewProj);
}

///////////////////////////////////////
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), worldMat).xyz;
	
	output.position = VP(output.originPosition);

	output.color = input.color;
	
	return output;
}

VS_OUTPUT VSInstancing(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f)
	, gameObjectInfos[instanceID].mTransform).xyz;
	
	output.position = VP(output.originPosition);

	output.color = gameObjectInfos[instanceID].meterial.ambient;
	return output;
}

VS_OUTPUT VSTerrain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), worldMat).xyz;
	
	output.position = VP(output.originPosition);

	output.color = input.color;

	return output;
}

VS_OUTPUT VSUI(VS_INPUT input, uint instanceID : SV_InstanceID)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), UIInfos[instanceID].mTransform).xyz;
	
	output.position = float4(output.originPosition, 1.0f);
	output.position.z = 0.0f;
	output.color = input.color + UIInfos[instanceID].mColor;

	return output;
}

/////////////////////////////////////////

float4 Effect0Wall(float3 originPos)
{
	float t = effectInfos[0].mLifeTime - effectInfos[0].mTime;
	if (15.0f > distance(effectInfos[0].mPosition, originPos) && 0.0f < t)
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

float4 PSInstancing(VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;
	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	return output;
}

float4 PSTerrain(VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;

	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	
	return output;
}

float4 PSUI(VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;
	
	return output;
}