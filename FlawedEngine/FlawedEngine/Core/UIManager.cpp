#include "UIManager.h"


static bool opt_fullscreen = true;
static bool opt_padding = false;
static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

namespace FlawedEngine
{


	cUIManager* cUIManager::sUIInstance = nullptr;

	cUIManager::cUIManager()
	{
	}

	cUIManager::~cUIManager()
	{
		mGfxAPI->DeleteFramebuffer(mFrameBuffer);
		mGfxAPI->DeleteTexture(mTextureColorBuffer);
		mGfxAPI->BindRenderBuffer(mRenderBufferObject);
		mGfxAPI->DeleteGeometryBuffer(mGBuffer);
	}

	void cUIManager::InitFrameBuffer()
	{
		if (mFrameBuffer)
		{
			mGfxAPI->DeleteFramebuffer(mFrameBuffer);
			mGfxAPI->DeleteTexture(mTextureColorBuffer);
			mGfxAPI->BindRenderBuffer(mRenderBufferObject);
			mGfxAPI->DeleteGeometryBuffer(mGBuffer);
		}

		sTextureProps FrameBufferTextureProps(eTextureProperties::None, eTextureProperties::None,
			eTextureProperties::Linear, eTextureProperties::Linear);
		mTextureColorBuffer = mGfxAPI->CreateTexture(mViewportSize.x, mViewportSize.y, 3, NULL, FrameBufferTextureProps);

		mFrameBuffer = mGfxAPI->CreateFramebuffer(mTextureColorBuffer);
		mGfxAPI->BindFramebuffer(mFrameBuffer);


		mRenderBufferObject = mGfxAPI->CreateRenderBuffer(mViewportSize.x, mViewportSize.y);
		
		mGfxAPI->AttachRenderBufferToFrameBuffer(mRenderBufferObject);

		mGfxAPI->SetViewport(0, 0, mViewportSize.x, mViewportSize.y);

		mUIFramebuffer = sFrameBuffer(mFrameBuffer, mViewportSize, mViewportPos, mPrevViewportSize);

		mGBuffer = mGfxAPI->CreateGeometryBuffer(mViewportSize.x, mViewportSize.y);

		mGfxAPI->BindFramebuffer(0);
	}

	void cUIManager::Init(void* Window, void* PhysicsWorld, void* GfxAPI)
	{
		mWindow = Window;
		mPhysicsWorld = PhysicsWorld;
		mGfxAPI = (cGraphicsAPI*)GfxAPI;
		LoadIcons();

		InitFrameBuffer();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		//ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		SetImGuiStyle();

		io.Fonts->AddFontFromFileTTF("RobotoSlabRegular-w1GE3.ttf", 18);


		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)Window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	void cUIManager::UpdateUI()
	{

		// IK this doesn't make a lot of sense being here, but this is basically going to the wrapper around the Scene rendering and to be rendered on the other framebuffer
		// RenderUI() will reset the frame buffer back to zero so it renders on the default one as usual

		mGfxAPI->BindFramebuffer(mFrameBuffer);
		mGfxAPI->ClearColorBuffer(sColor(0.365f, 0.506f, 0.635f, 1.0f)); //This also could be found in the window class, TODO: Refactor it
		mGfxAPI->ClearDepthBuffer();
		mGfxAPI->SetViewport(0, 0, mViewportSize.x, mViewportSize.y);
		mGfxAPI->SetDepthTest(true);
	}

	void cUIManager::RenderUI()
	{
		mGfxAPI->BindFramebuffer(0);
		InitRendering();
		//ImGui::ShowStyleEditor();
		ImGui::ShowDemoWindow();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Options"))
			{

				ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
				ImGui::MenuItem("Padding", NULL, &opt_padding);
				ImGui::Separator();

				if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
				if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
				if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
				ImGui::Separator();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		{
			ImGuiStyle& style = ImGui::GetStyle();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::Begin("Settings");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Move: W,A,S,D,E,Q");
			ImGui::Text("Gizmo: R,T,G");
			ImGui::Text("Right Click SceneHierachy To Create Entities");
			ImGui::Text("Press TAB And Move Gizmo To \nCopy Selected Entity");
			ImGui::Separator();
			ImGui::Checkbox("Mouse Picking *Buggy*", &mMousePicking);
			mObjectMan.mMousePicking = mMousePicking;
			if(ImGui::Button("RenderSkyBox"))
				mObjectMan.ToggleSkyBox();
			ImGui::Separator();
			if (ImGui::Button("Switch Perspective"))
				mCamera.ToggleShadowPerspective();
			ImGui::Separator();
			DrawVec3("DirectionalLight", GetDirectionalLightPos(), 0.5f);
			ImGui::Text("FontScale");
			ImGui::SameLine();
			ImGui::SliderFloat("##FontScale", &io.FontGlobalScale, 0.3, 2.0, "%.2f");


			{
				static char Path[20] = "";
				bool Save = ImGui::Button("Save");
				ImGui::SameLine();
				ImGui::InputTextWithHint(std::string("##Save" + mSelectedEntity).c_str(), "File Name", Path, IM_ARRAYSIZE(Path));
				if (Save)
				{
					if (!((Path != NULL) && (Path[0] == '\0')))
					{
						std::string path = "Assets\\SaveFiles\\" + std::string(Path) + ".json";
						mObjectMan.Save(path);
					}
				}
			}
			ImGui::End();

			RenderViewport(); 
			RenderSceneHierarchy();
			RenderProperties();
			RenderContentBrowser();
			RenderLogger();
			SelectEntity();
 		}

		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void cUIManager::DrawVec3(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button("X", ImVec2(25, 25)))	
			values.x = resetValue;
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));
		if (ImGui::Button("Y", ImVec2(25, 25)))	
			values.y = resetValue;
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		if (ImGui::Button("Z", ImVec2(25, 25)))	
			values.z = resetValue;
		ImGui::PopStyleColor();
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopStyleVar();
		ImGui::PopID();
	}

	void cUIManager::DrawDragDropPanel(const std::string& label, const std::string& text, PanelRefreshCallback refreshCallbackFunc, DragDropCallback callbackFunc, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		{
			ImGui::SetColumnWidth(0, columnWidth);
			ImGui::Text(label.c_str());
			ImGui::NextColumn();

			ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 5, 0 });
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
				if (ImGui::ImageButton((ImTextureID)mRefreshIcon, { 20, 20 }))
				{
					refreshCallbackFunc();
				}
				ImGui::PopStyleColor();

				ImGui::PopItemWidth();
				ImGui::SameLine();

				ImGui::Text(text.c_str());

				if (ImGui::BeginDragDropTarget())
				{
					callbackFunc();
					ImGui::EndDragDropTarget();
				}

				ImGui::PopItemWidth();
			}

			ImGui::PopStyleVar();
		}
		ImGui::Columns(1);

		ImGui::PopID();
	}

	void cUIManager::InitRendering()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", nullptr, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
	}

	cUIManager& cUIManager::get()
	{
		if (sUIInstance == nullptr)
		{
			sUIInstance = new cUIManager();
		}
		return *sUIInstance;
	}

	bool cUIManager::isKeyDown(int key)
	{
		if (ImGui::IsKeyPressed((ImGuiKey)key)) {	return true; }
		return false;
	}



}