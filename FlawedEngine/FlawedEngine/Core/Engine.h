#pragma once

#include <memory>
#include "Window.h"

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
	};
}