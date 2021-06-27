#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "GameFramework.h"
#include "Effect.h"

void Scene::ReleaseObjects()
{
	m_RootSignatures.clear();
	m_Terrains.clear();
	m_Shaders.clear();
};

void Scene::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	PrepareRender(commandList, camera);
	for (auto& shader : m_Shaders)shader->Render(commandList, camera);
}

void Scene::AnimateObjects(float timeElapsed)
{
	for (auto& obj : m_Objects)obj->Animate(timeElapsed);
}