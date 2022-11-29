#include "UIManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

static bool opt_fullscreen = true;
static bool opt_padding = false;
static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
namespace FlawedEngine
{
	cUIManager::cUIManager()
	{
	}

	cUIManager::~cUIManager()
	{
		glDeleteFramebuffers(1, &FrameBuffer);
		glDeleteTextures(1, &TextureColorBuffer);
		glDeleteRenderbuffers(1, &RenderBufferObject);
	}

	void cUIManager::InitFrameBuffer()
	{
		if (FrameBuffer)
		{
			glDeleteFramebuffers(1, &FrameBuffer);
			glDeleteTextures(1, &TextureColorBuffer);
			glDeleteRenderbuffers(1, &RenderBufferObject);
		}
		glGenFramebuffers(1, &FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

		glGenTextures(1, &TextureColorBuffer);
		glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ViewportSize.x, ViewportSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBuffer, 0);

		glGenRenderbuffers(1, &RenderBufferObject);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferObject);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ViewportSize.x, ViewportSize.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferObject);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			assert(false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void cUIManager::Init(void* Window)
	{
		InitFrameBuffer();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGui::StyleColorsDark();

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

		//IK this doesn't make a lot of sense being here, but this is basically going to the wrapper around the Scene rendering and to be rendered on the other framebuffer
		// RenderUI() will reset the frame buffer back to zero so it renders on the default one as usual

		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glClearColor(0.365f, 0.506f, 0.635f, 1.0f); //This also could be found in the window class, TODO: Refactor it
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

	}

	void cUIManager::RenderUI()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		InitRendering();

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

		static int Cubes		= 0;
		static int Spheres		= 0;
		static int Cones		= 0;
		static int Toruses		= 0;
		static int Triangles	= 0;
		static int Lights		= 0;

		{
			ImGui::Begin("Settings");

			bool CubePressed = ImGui::Button("Add Cube");
			static char CubeName[20] = "";
			ImGui::SameLine();
			ImGui::InputText("##Cube", CubeName, IM_ARRAYSIZE(CubeName));
			if (CubePressed)
				if (!((CubeName != NULL) && (CubeName[0] == '\0'))) //if not empty
				{
					ObjectMan->AddObject(Cube, CubeName);
				}
				else
				{
					char buffer[20];
					sprintf_s(buffer, "Cube(%i)", Cubes);
					ObjectMan->AddObject(Cube, buffer);
					Cubes++;
				}

			bool SpherePressed = ImGui::Button("Add Sphere");
			static char SphereName[20] = "";
			ImGui::SameLine();
			ImGui::InputText("##Shpere", SphereName, IM_ARRAYSIZE(SphereName));
			if (SpherePressed)
				if (!((SphereName != NULL) && (SphereName[0] == '\0')))
					ObjectMan->AddObject(Sphere, SphereName);
				else
				{
					char buffer[20];
					sprintf_s(buffer, "Sphere(%i)", Spheres);
					ObjectMan->AddObject(Sphere, buffer);
					Spheres++;
				}

			bool ConePressed = ImGui::Button("Add Cone");
			static char ConeName[20] = "";
			ImGui::SameLine();
			ImGui::InputText("##Cone", ConeName, IM_ARRAYSIZE(ConeName));
			if (ConePressed)
				if (!((ConeName != NULL) && (ConeName[0] == '\0')))
					ObjectMan->AddObject(Sphere, ConeName);
				else
				{
					char buffer[20];
					sprintf_s(buffer, "Cone(%i)", Cones);
					ObjectMan->AddObject(Cone, buffer);
					Cones++;
				}

			bool TorusPressed = ImGui::Button("Add Torus");
			static char TorusName[20] = "";
			ImGui::SameLine();
			ImGui::InputText("##Torus", TorusName, IM_ARRAYSIZE(TorusName));
			if (TorusPressed)
				if (!((TorusName != NULL) && (TorusName[0] == '\0')))
					ObjectMan->AddObject(Torus, TorusName);
				else
				{
					char buffer[20];
					sprintf_s(buffer, "Torus(%i)", Toruses);
					ObjectMan->AddObject(Torus, buffer);
					Toruses++;
				}

			bool TrianglePressed = ImGui::Button("Add Triangle");
			static char TriangleName[20] = "";
			ImGui::SameLine();
			ImGui::InputText("##Triangle", TriangleName, IM_ARRAYSIZE(TriangleName));
			if (TrianglePressed)
				if (!((TriangleName != NULL) && (TriangleName[0] == '\0')))
					ObjectMan->AddObject(Triangle, TriangleName);
				else
				{
					char buffer[20];
					sprintf_s(buffer, "Triangle(%i)", Triangles);
					ObjectMan->AddObject(Triangle, buffer);
					Triangles++;
				}

			bool LightPressed = ImGui::Button("Add PointLight");
			static char LightName[20] = "";
			ImGui::SameLine();
			ImGui::InputText("##PointLight", LightName, IM_ARRAYSIZE(LightName));
			if (LightPressed)
				if (!((LightName != NULL) && (LightName[0] == '\0')))
					ObjectMan->AddObject(PointLight, LightName);
				else
				{
					char buffer[20];
					sprintf_s(buffer, "Light(%i)", Lights);
					ObjectMan->AddObject(PointLight, buffer);
					Lights++;
				}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
			ImGui::Begin("ViewPort");
			ViewportSize = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
			if (PrevViewportSize != ViewportSize)
			{
				//resize framebuffer
				InitFrameBuffer();
			}
			PrevViewportSize = ViewportSize;
			ImGui::Image((void*)TextureColorBuffer, { ViewportSize.x, ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::Begin("Scene Hierarchy");
			for (auto& Object : *ObjectMan->GetObjectsPointer())
			{
				auto Entity = Object.second;
				if (ImGui::Selectable(Object.first.c_str(), (Object.first == mSelectedEntity) ? true : false))
					mSelectedEntity = Object.first;
			}
			ImGui::End();

			ImGui::Begin("Properties");
			auto Entity = ObjectMan->GetObjectByName(mSelectedEntity.c_str());
			if(Entity)
			{
				bool ChangeName = ImGui::Button("Name");
				static char NewName[20] = "";
				ImGui::SameLine();
				ImGui::InputTextWithHint("##", "Enter Name", NewName, IM_ARRAYSIZE(NewName));
				if (ChangeName)
				{
					if (!((NewName != NULL) && (NewName[0] == '\0')))
					{
						ObjectMan->ChangeName(mSelectedEntity.c_str(), NewName);
						mSelectedEntity = NewName;
					}
				}
				if (Entity->Type == PointLight)
				{
					sModel LightModel = Entity->GetModel();
					ImGui::SliderFloat3(std::string("Translation:##" + mSelectedEntity).c_str(), &LightModel.Translation.x, -10.f, 10.f);
					Entity->ModelTransform(LightModel);
					ObjectMan->ChangeLightPosition(mSelectedEntity.c_str(), LightModel.Translation);

					glm::vec3 LightColor = ObjectMan->GetLightColor(mSelectedEntity.c_str());
					ImGui::ColorEdit3(std::string("LightColor:##" + mSelectedEntity).c_str(), &LightColor.x);
					ObjectMan->ChangeLightColor(mSelectedEntity.c_str(), LightColor);
					Entity->SetColor(LightColor);
				}
				else
				{
					if (!Entity->mPhysics)
					{
						Entity->UnSetPhysics();
						sModel EntityModel = Entity->GetModel();
						ImGui::SliderFloat3(std::string("Translation:##" + mSelectedEntity).c_str(), &EntityModel.Translation.x, -10.f, 10.f);
						ImGui::SliderFloat3(std::string("Rotation:##" + mSelectedEntity).c_str(), &EntityModel.Rotation.x, -10.f, 10.f);
						ImGui::SliderFloat3(std::string("Scale:##" + mSelectedEntity).c_str(), &EntityModel.Scale.x, -10.f, 10.f);
						Entity->ModelTransform(EntityModel);
					}
					else
					{
						btTransform Trans;
						Entity->mRidigBody->getMotionState()->getWorldTransform(Trans);

						//Translation......
						btVector3 Origin = Trans.getOrigin();
						glm::vec3 Translation = glm::vec3(Origin.x(), Origin.y(), Origin.z());

						ImGui::SliderFloat3(std::string("Translation:##" + mSelectedEntity).c_str(), &Translation.x, -10.f, 10.f);
						btVector3 FinalTranslation(Translation.x, Translation.y, Translation.z);
						Trans.setOrigin(FinalTranslation);

						//Rotation........
						static btScalar roll, pitch, yaw;
						Entity->mRidigBody->getCenterOfMassTransform().getBasis().getEulerYPR(yaw, pitch, roll);

						glm::vec3 Rotation = glm::vec3(roll, pitch, yaw);
						ImGui::SliderFloat3(std::string("Rotation:##" + mSelectedEntity).c_str(), &Rotation.x, -0.5f, 0.5f);
						Trans.setRotation(btQuaternion(Rotation.z, Rotation.y, Rotation.x));
						Entity->mRidigBody->getMotionState()->setWorldTransform(Trans);

						//Scale..........
						btVector3 myscale = Entity->mRidigBody->getCollisionShape()->getLocalScaling();
						glm::vec3 scale(myscale.x(), myscale.y(), myscale.z());
						ImGui::SliderFloat3(std::string("Scale:##" + mSelectedEntity).c_str(), &scale.x, -10.f, 10.f);
						myscale = btVector3(scale.x, scale.y, scale.z);
						Entity->mRidigBody->getCollisionShape()->setLocalScaling(myscale);

						ImGui::Text("Activation State: %i", Entity->GetActivationState());

						if (!Entity->mDynamic)
						{
							Entity->mRidigBody->setWorldTransform(Trans);
						}
					}

					glm::vec3* EntityColor = Entity->GetColor();
					ImGui::ColorEdit3(std::string("Color:##" + mSelectedEntity).c_str(), &EntityColor->x);

					ImGui::Checkbox(std::string("Physics:##" + mSelectedEntity).c_str(), &Entity->mPhysics);

					if (Entity->mPhysics)
					{
						ImGui::Checkbox(std::string("Dynamic:##" + mSelectedEntity).c_str(), &Entity->mDynamic);
						Entity->SetPhysics(Entity->Type, ObjectMan->GetPhysicsWorld());
						Entity->setDynamic(Entity->mDynamic);
					}
					else
						Entity->mDynamic = false;
				}

				if (ImGui::Button("Remove"))
				{
					ObjectMan->RemoveObject(mSelectedEntity.c_str());
				}
			}
			ImGui::End();

			
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

	void cUIManager::InitRendering()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

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
}