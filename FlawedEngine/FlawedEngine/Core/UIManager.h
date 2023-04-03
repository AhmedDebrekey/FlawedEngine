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
#include <glm/gtx/string_cast.hpp>
#include <string>
#include "PerspectiveCamera.h"

namespace FlawedEngine
{
	class cUIManager
	{
	public:
		void Init(void* Window, void* Camera, void* Manager);
		void UpdateUI();
		void RenderUI();
		std::string& GetSelectedEntity() { return mSelectedEntity; }
		sFrameBuffer& GetFrameBuffer() { return mUIFramebuffer; }
		glm::vec2 GetViewportSize() { return ViewportSize; };
		glm::vec2 GetViewportPos() { return ViewportPos; };
	private:
		void InitRendering();
		void InitFrameBuffer();
		void DrawVec3(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

		bool isKeyDown(int key);


	private:
		uint32_t FrameBuffer;
		uint32_t TextureColorBuffer;
		uint32_t RenderBufferObject;
		sFrameBuffer mUIFramebuffer;
		glm::vec2 PrevViewportSize = {1600, 900};
		glm::vec2 ViewportSize = {1600, 900};
		glm::vec2 ViewportPos = { 1600, 900 };
		void* mWindow = nullptr;
		glm::vec2 m_ViewportBounds[2];
		std::string mSelectedEntity;
		int mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
		bool mMousePicking = false;
	private:
		cObjectManager* ObjectMan;
		cpCamera* mCamera;
	private:
		void RenderGizmo();
		void RenderViewport();
		void RenderSceneHierarchy();
		void RenderProperties();

		float mTmpMatrix[16];
	public:
		static cUIManager& get();
		~cUIManager();

		cUIManager(cUIManager const&) = delete;
		void operator=(cUIManager const&) = delete;

	private:
		static cUIManager* sUIInstance;

		cUIManager();
		cUIManager(cUIManager&);

	};
}
