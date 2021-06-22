#pragma once

#include "Mesh.h"
#include "GameObject.h"
#include "Shader.h"

struct SR_INSTANCE_INFO
{
	XMFLOAT4X4	transform;
	Meterial	meterial;
};

template<class SR> class InstancingShader abstract : public SRVShader<SR>
{
protected:
	virtual void OnRender(ID3D12GraphicsCommandList* commandList, Camera* camera) override
	{
		m_Objects.at(0)->Render(commandList, camera, static_cast<UINT>(m_Objects.size()));
	}
};


// example
template<> class InstancingShader<SR_INSTANCE_INFO> : public SRVShader<SR_INSTANCE_INFO>
{
protected:
	virtual void OnUpdateShaderVariables() override
	{
		for (size_t i = 0; i < m_Objects.size(); ++i) {
			//m_Mapped_RSRC[i].meterial = Meterial{ {}, {}, {1.0f,1.0f,1.0f,0.0f},{ 0.0f,0.0f,0.0f,0.0f },60.0f };
			Store(m_Mapped_RSRC[i].transform, XMMatrixTranspose(m_Objects[i]->GetWM()));
		}
	}
};