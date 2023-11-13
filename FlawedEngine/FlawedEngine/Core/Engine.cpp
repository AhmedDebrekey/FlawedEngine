#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		mPhysicsWorld = Physics.Init();
		mGraphics_API = std::make_shared<cOpenGLAPI>();
		mGraphics_API->EnableFaceCulling(true);
		{
			mScenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow(), mPhysicsWorld, Physics.GetCollisionShapesArray(), mGraphics_API.get()));
			mActiveScene = mScenes.at(mScenes.size() - 1);
		}

		UI.Init(EngineWindow.GetWindow(), mActiveScene->GetCamera(), mActiveScene->GetObjectManager(), mPhysicsWorld);
		mActiveScene->SetUIFrameBuffer(UI.GetFrameBuffer());
	}

	void cEngine::Run()
	{
		float DeltaTime = 0;
		float LastFrame = 0;
		while (!EngineWindow.ShouldClose())
		{
			float currentFrame = EngineWindow.GetTime();
			DeltaTime = currentFrame - LastFrame;
			LastFrame = currentFrame;

			EngineWindow.Update();
			
			UI.UpdateUI();
			OnEvent();
			
			Physics.Update(DeltaTime);

			mActiveScene->Render();

			UI.RenderUI();

			EngineWindow.EndFrame();
		}
	}

	void cEngine::OnEvent()
	{
		EngineWindow.PollEvents();
	}

	cEngine::~cEngine()
	{

	}

}