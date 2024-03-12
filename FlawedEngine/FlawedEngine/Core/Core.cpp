#include "Core.h"
#include "Models/ObjectManager.h"
#include "PerspectiveCamera.h"
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

	void RemoveEntity(const char* name)
	{
		cObjectManager& manager = cObjectManager::get();
		//manager.RemoveObject(name); // Can not remove the object in the middle of everything obvously
		manager.addToRemoveList(name);
	}

	std::function<bool(int)>& GetInputFunc()
	{
		cObjectManager& manager = cObjectManager::get();
		return manager.mInputFunc;
	}

	void* GetPhxsWorld()
	{
		cObjectManager& manager = cObjectManager::get();

		return manager.GetPhysicsWorld();
	}

	void MoveCamera(float dx, float dy, float dz)
	{
		cpCamera& camera = cpCamera::get();

		camera.MoveCamera(dx, dy, dz);
	}

	void SetCameraPos(float x, float y, float z)
	{
		cpCamera& camera = cpCamera::get();

		camera.SetPosition(x, y, z);
	}

	glm::vec3& GetDirectionalLightPos()
	{
		return DirectionalLightPos;
	}
}