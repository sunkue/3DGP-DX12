#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"


/// ////////////////////////////////////////

GameObject::GameObject()
	: mMesh{ nullptr }
	, mShader{ nullptr }
	, mReferences{ 0 }
{
	XMStoreFloat4x4A(&mxmf4x4World, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	SAFE_RELEASE(mMesh);
	if (mShader) {
		mShader->ReleaseShaderVariables();
		mShader->Release();
	}
}

/// ////////////////////////////////////////

void GameObject::SetMesh(Mesh* mesh)
{
	SAFE_RELEASE(mMesh);
	mMesh = mesh;
	SAFE_ADDREF(mesh);
}

void GameObject::SetShader(Shader* shader)
{
	SAFE_RELEASE(mShader);
	mShader = shader;
	SAFE_ADDREF(shader);
}

void GameObject::ReleaseUploadBuffers()
{
	if (mMesh)mMesh->ReleaseUploadBuffers();
}


/// ////////////////////////////////////////

void GameObject::Animate(milliseconds timeElapsed)
{

}

void GameObject::PrepareRender()
{

}

void GameObject::Render(ID3D12GraphicsCommandList* commandList)
{
	PrepareRender();
	if (mShader)mShader->Render(commandList);
	if (mMesh)mMesh->Render(commandList);
}

