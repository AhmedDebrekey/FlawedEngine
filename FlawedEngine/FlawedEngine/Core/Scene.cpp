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

		//Render Models
		for (auto &Entities : WorldEntities)
		{
			Entities.second->Render(tCamera, PointLights); 
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