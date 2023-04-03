#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
		
		mPhysicsWorld = Physics.Init();

		{
			mScenes.push_back(std::make_shared<cScene>(EngineWindow.GetWindow(), mPhysicsWorld, Physics.GetCollisionShapesArray()));
			mActiveScene = mScenes.at(mScenes.size() - 1);
		}

		UI.Init(EngineWindow.GetWindow(), mActiveScene->GetCamera(), mActiveScene->GetObjectManager());
		mActiveScene->SetSelectedEntity(UI.GetSelectedEntity());
		mActiveScene->SetUIFrameBuffer(UI.GetFrameBuffer());
	}

	void cEngine::Run()
	{
		float DeltaTime = 0;
		float LastFrame = 0;
		while (!EngineWindow.ShouldClose())
		{
			float currentFrame = glfwGetTime();
			DeltaTime = currentFrame - LastFrame;
			LastFrame = currentFrame;

			EngineWindow.Update();
			
			UI.UpdateUI();
			mActiveScene->UpdateViewport(UI.GetViewportSize(), UI.GetViewportPos());
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
		//To be continued
	}

	cEngine::~cEngine()
	{

	}

}