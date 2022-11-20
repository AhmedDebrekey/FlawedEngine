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
	cScene::cScene(void* Window, void* Physics)
		:mWindow(Window), Camera(mWindow), PhysicsWorld(Physics)
	{
		Setup();
	}

	cScene::~cScene()
	{
	}

	void cScene::Setup()
	{
		LoadModel(Triangle,		"Triangle",		PhysicsWorld);
		LoadModel(PointLight,	"Light",		PhysicsWorld);
		LoadModel(PointLight,	"Example",		PhysicsWorld);
		LoadModel(PointLight,	"AnotherOne",	PhysicsWorld);
		LoadModel(Sphere,		"Sphere",		PhysicsWorld);
		LoadModel(Cube,			"Ground",		PhysicsWorld);
		LoadModel(Cube,			"Ground2",		PhysicsWorld);
		LoadModel(Cube,			"PhysicsCube",	PhysicsWorld);
		LoadModel(Cone,			"Cone",			PhysicsWorld);
		LoadModel(Torus,		"Torus",		PhysicsWorld);
	}

	void cScene::Render()
	{
		Camera.Compute();
		sTransform tCamera { Camera.Postion() , Camera.Front(), Camera.Projection(), Camera.View()};		

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

			Sphere->SetPhysics(eBasicObject::Sphere, PhysicsWorld);
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

			Cone->SetPhysics(eBasicObject::Cube, PhysicsWorld);
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

			PhysicCube->SetPhysics(Cube, PhysicsWorld);
			PhysicCube->setDynamic(true);

		}

		auto Ground = GetObjectByName("Ground");
		if (Ground)
		{
			sModel GroundModel = { glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(30.0f, 0.1f, 30.0f) };
			Ground->ModelTransform(GroundModel);

			sMaterial GroundMat = { glm::vec3(0.0f, 0.2f, 0.8f), glm::vec3(1.0f), glm::vec3(1.0f), 16.0f };
			Ground->SetMaterial(GroundMat);

			Ground->SetPhysics(Cube, PhysicsWorld);
			Ground->setDynamic(false);
		}

		auto otherGround = GetObjectByName("Ground2");
		if (Ground)
		{
			sModel GroundModel = { glm::vec3(-60.0f, -20.0f, 0.0f), glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(30.0f, 0.1f, 30.0f) };
			otherGround->ModelTransform(GroundModel);

			sMaterial GroundMat = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), 16.0f };
			otherGround->SetMaterial(GroundMat);

			otherGround->SetPhysics(Cube, PhysicsWorld);
			otherGround->setDynamic(false);
		}

		//Render Models
		for (auto &Entities : WorldEntities)
		{
			Entities.second->Render(tCamera, PointLights); 
		}
	}

	void cScene::LoadModel(const char* FilePath, const char* Name, void* PhysicsWorld)
	{
		WorldEntities[Name] = std::make_shared<cOBJModel>(FilePath, PhysicsWorld);
	}

	void cScene::LoadModel(eBasicObject Object, const char* Name, void* PhysicsWorld)
	{
		switch (Object)
		{
		case FlawedEngine::Cube:
			{
				LoadModel("Core\\Models\\OBJ\\Cube\\Cube.obj", Name, PhysicsWorld); // should be a class but i cant bother
			}
			break;
		case FlawedEngine::Sphere:
			{
				LoadModel("Core\\Models\\OBJ\\Sphere\\Sphere.obj", Name, PhysicsWorld);
			}
			break;
		case FlawedEngine::Cone:
			{
				LoadModel("Core\\Models\\OBJ\\Cone\\Cone.obj", Name, PhysicsWorld);
			}
			break;
		case FlawedEngine::Torus:
			{
				LoadModel("Core\\Models\\OBJ\\Torus\\Torus.obj", Name, PhysicsWorld);
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