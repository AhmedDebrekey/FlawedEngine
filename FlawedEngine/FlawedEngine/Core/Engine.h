#pragma once

#include <memory>
#include "Window.h"
#include "Scene.h"

namespace FlawedEngine
{
	class cEngine
	{
	public:

		cEngine();
		void Run();
	
	private: 

		void OnEvent();

	private:
		FlawedEngine::cWindow& EngineWindow = FlawedEngine::cWindow::get();
		std::vector<std::shared_ptr<cScene>> Scenes;
	};
}