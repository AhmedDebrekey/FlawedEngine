#pragma once

#include "Core.h"
#include "Entity.h"
#include "Models/ObjectManager.h"
#include <memory>
#include <unordered_map>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

namespace FlawedEngine
{
	class cUIManager
	{
	public:
		cUIManager();
		~cUIManager();
		void Init(void* Window);
		void UpdateUI();
		void RenderUI();
		void SetObjectManager(cObjectManager* Manager) { ObjectMan = Manager; };
	private:
		void InitRendering();
		void InitFrameBuffer();

	private:
		uint32_t FrameBuffer;
		uint32_t TextureColorBuffer;
		uint32_t RenderBufferObject;
		glm::vec2 PrevViewportSize = {1600, 900};
		glm::vec2 ViewportSize = {1600, 900};
	private:
		cObjectManager* ObjectMan;
	};
}
