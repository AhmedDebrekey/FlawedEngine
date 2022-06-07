#include "Engine.h"

namespace FlawedEngine
{
	cEngine::cEngine()
	{
		EngineWindow.Init();
		EngineWindow.CreateWindow();
	}

	void cEngine::Run()
	{
		while (!EngineWindow.ShouldClose())
		{
			EngineWindow.Update();
			
			//Physics update
			//Audio update
			//UI update
			//World update

			OnEvent();
		}
	}

	void cEngine::OnEvent()
	{
		//add implementation
	}
}