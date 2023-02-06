#pragma once
#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "OBJ_Loader.h"
#include <unordered_map>
#include "../Core.h"

#include <functional>

#include <ImGui/imgui.h>

namespace FlawedEngine
{
	class cOBJModel : public cEntity
	{
	public:								//Have to copy it
		cOBJModel(const char* FilePath, std::string Name, void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);
		~cOBJModel();
		virtual void Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics(eBasicObject Object, void* PhysicsWorld) override;
		virtual void UnSetPhysics() override;
		virtual void SetupScripting() override;
		virtual void SendEntity(cEntity* Entity) override;
		virtual void SendInputToScripting(std::function<bool(int)>) override;
		bool LoadModel(const char* FilePath);
		void Populate();
		void SetCollisionShape(eBasicObject Object);
		bool isPhysicsSet = false;
		std::string mName;

		void LMove(float x, float y, float z);
		void LSetColor(float x, float y, float z);
		void LRotate(float x, float y, float z);
		void LScale(float x, float y, float z);
		void LApplyForce(float x, float y, float z);
		void LApplyRelativeForce(float x, float y, float z);
		void LAddObject();
		lua_State* LuaState = nullptr;

	private:
		btDiscreteDynamicsWorld* mPhysicsDynamicWorld;
		btCollisionShape* mCollisionShape;
		cModelRenderer Renderer;
		objl::Loader mLoader;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray;

		int ScriptingId;
	};
}
