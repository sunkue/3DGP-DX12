#pragma once

#include "GameObject.h"

enum class LIGHT_TYPE : __int32
{
	NO_LIGHT,
	POINT,
	SPOT,
	DIRECTIONAL
};
static constexpr XMFLOAT3 default_attenuation{ 1.0f,0.000007f,0.00000002f };
static constexpr XMFLOAT3 default_color{ 1.0f,1.0f,1.0f };
struct SR_LIGHT_INFO
{
	LIGHT_TYPE m_type{ LIGHT_TYPE::NO_LIGHT };
	XMFLOAT3 m_position{ 0,0,0 };
	XMFLOAT3 m_ambient{ default_color };
	float m_range{ 0 };
	XMFLOAT3 m_diffuse{ default_color };
	float m_falloff{ 0 };
	XMFLOAT3 m_specular{ default_color };
	float m_theta{ 0 };
	XMFLOAT3 m_attenuation{ default_attenuation };
	float m_phi{ 0 };
	XMFLOAT3 m_direction{ 0,0,0 };

	SR_LIGHT_INFO() = default;
	SR_LIGHT_INFO(LIGHT_TYPE t, XMFLOAT3 pos
		, XMFLOAT3 amb, XMFLOAT3 diff, XMFLOAT3 spec
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
	static shared_ptr<SR_LIGHT_INFO> MakePointLight(XMFLOAT3 pos, float range)
	{
		shared_ptr<SR_LIGHT_INFO> ret;
		ret = make_shared<SR_LIGHT_INFO>(
			  LIGHT_TYPE::POINT
			, pos
			, default_color
			, default_color
			, default_color
			, default_attenuation
			, range
			, XMFLOAT3{}
			, 0.0f
			, 0.0f
			, 0.0f
			);
		return ret;
	}

	static shared_ptr<SR_LIGHT_INFO> MakeSpotLight(XMFLOAT3 pos, XMFLOAT3 dir
		, float range, float falloff, float theta, float phi
	)
	{
		shared_ptr<SR_LIGHT_INFO> ret;
		ret = make_shared<SR_LIGHT_INFO>(
			  LIGHT_TYPE::SPOT
			, pos
			, default_color
			, default_color
			, default_color
			, default_attenuation
			, range
			, dir
			, falloff
			, theta
			, phi
			);
		return ret;
	}

	static shared_ptr<SR_LIGHT_INFO> MakeDirectinalLight(XMFLOAT3 dir)
	{
		shared_ptr<SR_LIGHT_INFO> ret;
		ret = make_shared<SR_LIGHT_INFO>(
			  LIGHT_TYPE::SPOT
			, XMFLOAT3{ 0.0f,0.0f,0.0f }
			, default_color
			, default_color
			, default_color
			, default_attenuation
			, 0.0f
			, dir
			, 0.0f
			, 0.0f
			, 0.0f
			);
		return ret;
	}
};

class LightShader : public IShaderResourceHelper
{
	static const int MaxLights{ 10 };
public:
	LightShader(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~LightShader() = default;

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	void AddLight(shared_ptr<SR_LIGHT_INFO> lightInfo) { m_Lights.push_back(lightInfo); }
	shared_ptr<SR_LIGHT_INFO> GetLight(size_t index) { return m_Lights.at(index); }
	void XM_CALLCONV SortByDist(XMVECTOR point);

protected:
	vector<shared_ptr<SR_LIGHT_INFO>> m_Lights;
	ComPtr<ID3D12Resource> m_RSRC;
	SR_LIGHT_INFO* m_Mapped_RSRC;
	UINT m_RootParamIndex;
};


class LightObj : public GameObject
{
public:
	LightObj(ID3D12Device* device, ID3D12RootSignature* rootSignature, shared_ptr<SR_LIGHT_INFO> = nullptr, int meshes = 0);
	virtual ~LightObj() {};

public:
	void SetLightColor(XMFLOAT3 lightColor) { 
		m_light->m_ambient = lightColor;
		m_light->m_diffuse = lightColor;
		m_light->m_specular = lightColor;
	};
	void SetLightColor(XMVECTORF32 lightColor) {
		XMFLOAT3 col; Store(col, lightColor);
		SetLightColor(col);
	};
	void SetLightColor(XMVECTOR lightColor) {
		XMFLOAT3 col; Store(col, lightColor);
		SetLightColor(col);
	};

	shared_ptr<SR_LIGHT_INFO> GetLightInfo()const { return m_light; }

protected:
	shared_ptr<SR_LIGHT_INFO> m_light;
};
