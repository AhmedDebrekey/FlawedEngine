#include "Core.h"
#include "Models/ObjectManager.h"
namespace FlawedEngine
{
	glm::vec3 DirectionalLightPos = glm::vec3(-2.0f, 10.0f, -1.0f);


	void SetDirectionalLightPos(const glm::vec3& Position)
	{
		DirectionalLightPos = Position;
	}

	void* GetEntity(const char* name)
	{
		cObjectManager& manager = cObjectManager::get();
		return manager.GetObjectByName(name).get();
	}

	void* SpawnEntity(const char* name, eBasicObject type)
	{
		cObjectManager& manager = cObjectManager::get();
		manager.AddObject(type, name);
		return manager.GetObjectByName(name).get();
	}

	glm::vec3& GetDirectionalLightPos()
	{
		return DirectionalLightPos;
	}
}