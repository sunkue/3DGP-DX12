#pragma once

#include "Mesh.h"

class HeightMapImage
{
public:
	HeightMapImage(string_view fileName, int width, int length, XMFLOAT3 scale);
	~HeightMapImage() = default;

	float GetHeight(float x, float z)const;
	XMVECTOR XM_CALLCONV GetNormal(int x, int z)const;

	XMFLOAT3 GetScale()const { return m_scale; }
	const BYTE* GetPixels()const { return m_heightMapPixels.get(); }
	int GetWidth()const { return m_width; }
	int GetLength()const { return m_length; }

private:
	template<typename N>requires is_arithmetic<N>::value
		bool const OutOfRange(N w, N l)const { return (w < 0) || (l < 0) || (m_width <= w) || (m_length <= l); }

private:
	shared_ptr<BYTE[]> m_heightMapPixels;
	int m_width;
	int m_length;
	XMFLOAT3 m_scale;
};

class HeighMapGridMesh : public MeshUnit
{
public:
	HeighMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, int Xstart, int Zstart, int width, int length
		, XMFLOAT3 scale = { 1.0f,1.0f,1.0f }
		, XMVECTORF32 color = Colors::Yellow
		, void* context = nullptr);
	virtual ~HeighMapGridMesh() = default;

	XMFLOAT3 const GetScale()const { return m_scale; }
	int const GetWidth()const { return m_width; }
	int const GetLength()const { return m_length; }

	virtual float const OnGetHeight(int x, int z, void* context)const;

protected:
	int m_width;
	int m_length;
	XMFLOAT3 m_scale;
};