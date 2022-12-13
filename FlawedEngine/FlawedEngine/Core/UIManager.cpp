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

		glViewport(0, 0, ViewportSize.x, ViewportSize.y);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			assert(false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void cUIManager::Init(void* Window, void* Camera)
	{
		mCamera = (cpCamera*)Camera;
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
		glViewport(0, 0, ViewportSize.x, ViewportSize.y);
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

		{
			ImGui::Begin("Settings");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::Text("Move: W,A,S,D,E,Q");
			ImGui::Text("Gizmo: R,T,G");
			ImGui::Text("Right Click SceneHierachy To Create Entitys");
			ImGui::End();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
			ImGui::Begin("ViewPort", 0, ImGuiWindowFlags_NoScrollbar);
			ViewportSize = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
			if (PrevViewportSize != ViewportSize)
			{
				//resize framebuffer
				InitFrameBuffer();
				mCamera->UpdateProjection(glm::perspective(glm::radians(mCamera->FoV()), ViewportSize.x / ViewportSize.y, 0.1f, 100.0f));
			
			}
			PrevViewportSize = ViewportSize;
			ImGui::Image((void*)TextureColorBuffer, { ViewportSize.x, ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

			//Gizmo
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T)) { mGizmoType = ImGuizmo::OPERATION::TRANSLATE; }
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R)) { mGizmoType = ImGuizmo::OPERATION::ROTATE; }
			if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_G)) { mGizmoType = ImGuizmo::OPERATION::SCALE; }

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();

			auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
			auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			auto viewportOffset = ImGui::GetWindowPos();
			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			{
				auto Entity = ObjectMan->GetObjectByName(mSelectedEntity.c_str());
				if (Entity)
				{

					float tmpMatrix[16];
					sModel Model = Entity->GetModel();
					ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(Model.Translation), glm::value_ptr(Model.Rotation), glm::value_ptr(Model.Scale), tmpMatrix);

					glm::mat4* Transform = Entity->GetModelMatrix();
					ImGuizmo::Manipulate(glm::value_ptr(mCamera->View()), glm::value_ptr(mCamera->Projection()),
						(ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL, tmpMatrix);

					if (ImGuizmo::IsUsing())
					{
						glm::vec3 translation, rotation, scale;
						ImGuizmo::DecomposeMatrixToComponents(tmpMatrix, glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

						Model.Translation = translation;
						Model.Rotation = rotation;
						Model.Scale = scale;
						Entity->ModelTransform(Model);
					}
				}
			}
			ImGui::End();
			ImGui::PopStyleVar();

			ImGui::Begin("Scene Hierarchy");

			static int Cubes = 0;
			static int Spheres = 0;
			static int Cones = 0;
			static int Toruses = 0;
			static int Triangles = 0;
			static int Lights = 0;

			for (auto& Object : *ObjectMan->GetObjectsPointer())
			{
				auto Entity = Object.second;
				if (ImGui::Selectable(Object.first.c_str(), (Object.first == mSelectedEntity) ? true : false))
					mSelectedEntity = Object.first;
			}
			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				mSelectedEntity = "";

			if (ImGui::BeginPopupContextWindow("Add Entity", 1))
			{
				if (ImGui::MenuItem("Point Light"))
				{
					char buffer[20];
					sprintf_s(buffer, "Light(%i)", Lights);
					ObjectMan->AddObject(PointLight, buffer);
					mSelectedEntity = buffer;
					Lights++;
				}

				if (ImGui::MenuItem("Cube"))
				{
					char buffer[20];
					sprintf_s(buffer, "Cube(%i)", Cubes);
					ObjectMan->AddObject(Cube, buffer);
					mSelectedEntity = buffer;
					Cubes++;
				}

				if (ImGui::MenuItem("Sphere"))
				{
					char buffer[20];
					sprintf_s(buffer, "Sphere(%i)", Spheres);
					ObjectMan->AddObject(Sphere, buffer);
					mSelectedEntity = buffer;
					Spheres++;
				}

				if (ImGui::MenuItem("Cone"))
				{
					char buffer[20];
					sprintf_s(buffer, "Cone(%i)", Cones);
					ObjectMan->AddObject(Cone, buffer);
					mSelectedEntity = buffer;
					Cones++;
				}

				if (ImGui::MenuItem("Torus"))
				{
					char buffer[20];
					sprintf_s(buffer, "Torus(%i)", Toruses);
					ObjectMan->AddObject(Torus, buffer);
					mSelectedEntity = buffer;
					Toruses++;
				} //Left out the Trianlge for no reason, It was just a proof of concept.
				// later could be used as a billbord, (rectangles ofc)

				ImGui::EndPopup();
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
					DrawVec3("Translation", LightModel.Translation, 0.0f);
					Entity->ModelTransform(LightModel);
					ObjectMan->ChangeLightPosition(mSelectedEntity.c_str(), LightModel.Translation);

					sLight* LightProps = nullptr;
					LightProps = ObjectMan->GetLightProps(mSelectedEntity.c_str());

					ImGui::ColorEdit3(std::string("LightColor:##" + mSelectedEntity).c_str(), &LightProps->ambient.x);
					Entity->SetColor(LightProps->ambient);

					ImGui::ColorEdit3(std::string("LightDiffuse:##" + mSelectedEntity).c_str(), &LightProps->diffuse.x);
					ImGui::ColorEdit3(std::string("LightSpecular:##" + mSelectedEntity).c_str(), &LightProps->specular.x);
					ImGui::DragFloat(std::string("LightConstant:##" + mSelectedEntity).c_str(), &LightProps->constant, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat(std::string("LightLinear:##" + mSelectedEntity).c_str(), &LightProps->linear, 0.01f, 0.0f, 1.0f);
					ImGui::DragFloat(std::string("LightQuadratic:##" + mSelectedEntity).c_str(), &LightProps->quadratic, 0.01f, 0.0f, 1.0f);
				}
				else
				{
					if (!Entity->mPhysics)
					{
						Entity->UnSetPhysics();
						sModel EntityModel = Entity->GetModel();
						DrawVec3("Translation", EntityModel.Translation, 0.0f);
						DrawVec3("Rotation", EntityModel.Rotation, 0.0f);
						DrawVec3("Scale", EntityModel.Scale, 1.0f);
						Entity->ModelTransform(EntityModel);
					}
					else
					{
						btTransform Trans;
						Entity->mRidigBody->getMotionState()->getWorldTransform(Trans);

						//Translation......
						btVector3 Origin = Trans.getOrigin();
						glm::vec3 Translation = glm::vec3(Origin.x(), Origin.y(), Origin.z());
						DrawVec3("Translation", Translation);
						btVector3 FinalTranslation(Translation.x, Translation.y, Translation.z);
						Trans.setOrigin(FinalTranslation);

						//Rotation........
						static btScalar roll, pitch, yaw;
						Entity->mRidigBody->getCenterOfMassTransform().getBasis().getEulerYPR(yaw, pitch, roll);

						glm::vec3 Rotation = glm::vec3(roll, pitch, yaw);
						DrawVec3("Rotation", Rotation);
						Trans.setRotation(btQuaternion(Rotation.z, Rotation.y, Rotation.x));
						Entity->mRidigBody->getMotionState()->setWorldTransform(Trans);

						//Scale..........
						btVector3 myscale = Entity->mRidigBody->getCollisionShape()->getLocalScaling();
						glm::vec3 scale(myscale.x(), myscale.y(), myscale.z());
						DrawVec3("Scale", scale, 1.0f);
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

	void cUIManager::DrawVec3(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		if (ImGui::Button("X", ImVec2(25, 25)))	
			values.x = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Y", ImVec2(25, 25)))	
			values.y = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Z", ImVec2(25, 25)))	
			values.z = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::Columns(1);
		ImGui::PopStyleVar();
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
}