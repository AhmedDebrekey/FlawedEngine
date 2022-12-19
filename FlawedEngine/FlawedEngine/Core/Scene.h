#pragma once

#include <unordered_map>
#include <vector>
#include <Bullet/btBulletDynamicsCommon.h>

#include "Entity.h"
#include "PerspectiveCamera.h"
#include "Models/ObjectManager.h"


namespace FlawedEngine
{
	///<summary>Scene contains all objects in the world, every world has it's object manager, 
	/// managing the 3d models. Every scene should in the future be able to seralize its self and 
	/// deserialize as well, however this has not been implemented yet.</summary>
	class cScene
	{
	public:
		cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes);
		~cScene();
		void Setup();
		void Render();

		cObjectManager* GetObjectManager() { return &ObjectMan; }
		cpCamera* GetCamera() { return &Camera; }
		void UpdateViewport(glm::vec2 Size, glm::vec2 Pos) { ViewportSize = Size; ViewportPos = Pos; }
		void SetSelectedEntity(std::string& SelectedEntity) { mSelectedEntity = &SelectedEntity; }
	private:
		cObjectManager ObjectMan;
		void* mWindow;
		void* PhysicsWorld = nullptr;
		cpCamera Camera;
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;
	private:
		glm::vec2 ViewportSize = { 1600, 900 };
		glm::vec2 ViewportPos = { 1600, 900 };
		glm::vec2 CursorPos = { 0,0 };
		void onCursorPos(glm::vec2 pos) { CursorPos = pos; }
	private:
		std::string* mSelectedEntity;
	};
}