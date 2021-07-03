#pragma once

#include "GameObject.h"
#include "HeightMap.h"

class HeightMapTerrain
{
public:
	HeightMapTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature, string_view fileName
		, int width, int length
		, int blockWidth, int blockLength
		, XMFLOAT3 scale, XMVECTORF32 color);
	virtual ~HeightMapTerrain() = default;

public:
	float GetHeight(float x, float z)const { return m_heightMapImage->GetHeight(x / m_scale.x, z / m_scale.z) * m_scale.y; }
	XMVECTOR XM_CALLCONV GetNormal(float x, float z)const { return m_heightMapImage->GetNormal(int(x / m_scale.x), int(z / m_scale.z)); }

	int GetHeightMapWidth()const { return m_heightMapImage->GetWidth(); }
	int GetHeightMapLength()const { return m_heightMapImage->GetLength(); }

	auto GetScale()const { return m_scale; }
	float GetWidth()const { return m_heightMapImage->GetWidth() * m_scale.x; }
	float GetLength()const { return m_heightMapImage->GetLength() * m_scale.z; }

	virtual bool IsVisible(const Camera* camera = nullptr);

private:
	unique_ptr<HeightMapImage> m_heightMapImage;
	XMFLOAT3 m_scale;

};