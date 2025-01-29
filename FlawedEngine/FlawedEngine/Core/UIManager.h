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
#include <deque>
#include <filesystem>

#include "PerspectiveCamera.h"
#include "Graphics/GraphicsAPI.h"

namespace FlawedEngine
{
	class cUIManager
	{
	public:
		void Init(void* Window, void* PhysicsWorld, void* GfxAPI);
		void UpdateUI();
		void RenderUI();
		sFrameBuffer& GetFrameBuffer() { return mUIFramebuffer; }
		sGBufferObjects& GetGeometryBuffer() { return mGBuffer; }

	private:
		void InitRendering();
		void InitFrameBuffer();
		void DrawVec3(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
		void DrawDragDropPanel(const std::string& label, const std::string& text, PanelRefreshCallback refreshCallbackFunc, DragDropCallback callbackFunc, float columnWidth = 100.0f);
		bool isKeyDown(int key);
		void SetImGuiStyle();

	private:

		sGBufferObjects mGBuffer;
		uint32_t mFrameBuffer;
		uint32_t mTextureColorBuffer;
		uint32_t mRenderBufferObject;
		sFrameBuffer mUIFramebuffer;
		glm::vec2 mPrevViewportSize = {1600, 900};
		glm::vec2 mViewportSize = {1600, 900};
		glm::vec2 mViewportPos = { 1600, 900 };
		void* mWindow = nullptr;
		glm::vec2 m_ViewportBounds[2];
		std::string mSelectedEntity;
		int mGizmoType = ImGuizmo::OPERATION::TRANSLATE;
		bool mMousePicking = false;

		std::filesystem::path mBaseDir;
		std::filesystem::path mCurrentDir;
		uint32_t mDirIcon;
		uint32_t mFileIcon;
		uint32_t mBackIcon;
		uint32_t mRefreshIcon;
		uint32_t mImageIcon;
		uint32_t mModelIcon;
		uint32_t mLuaIcon;
		uint32_t mSaveIcon;
		uint32_t mTrashIcon;
		std::vector<std::filesystem::directory_entry> mDirectories;

		void LoadIcons();
		void LoadTexture(const std::string& path, uint32_t& TextureID, const sTextureProps& props);
		void GetFilesandFolders(const std::filesystem::path& path);
		uint32_t GetFileIcon(const std::filesystem::path& path);
		
		cGraphicsAPI* mGfxAPI = nullptr;
		

	private:
		cObjectManager& mObjectMan = cObjectManager::get();
		cpCamera& mCamera = cpCamera::get();
		void* mPhysicsWorld = nullptr;

	private:
		void RenderGizmo();
		void RenderViewport();
		void RenderSceneHierarchy();
		void RenderProperties();
		void RenderContentBrowser();
		void SelectEntity();

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
