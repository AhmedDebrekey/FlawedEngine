#include "Scene.h"

#include "BasicModel/Triangle.h"
#include "BasicModel/Cube.h"
#include "BasicModel/PointLight.h"

#include "Models/OBJModel.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

namespace FlawedEngine
{
	cScene::cScene(void* Window)
		:mWindow(Window), Camera(mWindow)
	{
		//Should have more stuff in the future probably, IDK little secret, I am just winging it, I don't really know what I am doing.

		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration); 
		dynamicsWorld->setGravity(btVector3(0.0, -0.9, 0));

		Setup();
	}

	cScene::~cScene()
	{
		for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

		for (int j = 0; j < collisionShapes.size(); j++)
		{
			btCollisionShape* shape = collisionShapes[j];
			collisionShapes[j] = 0;
			delete shape;
		}

		delete dynamicsWorld;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;
		collisionShapes.clear();
	}

	void cScene::Setup()
	{
		myPhysics->Setup();

		LoadModel(Triangle,		"Triangle",		dynamicsWorld);
		LoadModel(PointLight,	"Light",		dynamicsWorld);
		LoadModel(PointLight,	"Example",		dynamicsWorld);
		LoadModel(PointLight,	"AnotherOne",	dynamicsWorld);
		LoadModel(Sphere,		"Sphere",		dynamicsWorld);
		LoadModel(Cube,			"Ground",		dynamicsWorld);
		LoadModel(Cube,			"Ground2",		dynamicsWorld);
		LoadModel(Cube,			"PhysicsCube",	dynamicsWorld);
		LoadModel(Cone,			"Cone",			dynamicsWorld);
		LoadModel(Torus,		"Torus",		dynamicsWorld);

		glGenFramebuffers(1, &FrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

		glGenTextures(1, &TextureColorBuffer);
		glBindTexture(GL_TEXTURE_2D, TextureColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureColorBuffer, 0);

		glGenRenderbuffers(1, &RenderBufferObject);
		glBindRenderbuffer(GL_RENDERBUFFER, RenderBufferObject);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600, 900);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RenderBufferObject);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			assert(false);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)mWindow, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	static bool isPlayPressed = false; //hehe idc hahah :(

	void cScene::Render()
	{
		myPhysics->Update();

		Camera.Compute();
		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};		

		if (glfwGetKey((GLFWwindow*)mWindow, GLFW_KEY_P) == GLFW_PRESS) isPlayPressed = !isPlayPressed; 
		if (isPlayPressed)	dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		std::shared_ptr<cEntity> Triangle = GetObjectByName("Triangle");
		if (Triangle)
		{
			sModel TriangleModel = { glm::vec3(3.0f, 5.0f, 0.0f), glm::vec3(45.0f), glm::vec3(5.0f) };
			Triangle->ModelTransform(TriangleModel);

			Triangle->SetColor(glm::vec3( 0.3f, 0.6f, 0.1f ));
		}

		auto Sphere = GetObjectByName("Sphere");
		if (Sphere)
		{
			sModel SphereModel = { glm::vec3(-15.0f, 30.0f, -10.0f), glm::vec3(0.0f), glm::vec3(2.0f) };
			Sphere->ModelTransform(SphereModel);

			Sphere->SetColor(glm::vec3(0.5f, 0.2f, 0.5f));
			
			sPhysicsProps PhysProps = { 1.f, 1.0f, 10.0f, SphereModel };
			Sphere->SetPhysicsProps(PhysProps);

			Sphere->SetPhysics(eBasicObject::Sphere, myPhysics.get());
			Sphere->setDynamic(true);


			if (glfwGetKey((GLFWwindow*)mWindow, GLFW_KEY_N) == GLFW_PRESS)
			{
				Sphere->ApplyForce(glm::vec3(0, 5, 0));
			}
		}

		auto Torus = GetObjectByName("Torus");
		if (Torus)
		{
			sModel TorusModel = { glm::vec3(10.0f, 5.0f, -15.0f), glm::vec3(45.0f, 0.0f, 0.0f), glm::vec3(5.0f) };
			Torus->ModelTransform(TorusModel);

			Torus->SetColor(glm::vec3(1.0f, 0.5f, 1.0f));
		}

		auto Cone = GetObjectByName("Cone");
		if (Cone)
		{
			sModel ConeModel = { glm::vec3(-15.0f , 20.0f, 4.0f ), glm::vec3(0.0f), glm::vec3(3.0f) };
			Cone->ModelTransform(ConeModel);

			sMaterial ConeMaterial = { glm::vec3(0.05f, 0.05f, 0.4f), glm::vec3(1.0f, 0.5f, 0.31f), glm::vec3(0.5f, 0.5f, 0.5f), 32.0f };
			Cone->SetMaterial(ConeMaterial);

			sPhysicsProps PhysProps = { 10.f, 1.0f, 0.5 };
			Cone->SetPhysicsProps(PhysProps);

			Cone->SetPhysics(eBasicObject::Cube, myPhysics.get());
			Cone->setDynamic(true);

			if (glfwGetKey((GLFWwindow*)mWindow, GLFW_KEY_N) == GLFW_PRESS) //in need of better input system :)
			{
				Cone->ApplyForce(glm::vec3(0, 5, 0));
			}
		}
			
		auto Light = GetObjectByName("Light");
		if (Light)
		{
			sModel LightModel = { glm::vec3(-18.0f, 6.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			Light->ModelTransform(LightModel);

			Light->SetColor(glm::vec3(0.5f, 0.3f, 0.9f));

			sLight LightProps = { LightModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f)};
			AddLight("Light", LightProps);
		}

		auto ExampleLight = GetObjectByName("Example");
		if(ExampleLight) 
		{
			sModel LightModel = { glm::vec3(15.0f, 3.0f, 2.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			ExampleLight->ModelTransform(LightModel);

			ExampleLight->SetColor(glm::vec3(0.6f, 0.5f, 0.3f));

			sLight LightProps = { LightModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(0.3f, 0.2f, 0.7f), glm::vec3(0.6f, 0.3f, 0.9f), glm::vec3(0.5f, 0.2f, 0.9f) };
			AddLight("Example", LightProps);
		}

		auto SomeLight = GetObjectByName("AnotherOne");
		if (SomeLight)
		{
			sModel LightModel = { glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			SomeLight->ModelTransform(LightModel);

			sLight LightProps = { LightModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(0.5f, 0.1f, 0.8f), glm::vec3(0.8f), glm::vec3(1.0f) };
			AddLight("AnotherOne", LightProps);
		}

		auto PhysicCube = GetObjectByName("PhysicsCube");
		if(PhysicCube)
		{
			sModel CubeModel = { glm::vec3(0.0f, 20.0f, 0.0f), glm::vec3(10.5f), glm::vec3(2.0f)};
			PhysicCube->ModelTransform(CubeModel);
			
			sMaterial CubeMat = {glm::vec3(1,1,1), glm::vec3(1.0f), glm::vec3(1.0f), 32.0f};
			PhysicCube->SetMaterial(CubeMat);
			
			sPhysicsProps PhysProps = { 1.f, 10.0f, 0.0f };
			PhysicCube->SetPhysicsProps(PhysProps);

			PhysicCube->SetPhysics(Cube, myPhysics.get());
			PhysicCube->setDynamic(true);

		}

		auto Ground = GetObjectByName("Ground");
		if (Ground)
		{
			sModel GroundModel = { glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(30.0f, 0.1f, 30.0f) };
			Ground->ModelTransform(GroundModel);

			sMaterial GroundMat = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), 16.0f };
			Ground->SetMaterial(GroundMat);

			Ground->SetPhysics(Cube, myPhysics.get());
			Ground->setDynamic(false);
		}

		auto otherGround = GetObjectByName("Ground2");
		if (Ground)
		{
			sModel GroundModel = { glm::vec3(-60.0f, -20.0f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(30.0f, 0.1f, 30.0f) };
			otherGround->ModelTransform(GroundModel);

			sMaterial GroundMat = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), 16.0f };
			otherGround->SetMaterial(GroundMat);

			otherGround->SetPhysics(Cube, myPhysics.get());
			otherGround->setDynamic(false);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
		glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
		glEnable(GL_DEPTH_TEST);
		//Render Models
		for (auto &Entities : WorldEntities)
		{
			Entities.second->Render(tCamera, PointLights); 
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;


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
		ImGui::Begin("DockSpace Demo", nullptr, window_flags);
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
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Settings");   

			if (ImGui::Button("Button"))
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();

			ImGui::Begin("ViewPort");
			ImGui::Image((void*)TextureColorBuffer, ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), ImVec2(0, 1), ImVec2(1, 0));
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

	void cScene::LoadModel(const char* FilePath, const char* Name, void* PhysicsWorld)
	{
		WorldEntities[Name] = std::make_shared<cOBJModel>(FilePath, dynamicsWorld);
	}

	void cScene::LoadModel(eBasicObject Object, const char* Name, void* PhysicsWorld)
	{
		switch (Object)
		{
		case FlawedEngine::Cube:
			{
				LoadModel("Core\\Models\\OBJ\\Cube\\Cube.obj", Name, dynamicsWorld); // should be a class but i cant bother
			}
			break;
		case FlawedEngine::Sphere:
			{
				LoadModel("Core\\Models\\OBJ\\Sphere\\Sphere.obj", Name, dynamicsWorld);
			}
			break;
		case FlawedEngine::Cone:
			{
				LoadModel("Core\\Models\\OBJ\\Cone\\Cone.obj", Name, dynamicsWorld);
			}
			break;
		case FlawedEngine::Torus:
			{
				LoadModel("Core\\Models\\OBJ\\Torus\\Torus.obj", Name, dynamicsWorld);
			}
			break;
		case FlawedEngine::Triangle:
			{
				WorldEntities[Name] = std::make_shared<cTriangle>();
			}
			break;
		case FlawedEngine::PointLight:
			{
				WorldEntities[Name] = std::make_shared<cPointLight>();
			}
			break;
		case FlawedEngine::SpotLight:
			break;
		default:
			break;
		}
	}

	std::shared_ptr<cEntity> cScene::GetObjectByName(const char* Name)
	{
		auto Entity = WorldEntities.find(Name);

		if (Entity == WorldEntities.end())
			return nullptr;

		return Entity->second;
	}

	void cScene::AddLight(const char* Name, sLight& Props)
	{
		auto Light = PointLights.find(Name);

		if (Light == PointLights.end())
		{
			PointLights[Name] = Props;
		}
	}
}