#pragma once

#include "Core.h"
#include "Entity.h"
#include "Models/ObjectManager.h"
#include <memory>
#include <unordered_map>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <string>
#include "PerspectiveCamera.h"
namespace FlawedEngine
{
	class cUIManager
	{
	public:
		cUIManager();
		~cUIManager();
		void Init(void* Window, void* Camera);
		void UpdateUI();
		void RenderUI();
		void SetObjectManager(cObjectManager* Manager) { ObjectMan = Manager; };
	private:
		void InitRendering();
		void InitFrameBuffer();
		void DrawVec3(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	private:
		uint32_t FrameBuffer;
		uint32_t TextureColorBuffer;
		uint32_t RenderBufferObject;
		glm::vec2 PrevViewportSize = {1600, 900};
		glm::vec2 ViewportSize = {1600, 900};
		glm::vec2 m_ViewportBounds[2];
		std::string mSelectedEntity;
		int mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
	private:
		cObjectManager* ObjectMan;
		cpCamera* mCamera;
	};
}
