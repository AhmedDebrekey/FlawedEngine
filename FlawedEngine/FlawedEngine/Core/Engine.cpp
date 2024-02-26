#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		mEngineWindow.Init();
		mEngineWindow.CreateWindow();
		
		mPhysicsWorld = mPhysics.Init();
		mGfxAPI = std::make_shared<cOpenGLAPI>();
		mGfxAPI->EnableFaceCulling(true);
		{
			mScenes.push_back(std::make_shared<cScene>(mEngineWindow.GetWindow(), mPhysicsWorld, mPhysics.GetCollisionShapesArray(), mGfxAPI.get()));
			mActiveScene = mScenes.at(mScenes.size() - 1);
		}

		mUI.Init(mEngineWindow.GetWindow(), mPhysicsWorld, mGfxAPI.get());
		mActiveScene->SetUIFrameBuffer(mUI.GetFrameBuffer());
	}

	void cEngine::Run()
	{
		float DeltaTime = 0;
		float LastFrame = 0;
		while (!mEngineWindow.ShouldClose())
		{
			float currentFrame = mEngineWindow.GetTime();
			DeltaTime = currentFrame - LastFrame;
			LastFrame = currentFrame;

			mEngineWindow.Update();
			
			mUI.UpdateUI();
			OnEvent();
			
			mPhysics.Update(DeltaTime);

			mActiveScene->Render();

			mUI.RenderUI();

			mEngineWindow.EndFrame();
		}
	}

	void cEngine::OnEvent()
	{
		mEngineWindow.PollEvents();
	}

	cEngine::~cEngine()
	{

	}

}