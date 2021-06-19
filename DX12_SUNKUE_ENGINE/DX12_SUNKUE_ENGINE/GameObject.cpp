#include "stdafx.h"
#include "GameFramework.h"
#include "GameObject.h"
#include "Player.h"
#include "Shader.h"

/// //////////////

EnemyObject::TEAM& operator++(EnemyObject::TEAM& t) {
	if (EnemyObject::TEAM::ENDCOUNT == t)t = static_cast<EnemyObject::TEAM>(0);
	else t = static_cast<EnemyObject::TEAM>((int)t + 1);
	return t;
}
EnemyObject::TEAM operator++(EnemyObject::TEAM& t, int) {
	auto temp = t;
	++t;
	return temp;
}
/// ////////////////////////////////////////

GameObject::GameObject(int meshes)
	: mShader{ nullptr }
	, m_scale{ 1.0f,1.0f,1.0f }
	, mReferences{ 0 }
	, m_brother{ nullptr }
	, m_child{ nullptr }
{
	XMStoreFloat4x4A(&mWorldMat, XMMatrixIdentity());
	assert(0 <= meshes);
	mMesh.resize(meshes);
	assert(mMesh.size() == meshes);
}

GameObject::~GameObject()
{
	for (auto& m : mMesh) SAFE_RELEASE(m);
	mMesh.clear();
	if (mShader) {
		mShader->ReleaseShaderVariables();
		mShader->Release();
	}
	ForFamily([](GameObject& a) {a.~GameObject(); });
}

/// ////////////////////////////////////////

void GameObject::SetMesh(int index, VertexBufferData* mesh)
{
	SAFE_RELEASE(mMesh[index]);
	mMesh[index] = mesh;
	SAFE_ADDREF(mesh);
	assert(mMesh[index]);
}

void GameObject::SetShader(Shader* shader)
{
	SAFE_RELEASE(mShader);
	mShader = shader;
	SAFE_ADDREF(shader);
}

void GameObject::ReleaseUploadBuffers()
{
	for (auto& m : mMesh)m->ReleaseUploadBuffers();
	ForFamily([](auto& obj) {obj.ReleaseUploadBuffers(); });

}


/// ////////////////////////////////////////

void GameObject::Animate(milliseconds timeElapsed)
{
	ForFamily([=](auto& obj) {obj.Animate(timeElapsed); });
}

void GameObject::PrepareRender()
{

}


void GameObject::Render(ID3D12GraphicsCommandList* commandList, Camera* camera, UINT instanceCount)
{
	PrepareRender();
	for (auto& m : mMesh)m->Render(commandList, instanceCount);
	ForFamily([=](auto& obj) {obj.Render(commandList, camera, instanceCount); });
}

void GameObject::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	if (false == IsVisible(camera))return;

	PrepareRender();
	UpdateShaderVariables(commandList);

	assert(mShader);
	mShader->Render(commandList, camera);

	for (auto& m : mMesh)m->Render(commandList);
	ForFamily([=](auto& obj) {obj.Render(commandList, camera); });
}

void GameObject::Render(
	  ID3D12GraphicsCommandList* commandList
	, Camera* camera
	, UINT instanceCount
	, D3D12_VERTEX_BUFFER_VIEW instancingBufferView)
{
	PrepareRender();
	for (auto& m : mMesh)m->Render(commandList, instanceCount, instancingBufferView);
	ForFamily([=](auto& obj) {obj.Render(commandList, camera, instanceCount, instancingBufferView); });
}

void GameObject::RotateByAxis(FXMVECTOR axis, const float angle)
{
	XMMATRIX rotate{ XMMatrixRotationAxis(axis,XMConvertToRadians(angle)) };
	XMStoreFloat4x4A(&mWorldMat, rotate * XMLoadFloat4x4A(&mWorldMat));
}

void GameObject::RotateByPYR(float pitch, float yaw, float roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4A(&mWorldMat, rotate * XMLoadFloat4x4A(&mWorldMat));
}

void GameObject::SetPosition(float x, float y, float z)
{
	mWorldMat._41 = x;
	mWorldMat._42 = y;
	mWorldMat._43 = z;
}

void GameObject::SetPosition(FXMVECTOR pos)
{
	SetPosition(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));
	
}

XMVECTOR GameObject::GetPosition()	const
{
	XMVECTOR pos{ XMVectorSet(mWorldMat._41,mWorldMat._42,mWorldMat._43,1.0f) };
	return pos;
}

XMVECTOR GameObject::GetLook() const
{
	XMVECTOR look{ XMVectorSet(mWorldMat._31,mWorldMat._32,mWorldMat._33,0.0f) };
	return XMVector3Normalize(look);
}

XMVECTOR GameObject::GetUp() const
{
	XMVECTOR up{ XMVectorSet(mWorldMat._21,mWorldMat._22,mWorldMat._23,0.0f) };
	return XMVector3Normalize(up);
}

XMVECTOR GameObject::GetRight() const
{
	XMVECTOR right{ XMVectorSet(mWorldMat._21,mWorldMat._22,mWorldMat._23,0.0f) };
	return XMVector3Normalize(right);
}

void GameObject::MoveRight(float distance)
{
	XMVECTOR pos = GetPosition();
	XMVECTOR right = GetRight();
	pos += right * distance;
	SetPosition(pos);
}

void GameObject::MoveUp(float distance)
{
	XMVECTOR pos = GetPosition();
	XMVECTOR up = GetUp();
	pos += up * distance;
	SetPosition(pos);
}

void GameObject::MoveFoward(float distance)
{
	XMVECTOR pos = GetPosition();
	XMVECTOR look = GetLook();
	pos += look * distance;
	SetPosition(pos);
}

void GameObject::Move(FXMVECTOR vel)
{
	XMVECTOR pos = GetPosition();
	pos += vel;
	SetPosition(pos);
}

void GameObject::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{

}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4A world;
	XMStoreFloat4x4A(&world, XMMatrixTranspose(GetWM()));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
}

void GameObject::ReleaseShaderVariables()
{

}

void GameObject::UpdateBoundingBox()
{
	assert(mMesh.empty() == false);
	assert(mMesh[0]);
	mMesh[0]->GetOOBB().Transform(m_OOBB, GetWM());
	XMStoreFloat4(&m_OOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_OOBB.Orientation)));
}

bool const GameObject::IsVisible(Camera const* const camera)
{
	if (nullptr == camera)return false;
	PrepareRender();
	BoundingOrientedBox OOBB = mMesh.at(0)->GetOOBB();
	OOBB.Transform(OOBB, GetWM());
	return camera->IsInFrustum(OOBB);
}

//////////////////////////////



/////////////////////////////
EnemyObject::EnemyObject(int meshes)
	: GameObject(meshes)
	, mRotationSpeed{ 90.0f }
	, mRotationAxis{ 0.0f, 1.0f, 0.0f }
	, mSpeed{ 0.0f }
	, m_able{ true }
{
	XMStoreFloat3A(&mDir, XMVectorZero());
}


EnemyObject::~EnemyObject()
{

}

void EnemyObject::Animate(milliseconds timeElapsed)
{
	const float timeE{ timeElapsed.count() / 1000.0f };
	RotateByAxis(XMLoadFloat3A(&mRotationAxis), mRotationSpeed * timeE);
	Move(XMLoadFloat3A(&mDir) * mSpeed * timeE);
	
	UpdateBoundingBox();

}

////////////////////


///////////////////////////////////


HeightMapTerrain::HeightMapTerrain(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, ID3D12RootSignature* rootSignature
	, string_view fileName
	, int width
	, int length
	, int blockWidth
	, int blockLength
	, XMFLOAT3A scale
	, XMVECTORF32 color
)
	: GameObject{ 0 }
	, m_width{ width }
	, m_length{ length }
	, m_scale{ scale }
	, mHeightMapImage{ make_unique<HeightMapImage>(fileName,width,length,scale) }
{
	int const xQuadsPerBlock{ blockWidth - 1 };
	int const zQuadsPerBlock{ blockLength - 1 };

	long const xBlocks{ (width - 1) / xQuadsPerBlock };
	long const zBlocks{ (length - 1) / zQuadsPerBlock };
	mMesh.resize(static_cast<size_t>(xBlocks)* zBlocks);

	HeighMapGridMesh* HMGM{ nullptr };
	for (int z = 0, zStart = 0; z < zBlocks; z++) {
		for (int x = 0, xStart = 0; x < xBlocks; x++) {
			xStart = x * xQuadsPerBlock;
			zStart = z * zQuadsPerBlock;
			HMGM = new HeighMapGridMesh{
				  device, commandList
				, xStart, zStart
				, blockWidth, blockLength
				, scale, color, mHeightMapImage.get() };
			SetMesh(x + z * xBlocks, HMGM);
		}
	}
	TerrainShader* shader{ new TerrainShader() };
	shader->CreateShader(device, rootSignature);
	SetShader(shader);
}

HeightMapTerrain::~HeightMapTerrain()
{
	
}

bool const HeightMapTerrain::IsVisible(Camera const* const camera)
{
	return true;
}


////////////////////////


UIObject::UIObject(int meshes)
	: GameObject{ meshes }
{

}

UIObject::~UIObject()
{

}

void UIObject::Animate(const milliseconds timeElapsed)
{

}


bool const UIObject::IsVisible(Camera const* const camera)
{
	return true;
}