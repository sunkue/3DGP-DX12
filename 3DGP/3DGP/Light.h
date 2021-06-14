#pragma once

#include "ShaderHelper.hpp"
class GameObject;

enum class LIGHT_TYPE : __int32
{
	NO_LIGHT,
	POINT,
	SPOT,
	DIRECTIONAL
};

struct LightInfo
{
	LIGHT_TYPE m_type;
	XMFLOAT3 m_position;
	XMFLOAT4 m_ambient;
	XMFLOAT4 m_diffuse;
	XMFLOAT4 m_specular;
	XMFLOAT3 m_attenuation;
	float m_range;
	XMFLOAT3 m_direction;
	float m_falloff;
	float m_theta;
	float m_phi;

	LightInfo() = default;
	LightInfo(LIGHT_TYPE t, XMFLOAT3 pos
		, XMFLOAT4 amb, XMFLOAT4 diff, XMFLOAT4 spec
		, XMFLOAT3 atten, float range
		, XMFLOAT3 dir, float falloff
		, float theta, float phi
	)
		: m_type{ t }
		, m_position{ pos }
		, m_ambient{ amb }
		, m_diffuse{ diff }
		, m_specular{ spec }
		, m_attenuation{ atten }
		, m_range{ range }
		, m_direction{ dir }
		, m_falloff{ falloff }
		, m_theta{ theta }
		, m_phi{ phi }
	{}
};

struct LightFactory
{
	static shared_ptr<LightInfo> MakePointLight(XMFLOAT3 pos, float range)
	{
		shared_ptr<LightInfo> ret;
		ret = make_shared<LightInfo>(
			  LIGHT_TYPE::POINT
			, pos
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT3{ 1.0f,10.0f,100.0f }
			, range
			, XMFLOAT3{}
			, 0.0f
			, 0.0f
			, 0.0f
			);
		return ret;
	}

	static shared_ptr<LightInfo> MakeSpotLight(XMFLOAT3 pos, XMFLOAT3 dir
		, float range, float falloff, float theta, float phi
	)
	{
		shared_ptr<LightInfo> ret;
		ret = make_shared<LightInfo>(
			  LIGHT_TYPE::SPOT
			, pos
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT3{ 1.0f,10.0f,100.0f }
			, range
			, dir
			, falloff
			, theta
			, phi
			);
		return ret;
	}

	static shared_ptr<LightInfo> MakeDirectinalLight(XMFLOAT3 dir)
	{
		shared_ptr<LightInfo> ret;
		ret = make_shared<LightInfo>(
			  LIGHT_TYPE::SPOT
			, XMFLOAT3{ 0.0f,0.0f,0.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT4{ 1.0f,1.0f,1.0f,1.0f }
			, XMFLOAT3{ 1.0f,10.0f,100.0f }
			, 0.0f
			, dir
			, 0.0f
			, 0.0f
			, 0.0f
			);
		return ret;
	}
};

class Light : public IShaderHelper
{
	static const int MaxLights{ 10 };
public:
	Light(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~Light();

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	void AddLight(shared_ptr<LightInfo> lightInfo) { Lights.push_back(lightInfo); }

protected:
	ComPtr<ID3D12Resource>	mcbLights;
	LightInfo* mcbMappedLights;
	vector<shared_ptr<LightInfo>> Lights;
};


class LightObj : public GameObject
{
public:
	LightObj(shared_ptr<LightInfo> = nullptr, int meshes = 0);
	virtual ~LightObj() {};

public:
	void SetLightColor(XMFLOAT4 lightColor) { 
		m_light->m_ambient = lightColor;
		m_light->m_diffuse = lightColor;
		m_light->m_specular = lightColor;
	};
	void SetLightColor(XMVECTORF32 lightColor) {
		XMFLOAT4 col; XMStoreFloat4(&col, lightColor);
		SetLightColor(col);
	};
	void SetLightColor(XMVECTOR lightColor) {
		XMFLOAT4 col; XMStoreFloat4(&col, lightColor);
		SetLightColor(col);
	};

	shared_ptr<LightInfo> GetLightInfo()const { return m_light; }

protected:
	shared_ptr<LightInfo> m_light;
};