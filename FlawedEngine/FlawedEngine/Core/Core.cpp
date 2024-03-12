#include "Core.h"
#include "Models/ObjectManager.h"
#include "PerspectiveCamera.h"
namespace FlawedEngine
{
	glm::vec3 DirectionalLightPos = glm::vec3(-2.0f, 10.0f, -1.0f);
	
	cObjectManager& manager = cObjectManager::get();
	cpCamera& camera = cpCamera::get();

	void SetDirectionalLightPos(const glm::vec3& Position)
	{
		DirectionalLightPos = Position;
	}

	void* GetEntity(const char* name)
	{

		return manager.GetObjectByName(name).get();
	}

	void* SpawnEntity(const char* name, eBasicObject type)
	{

		manager.AddObject(type, name);
		return manager.GetObjectByName(name).get();
	}

	void RemoveEntity(const char* name)
	{
		//manager.RemoveObject(name); // Can not remove the object in the middle of everything obvously
		manager.addToRemoveList(name);
	}

	std::function<bool(int)>& GetInputFunc()
	{
		return manager.mInputFunc;
	}

	void* GetPhxsWorld()
	{

		return manager.GetPhysicsWorld();
	}

	void MoveCamera(float dx, float dy, float dz)
	{

		camera.MoveCamera(dx, dy, dz);
	}

	void RotateCamera(float pitch, float yaw, float roll)
	{
		
		camera.RotateCamera(pitch, yaw, roll);
	}

	void SetCameraRotation(float pitch, float yaw, float roll)
	{
		camera.SetRotation(pitch, yaw, roll);
	}

	void SetCameraPos(float x, float y, float z)
	{
		camera.SetPosition(x, y, z);
	}

	glm::vec3& GetDirectionalLightPos()
	{
		return DirectionalLightPos;
	}
}