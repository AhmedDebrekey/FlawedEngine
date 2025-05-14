#include "Core.h"
#include "Models/ObjectManager.h"
#include "PerspectiveCamera.h"
namespace FlawedEngine
{
	glm::vec3 DirectionalLightPos = glm::vec3(-2.0f, 10.0f, -1.0f);
	
	cObjectManager& manager = cObjectManager::get();
	cpCamera& camera = cpCamera::get();
	cScriptingManager& scripts = cScriptingManager::get();
	bool mIsPlaying = false;


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

	void* LoadEntity(const char* path, const char* name)
	{
		manager.LoadObject(path, name);
		return manager.GetObjectByName(name).get();
	}

	void RemoveEntity(const char* name)
	{
		//manager.RemoveObject(name); // Can not remove the object in the middle of everything obvously
		manager.RemoveFromRuntimeObjects(name);
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

	float GetCamYaw()
	{
		return camera.GetYaw();
	}

	float GetCamPitch()
	{
		return camera.GetPitch();
	}

	void SetCameraPos(float x, float y, float z)
	{
		camera.SetPosition(x, y, z);
	}

	glm::vec3& GetDirectionalLightPos()
	{
		return DirectionalLightPos;
	}

	static sTransform cameraTransform;

	void StartPlayMode()
	{
		mIsPlaying = true; 

		manager.Save("game_config.json");

		cameraTransform = { camera.Postion(), camera.Front(), camera.Projection(), camera.View() };
		camera.SetAllowInput(false);

		scripts.ReloadScripts();
		EngineLog("PlayMode ON", Info);
	}

	void StopPlayMode()
	{
		mIsPlaying = false;
		manager.RemoveRuntimeObjects();
		manager.LoadSave("game_config.json");
		camera.SetOrientation(cameraTransform);
		camera.SetAllowInput(true);

		EngineLog("PlayMode OFF", Info);
	}

}