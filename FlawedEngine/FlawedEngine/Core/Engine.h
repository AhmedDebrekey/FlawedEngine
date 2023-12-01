#pragma once

#include <memory>
#include "Scene.h"
#include "Window.h"
#include "UIManager.h"
#include "Physics/Physics.h"
#include "Graphics/GraphicsAPI.h"
#include "Graphics/OpenGLAPI.h"

namespace FlawedEngine
{
	class cEngine
	{
	public:

		cEngine();
		~cEngine();

		void Run();
	
	private: 

		void OnEvent();

	private:
		cWindow& mEngineWindow = cWindow::get();
		cUIManager& mUI = cUIManager::get();
		cPhysics mPhysics;//make into singleton

		std::vector<std::shared_ptr<cScene>> mScenes;
		std::shared_ptr<cScene> mActiveScene;
		std::shared_ptr<cGraphicsAPI> mGfxAPI;
		void* mPhysicsWorld = nullptr;
	};
}