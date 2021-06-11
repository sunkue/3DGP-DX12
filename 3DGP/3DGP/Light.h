#pragma once

class GameObject;

enum class LIGHT_TYPE : __int32
{
	POINT,
	SPOT,
	DIRECTIONAL
};

struct Light
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

	Light() = default;
	Light(LIGHT_TYPE t, XMFLOAT3 pos
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
	static shared_ptr<Light> MakePointLight(XMFLOAT3 pos, float range)
	{
		shared_ptr<Light> ret;
		ret = make_shared<Light>(
				new Light{
				 { LIGHT_TYPE::POINT }
				, pos
				, { 1.0f,1.0f,1.0f,1.0f }
				, { 1.0f,1.0f,1.0f,1.0f }
				, { 1.0f,1.0f,1.0f,1.0f }
				, { 1.0f,10.0f,100.0f }
				, { range }
				, {}
				, {}
				, {}
				, {}
			});
		return ret;
	}

	static shared_ptr<Light> MakeSpotLight(XMFLOAT3 pos, XMFLOAT3 dir
		, float range, float falloff, float theta, float phi
	)
	{
		shared_ptr<Light> ret;
		ret = make_shared<Light>(
			new Light{
			 { LIGHT_TYPE::SPOT }
			, pos
			, { 1.0f,1.0f,1.0f,1.0f }
			, { 1.0f,1.0f,1.0f,1.0f }
			, { 1.0f,1.0f,1.0f,1.0f }
			, { 1.0f,10.0f,100.0f }
			, { range }
			, dir
			, falloff
			, theta
			, phi
			});
		return ret;
	}

	static shared_ptr<Light> MakeDirectinalLight(XMFLOAT3 dir)
	{
		shared_ptr<Light> ret;
		ret = make_shared<Light>(
			new Light{
			 { LIGHT_TYPE::SPOT }
			, {}
			, { 1.0f,1.0f,1.0f,1.0f }
			, { 1.0f,1.0f,1.0f,1.0f }
			, { 1.0f,1.0f,1.0f,1.0f }
			, { 1.0f,10.0f,100.0f }
			, {}
			, dir
			, {}
			, {}
			, {}
			});
		return ret;
	}
};



class LightObj : public GameObject
{
public:
	LightObj(shared_ptr<Light> = nullptr, int meshes = 0);
	virtual ~LightObj() {};

public:
	virtual void Animate(const milliseconds timeElapsed)override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

protected:
	virtual void PrepareRender()override;
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override;

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

protected:
	shared_ptr<Light> m_light;
};

