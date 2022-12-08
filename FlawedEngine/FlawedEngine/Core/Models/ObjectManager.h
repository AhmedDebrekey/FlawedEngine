#pragma once

#include "../Core.h"
#include "../Entity.h"
#include "OBJModel.h"

namespace FlawedEngine
{
	class cObjectManager
	{
	public:
		cObjectManager();
		~cObjectManager();
		
		void Init(void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);

		void RenderObjects(sTransform& tCamera);

		void AddObject(eBasicObject ObjectType, const char* Name);
		void RemoveObject(const char* Name);

		void ModifyObject(const char* Name, sModel& Model, sMaterial Material, bool setPhysics, sPhysicsProps PhysicsProps);

		void* GetPhysicsWorld() { return mPhysicsWorld; }
		std::unordered_map<std::string, std::shared_ptr<cEntity>>* GetObjectsPointer() { return &SceneObjects; }
		
		glm::vec3 GetLightColor(const char* Name);
		void ChangeLightColor(const char* Name, glm::vec3 Color);
		void ChangeLightPosition(const char* Name, glm::vec3 Position);
		void ChangeName(const char* OldName, const char* NewName); 
		std::shared_ptr<cEntity> GetObjectByName(const char* Name);

	private:
		void AddLight(const char* Name, sLight& Props);
		sLight* GetLightByName(const char* Name);

	private:
		std::unordered_map<std::string, std::shared_ptr<cEntity>> SceneObjects;
		std::unordered_map<std::string, sLight> PointLights;

		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;

		sTransform tCamera;

		void* mPhysicsWorld = nullptr;

	private:
		void SetupSkybox();
		void RenderSkyBox();
		uint32_t loadCubemap(std::vector<std::string> faces);
		cShader mSkyboxShader;
		unsigned int mskyboxVAO, mskyboxVBO;
		uint32_t mCubeMapTexture;
	};
}

