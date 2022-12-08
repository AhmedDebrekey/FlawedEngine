#include "Scene.h"

#include "Shader.h"

namespace FlawedEngine
{
	cScene::cScene(void* Window, void* Physics, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
		:mWindow(Window), Camera(mWindow), PhysicsWorld(Physics), mCollisionShapesArray(CollisionShapes)
	{
		Setup();
	}

	cScene::~cScene()
	{
	}

	void cScene::Setup()
	{
		ObjectMan.Init(PhysicsWorld, mCollisionShapesArray);
	}

	void cScene::Render()
	{
		Camera.Compute();
		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};
		ObjectMan.RenderObjects(tCamera);
	}
}