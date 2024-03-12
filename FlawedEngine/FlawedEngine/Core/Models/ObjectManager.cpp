#include "ObjectManager.h"
#include "../BasicModel/Pointlight.h"	
#include "../BasicModel/Triangle.h"
#include "../AssimpLoader/Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <nlohmann/json.hpp>

namespace FlawedEngine
{
	cObjectManager* cObjectManager::sObjectManagerInstance = nullptr;

	cObjectManager::cObjectManager()
	{
	}

	cObjectManager::~cObjectManager()
	{
		delete sObjectManagerInstance;
	}

	void cObjectManager::Init(void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes, void* CamFrustum, void* Graphics_API)
	{
		mPhysicsWorld = PhysicsWorld;
		mCollisionShapesArray = CollisionShapes;

		mCamFrustum = CamFrustum;
		mGfxAPI = Graphics_API;

		mSkybox.Setup(mGfxAPI);
		mInputFunc = std::bind(&cObjectManager::isKeyDown, this, std::placeholders::_1);
	}

	void cObjectManager::ShadowRender(sTransform& LightPerspective, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
	{
		for (auto& Object : SceneObjects)
		{
			Object.second->ShadowRender(LightPerspective, LightSpaceMatrix, DepthMap);
		}
	}

	void cObjectManager::RenderObjects(sTransform& tCamera)
	{
		this->tCamera = tCamera;
		mSkybox.RenderSkyBox(tCamera);
		uint32_t CubeMapTexture = mSkybox.GetSkyTexture();

		eraseRemoveList();

		for (auto& Object : SceneObjects)
		{
			Object.second->Update();
			Object.second->Render(tCamera, PointLights, &CubeMapTexture);
		}
	}

	void cObjectManager::AddObject(eBasicObject ObjectType, const char* Name)
	{
		switch (ObjectType)
		{
		case FlawedEngine::Cube:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Cube\\Cube.obj", Name, mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::Sphere:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Sphere\\Sphere.obj", Name, mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Sphere;
		}
		break;
		case FlawedEngine::Cone:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Cone\\Cone.obj", Name, mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cone;
		}
		break;
		case FlawedEngine::Torus:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Torus\\Torus.obj", Name, mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Torus;
		}
		break;
		case FlawedEngine::Triangle:
		{
			SceneObjects[Name] = std::make_shared<cTriangle>();
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::PointLight:
		{
			SceneObjects[Name] = std::make_shared<cPointLight>();
			sModel DefaultModel = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.5f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			glm::vec3 DefaultColor = glm::vec3(1.0f);
			SceneObjects[Name]->SetColor(DefaultColor);
			sLight DefualtLightProps = { DefaultModel.Translation, 1.0f, 0.09f, 0.032f, glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f) };
			AddLight(Name, DefualtLightProps);
			SceneObjects[Name]->Type = PointLight;
		}
		break;
		case FlawedEngine::SpotLight:
			break;
		default:
			break;
		}
	}

	void cObjectManager::LoadObject(const char* FilePath, const char* Name)
	{
		SceneObjects[Name] = std::make_shared<cModel>(FilePath, Name, mPhysicsWorld, mCollisionShapesArray, (Frustum*)mCamFrustum, mGfxAPI);
		sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
		SceneObjects[Name]->ModelTransform(DefaultModel);
		sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
		SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
		SceneObjects[Name]->Type = Custom;
	}

	void cObjectManager::RemoveObject(const char* Name)
	{
		if (SceneObjects[Name]->Type == PointLight)
			PointLights.erase(Name);
		SceneObjects.erase(Name);
	}

	void cObjectManager::ModifyObject(const char* Name, sModel& Model, sMaterial Material, bool setPhysics, sPhysicsProps PhysicsProps)
	{
		auto Object = GetObjectByName(Name);
		if (Object)
		{
			Object->ModelTransform(Model);

			Object->SetMaterial(Material);

			Object->SetPhysicsProps(PhysicsProps);

			Object->SetPhysics(Cube, mPhysicsWorld); //Get Type from Entity
			Object->setDynamic(setPhysics);
		}
	}

	glm::vec3 cObjectManager::GetLightColor(const char* Name)
	{
		auto Light = GetLightByName(Name);
		if(Light)
			return Light->ambient;
	}

	sLight* cObjectManager::GetLightProps(const char* Name)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			return Light;
	}

	void cObjectManager::ChangeLightColor(const char* Name, glm::vec3 Color)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->ambient = Color;
	}

	void cObjectManager::ChangeLightDiffuse(const char* Name, glm::vec3 Diffuse)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->diffuse = Diffuse;
	}

	void cObjectManager::ChangeLightSpecular(const char* Name, glm::vec3 Specular)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->specular = Specular;
	}

	void cObjectManager::ChangeLightPosition(const char* Name, glm::vec3 Position)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->position = Position;
	}

	void cObjectManager::ChangeName(const char* OldName, const char* NewName)
	{
		auto Type = SceneObjects.extract(OldName);
		Type.key() = NewName;
		SceneObjects.insert(std::move(Type));

		if(auto Entity = GetObjectByName(NewName))
		{
			Entity->mName = NewName;
			if (bool isLight = Entity->Type == PointLight)
			{
				auto LightType = PointLights.extract(OldName);
				LightType.key() = NewName;
				PointLights.insert(std::move(LightType));
			}
		}
	}

	void cObjectManager::ChangeLightQuadratic(const char* Name, float Quadratic)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->quadratic = Quadratic;
	}

	void cObjectManager::ChangeLightConstant(const char* Name, float Constant)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->constant = Constant;
	}

	void cObjectManager::ChangeLightLinear(const char* Name, float linear)
	{
		auto Light = GetLightByName(Name);
		if (Light)
			Light->linear = linear;
	}

	std::shared_ptr<cEntity> cObjectManager::GetObjectByName(const char* Name)
	{
		auto Object = SceneObjects.find(Name);

		if (Object == SceneObjects.end())
			return nullptr;

		if (Object->second->mDead)
			return nullptr;

		return Object->second;
	}

	void cObjectManager::addToRemoveList(const char* entityName)
	{
		mRemoveList.push_back(entityName);
	}

	void cObjectManager::eraseRemoveList()
	{
		for (auto& entity : mRemoveList)
		{
			RemoveObject(entity.c_str());
		}

		mRemoveList.clear();
	}

	void cObjectManager::Save(const std::string& FileName)
	{
		using json = nlohmann::json;

		json data;

		// serialize all the objects in the scene
		for (const auto& [name, entity] : SceneObjects)
		{
			json object;

			object["transform"]["position"] = { entity->mTransformation.Translation.x,entity->mTransformation.Translation.y, entity->mTransformation.Translation.z };
			object["transform"]["rotation"] = { entity->mTransformation.Rotation.x,entity->mTransformation.Rotation.y, entity->mTransformation.Rotation.z};
			object["transform"]["scale"] = { entity->mTransformation.Scale.x, entity->mTransformation.Scale.y, entity->mTransformation.Scale.z };
			object["color"] = { entity->GetColor()->x, entity->GetColor()->y, entity->GetColor()->z};
			object["isCostume"] = { entity->mIsCostume };
			object["type"] = { entity->Type };
			object["filepath"] = { entity->mFilePath };
			object["scripting"]["hasScript"] = { entity->mHasScripting };
			object["scripting"]["scriptPath"] = { entity->mScriptPath };
			object["physics"] = { entity->mPhysics };
			object["animation"]["hasAnimation"] = {	entity->mHasAnimation };
			object["animation"]["animationPath"] = { entity->mAnimationPath };
			data["objects"][name] = object;
		}

		std::ofstream file(FileName);
		file << data.dump(4);
	}

	void cObjectManager::LoadSave(const std::string& FileName)
	{
		using json = nlohmann::json;
		std::ifstream file(FileName);
		if (file.fail())
		{
			std::perror("Error");
			return;
		}
		if (!file.is_open())
		{
			std::cout << "{ERROR} Can't Load " << FileName << std::endl;
			return;
		}
		json data;
		file >> data;

		// iterate through the objects in the JSON data
		for (auto& [name, object] : data["objects"].items())
		{
			sModel Model = { glm::vec3(object["transform"]["position"][0], object["transform"]["position"][1], object["transform"]["position"][2]), 
				glm::vec3(object["transform"]["rotation"][0], object["transform"]["rotation"][1], object["transform"]["rotation"][2]),
				glm::vec3(object["transform"]["scale"][0], object["transform"]["scale"][1], object["transform"]["scale"][2]) };

			if (object["isCostume"][0] == false)
			{
				AddObject(object["type"][0], name.c_str());
			}
			else
			{
				std::string Filepath = object["filepath"][0];
				LoadObject(Filepath.c_str(), name.c_str());
			}

			auto Entity = GetObjectByName(name.c_str());

			Entity->ModelTransform(Model);
			Entity->SetColor(glm::vec3(object["color"][0], object["color"][1], object["color"][2]));

			if (object["type"][0] == PointLight)
			{
				ChangeLightPosition(name.c_str(), Model.Translation);
				ChangeLightColor(name.c_str(), glm::vec3(object["color"][0], object["color"][1], object["color"][2]));
			}

			if (object["physics"][0] == true)
			{
				Entity->SetPhysics(object["type"][0], GetPhysicsWorld());
				Entity->mPhysics = object["physics"][0];
				Entity->setDynamic(false);
			}

			if (object["scripting"]["hasScript"][0] == true)
			{
				std::string ScriptPath = object["scripting"]["scriptPath"][0];
				Entity->SetupScripting(ScriptPath.c_str(), mInputFunc);
			}

			if (object["animation"]["hasAnimation"][0] == true)
			{
				std::string AnimPath = object["animation"]["animationPath"][0];
				Entity->AddAnimation(AnimPath.c_str());
			}
		}
	}

	bool cObjectManager::isKeyDown(int key)
	{
		if (ImGui::IsKeyDown((ImGuiKey)key)) {	return true; }
		return false;
	}

	void cObjectManager::AddLight(const char* Name, sLight& Props)
	{
		auto Light = PointLights.find(Name);

		if (Light == PointLights.end())
		{
			PointLights[Name] = Props;
		}
	}

	sLight* cObjectManager::GetLightByName(const char* Name)
	{
		auto Light = PointLights.find(Name);

		if (Light == PointLights.end())
			return nullptr;

		return &Light->second;
	}

	cObjectManager& cObjectManager::get()
	{
		if (sObjectManagerInstance == nullptr)
		{
			sObjectManagerInstance = new cObjectManager();
		}
		return *sObjectManagerInstance;
	}
}