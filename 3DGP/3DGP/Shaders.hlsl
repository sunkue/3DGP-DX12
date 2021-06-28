#include "Header.hlsli"
#include "Light.hlsli"
//////////////////////////////////////

struct METERIAL
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float4 emmesive;
	float specularPower;
	float3 _padd;
};

//////////////////////////////////////////
cbuffer cbObjInfo : register(b0)
{
	matrix worldMat : packoffset(c0);
	METERIAL material : packoffset(c4);
}

cbuffer cbCameraInfo : register(b1)
{
	matrix viewProj : packoffset(c0);
	float3 cameraPos : packoffset(c4);
	int diffuse_factor_mode : packoffset(c4.w);
}

struct INSTANCED_GAMEOBJECT_INFO
{
	matrix mTransform;
	METERIAL meterial;
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

struct LIGHT
{
	int type;
	float3 position;
	float4 ambient;
	float4 diffuse;
	float4 specualr;
	float3 attenuation;
	float range;
	float3 direction;
	float falloff;
	float theta;
	float phi;
};
StructuredBuffer<LIGHT> lightInfos : register(t3);
//////////////////////////////////////////

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 originPosition : POSITION;
	float3 normal : NORMAL;
};

struct INSTANCING_VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct INSTANCING_VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 originPosition : POSITION;
	float3 normal : NORMAL;
	int index : INDEX;
};

struct UI_VS_INPUT
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct UI_VS_OUTPUT
{
	float4 position : SV_POSITION;
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
	output.normal = normalize(mul(float4(input.normal, 0.0f), worldMat).xyz);
	return output;
}

INSTANCING_VS_OUTPUT VSInstancing(INSTANCING_VS_INPUT input, uint instanceID : SV_InstanceID)
{
	INSTANCING_VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f)
	, gameObjectInfos[instanceID].mTransform).xyz;
	output.position = VP(output.originPosition);
	output.normal = normalize(mul(float4(input.normal, 0.0f), gameObjectInfos[instanceID].mTransform).xyz);
	output.index = instanceID;
	return output;
}

VS_OUTPUT VSTerrain(VS_INPUT input)
{
	VS_OUTPUT output;
	output.originPosition = mul(float4(input.position, 1.0f), worldMat).xyz;
	output.position = VP(output.originPosition);
	output.normal = normalize(mul(float4(input.normal, 0.0f), worldMat).xyz);
	return output;
}

UI_VS_OUTPUT VSUI(UI_VS_INPUT input, uint instanceID : SV_InstanceID)
{
	UI_VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), UIInfos[instanceID].mTransform);
	output.position.z = 0.0f;
	output.color = float4(0.2f, 0.2f, 0.2f, 0.0f) + UIInfos[instanceID].mColor;
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


//////////////////////////////
float4 ComputeDirectionalLight(
	  METERIAL material
	, LIGHT directionalLight
	, float3 normal
	, float3 toCamera
)
{
	float4 ambient = material.ambient * directionalLight.ambient;
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float3 toLight = -directionalLight.direction;
	float diffuseFactor = dot(toLight, normal);
	if (0.0f < diffuseFactor)
	{
		float3 reflection = reflect(-toLight, normal);
		float specFactor = pow(max(dot(reflection, toCamera), 0.0f), material.specularPower);
		
		diffuse = material.diffuse * (directionalLight.diffuse * diffuseFactor);
		specular = material.specular * (directionalLight.specualr * specFactor);
	}
	
	return ambient + diffuse + specular + material.emmesive;
}

float4 ComputePointLight(
	  METERIAL material
	, LIGHT pointLight
	, float3 position
	, float3 normal
	, float3 toCamera
)
{
	float3 toLight = pointLight.position - position;
	float distance = length(toLight);
	if (pointLight.range < distance)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		float4 ambient = material.ambient * pointLight.ambient;
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
		toLight /= distance;
		float diffuseFactor = dot(toLight, normal);
		if (0.0f < diffuseFactor)
		{
			float3 reflection = reflect(-toLight, normal);
			reflection = normalize(reflection);
			float4 specFactor = pow(max(dot(reflection, toCamera), 0.0f), material.specularPower);
			//float3 Half = normalize(toCamera + toLight);
			//float4 specFactor = pow(max(dot(Half, normal), 0.0f), material.specularPower);
			diffuseFactor = factorMode(diffuse_factor_mode, diffuseFactor);
			diffuse = material.diffuse * (pointLight.diffuse * diffuseFactor);
			specular = material.specular * (pointLight.specualr * specFactor);
		}
		float attenFactor = 1.0f / dot(pointLight.attenuation, float3(1.0f, distance, distance * distance));
		ambient *= attenFactor;
		diffuse *= attenFactor;
		specular *= attenFactor;
		return ambient + diffuse + specular + material.emmesive;
	}
}

float4 ComputeSpotLight(
	  METERIAL material
	, LIGHT spotLight
	, float3 position
	, float3 normal
	, float3 toCamera
)
{
	float3 toLight = spotLight.position - position;
	float distance = length(toLight);
	if (spotLight.range < distance)
	{
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		float4 ambient = material.ambient * spotLight.ambient;
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
		toLight /= distance;
		float diffuseFactor = dot(toLight, normal);
		if (0.0f < diffuseFactor)
		{
			float3 reflection = reflect(-toLight, normal);
			float specFactor = pow(max(dot(reflection, toCamera), 0.0f), material.specularPower);
		
			diffuse = material.diffuse * (spotLight.diffuse * diffuseFactor);
			specular = material.specular * (spotLight.specualr * specFactor);
		}
		float spotFactor = pow(max(dot(-toLight, spotLight.direction), 0.0f), spotLight.falloff);
		ambient *= spotFactor;
		float attenFactor = 1.0f / dot(spotLight.attenuation, float3(1.0f, distance, distance * distance));
		diffuse *= attenFactor;
		specular *= attenFactor;
	
		return ambient + diffuse + specular + material.emmesive;
	}
}

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3
float4 ComputeLight(METERIAL m, LIGHT L, float3 pos, float3 nor, float3 toCamera)
{
	float4 ret = float4(0.0f, 0.0f, 0.0f, 0.0f);
	toCamera = normalize(toCamera);
	nor = normalize(nor);
	switch (L.type)
	{
		case POINT_LIGHT:
			ret = ComputePointLight(m, L, pos, nor, toCamera);
			break;
		case SPOT_LIGHT:
			ret = ComputeSpotLight(m, L, pos, nor, toCamera);
			break;
		case DIRECTIONAL_LIGHT:
			ret = ComputeDirectionalLight(m, L, nor, toCamera);
			break;
		default:
			break;
	}
	return ret;
}

/////////////////////////////////////////

//x : 1918
//y : 1061
// x : 0right1	y : 0down1
#define MaxLights 10
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);

	
	for (int i = 0; i < MaxLights; i++)
	{
		output += ComputeLight(material, lightInfos[i], input.originPosition, input.normal, cameraPos - input.originPosition);
	}
	
	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	return output;
}

float4 PSInstancing(INSTANCING_VS_OUTPUT input) : SV_TARGET
{
	float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for (int i = 0; i < MaxLights; i++)
	{
		output += ComputeLight(gameObjectInfos[input.index].meterial, lightInfos[i], input.originPosition, input.normal, normalize(cameraPos - input.originPosition));
	}
	
	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	return output;
}

float4 PSTerrain(VS_OUTPUT input) : SV_TARGET
{
	float4 output = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < MaxLights; i++)
	{
		output += ComputeLight(material, lightInfos[i], input.originPosition, input.normal, cameraPos - input.originPosition);
	}
	
	output -= Effect0Wall(input.originPosition);
	output += Effect1Obj(input.originPosition);
	
	
	return output;
}

float4 PSUI(UI_VS_OUTPUT input) : SV_TARGET
{
	float4 output = input.color;
	
	return output;
}