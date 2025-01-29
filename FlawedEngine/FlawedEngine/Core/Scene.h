#pragma once

#include <unordered_map>
#include <vector>
#include <Bullet/btBulletDynamicsCommon.h>

#include "Entity.h"
#include "PerspectiveCamera.h"
#include "Models/ObjectManager.h"

#include "Graphics/OpenGLAPI.h"



namespace FlawedEngine
{
	///<summary>Scene contains all objects in the world, every world has it's object manager, 
	/// managing the 3d models. Every Scene is serialized on command, but there are no differentiation
	/// between different scenes.</summary>
	class cScene
	{
	public:
		cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, void* Graphic_API);
		~cScene();
		void Setup();
		void Render();

		cObjectManager* GetObjectManager() { return &ObjectMan; }
		cpCamera* GetCamera() { return &Camera; }
		void SetUIFrameBuffer(sFrameBuffer& FrameBuffer) { mUIFrameBuffer = &FrameBuffer; }

		bool isKeyDown(int key);

	private:
		cObjectManager& ObjectMan = cObjectManager::get();

		void* mWindow;
		void* PhysicsWorld = nullptr;
		void* mGfxAPI = nullptr;
		cpCamera& Camera = cpCamera::get();
		btAlignedObjectArray<btCollisionShape*>* mCollisionShapesArray = nullptr;
	private:
		std::string* mSelectedEntity;
	private:

		const uint32_t SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
		uint32_t mDepthMapFBO	= 0;
		uint32_t mDepthMap		= 0;
		sFrameBuffer* mUIFrameBuffer = nullptr;
	private:
		sGBufferObjects mGeometryObject;
	};
}