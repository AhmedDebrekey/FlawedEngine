#pragma once

#include "../Core.h"
#include "../Entity.h"
#include "OBJModel.h"
#include "skybox/Skybox.h"

namespace FlawedEngine
{
	class cObjectManager
	{
	public:		
		void Init(void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, void* CamFrustum, void* Graphics_API);
		
		void ShadowRender(sTransform& LightPerspective, glm::mat4& LightSpaceMatrix, uint32_t DepthMap);
		void RenderObjects(sTransform& tCamera);

		void AddObject(eBasicObject ObjectType, const char* Name);
		void LoadObject(const char* FilePath, const char* Name);
		void RemoveObject(const char* Name);

		void ModifyObject(const char* Name, sModel& Model, sMaterial Material, bool setPhysics, sPhysicsProps PhysicsProps);

		void* GetPhysicsWorld() { return mPhysicsWorld; }
		std::unordered_map<std::string, std::shared_ptr<cEntity>>* GetObjectsPointer() { return &SceneObjects; }
		
		sLight* GetLightProps(const char* Name);
		glm::vec3 GetLightColor(const char* Name);
		void ChangeLightColor(const char* Name, glm::vec3 Color);
		void ChangeLightDiffuse(const char* Name, glm::vec3 Diffuse);
		void ChangeLightSpecular(const char* Name, glm::vec3 Specular);
		void ChangeLightPosition(const char* Name, glm::vec3 Position);
		void ChangeLightQuadratic(const char* Name, float Quadratic);
		void ChangeLightConstant(const char* Name, float Constant);
		void ChangeLightLinear(const char* Name, float Linear);
		void ChangeName(const char* OldName, const char* NewName);
		std::shared_ptr<cEntity> GetObjectByName(const char* Name);
		void ToggleSkyBox() { mSkybox.ToggleSkyBox(); };
		void addToRemoveList(const char*);
		void eraseRemoveList();

		void Save(const std::string& FileName);
		void LoadSave(const std::string& FileName);
		bool isKeyDown(int key);
		std::function<bool(int)> mInputFunc;

		bool mMousePicking = true;

	private:
		void AddLight(const char* Name, sLight& Props);
		sLight* GetLightByName(const char* Name);

	private:
		std::unordered_map<std::string, std::shared_ptr<cEntity>> SceneObjects;
		std::unordered_map<std::string, sLight> PointLights;
		std::vector<std::string> mRemoveList;

		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;

		sTransform tCamera;

		void* mPhysicsWorld = nullptr;
	
	private:

		void* mCamFrustum = nullptr;
		void* mGfxAPI = nullptr;

	private:
		cSkybox mSkybox;

	public:
		static cObjectManager& get();
		~cObjectManager();

		cObjectManager(cObjectManager const&) = delete;
		void operator=(cObjectManager const&) = delete;

	private:
		static cObjectManager* sObjectManagerInstance;

		cObjectManager();
		cObjectManager(cObjectManager&);
	};
}

