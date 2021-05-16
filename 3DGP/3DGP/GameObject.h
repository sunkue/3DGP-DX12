#pragma once

#include "Mesh.h"

class Shader;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release()	{ assert(0 < mReferences);  if (--mReferences == 0)delete this; }

private:
	int mReferences;

public:
	void ReleaseUploadBuffers();

	virtual void SetMesh(Mesh* mesh);
	virtual void SetShader(Shader* shader);

	virtual void Animate(milliseconds timeElapsed);

	virtual void PrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* commandList);

protected:
	XMFLOAT4X4A	mxmf4x4World;
	Mesh*	mMesh;
	Shader*	mShader;

};

