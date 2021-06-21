#include "stdafx.h"
#include "Terrain.h"
#include "Shader.h"


HeightMapTerrain::HeightMapTerrain(
	ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, ID3D12RootSignature* rootSignature
	, string_view fileName
	, int width
	, int length
	, int blockWidth
	, int blockLength
	, XMFLOAT3 scale
	, XMVECTORF32 color
)
	: GameObject{ 0 }
	, m_scale{ scale }
	, m_heightMapImage{ make_unique<HeightMapImage>(fileName,width,length,scale) }
{
	int const xQuadsPerBlock{ blockWidth - 1 };
	int const zQuadsPerBlock{ blockLength - 1 };

	int const xBlocks{ (width - 1) / xQuadsPerBlock };
	int const zBlocks{ (length - 1) / zQuadsPerBlock };
	m_meshes.resize(xBlocks* zBlocks);

	HeighMapGridMesh* HMGM{ nullptr };
	for (int z = 0, zStart = 0; z < zBlocks; z++) {
		for (int x = 0, xStart = 0; x < xBlocks; x++) {
			xStart = x * xQuadsPerBlock;
			zStart = z * zQuadsPerBlock;
			HMGM = new HeighMapGridMesh{
				  device, commandList
				, xStart, zStart
				, blockWidth, blockLength
				, scale, color, m_heightMapImage.get() };
			SetMesh(x + z * xBlocks, HMGM);
		}
	}
	TerrainShader* shader{ new TerrainShader() };
	shader->CreateShader(device, rootSignature);
	SetShader(shader);
}

bool HeightMapTerrain::IsVisible(const Camera* camera)
{
	return true;
}