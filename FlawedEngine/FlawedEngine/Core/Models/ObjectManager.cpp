#include "ObjectManager.h"
#include "../BasicModel/Pointlight.h"	
#include "../BasicModel/Triangle.h"	

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


namespace FlawedEngine
{
	cObjectManager* cObjectManager::sObjectManagerInstance = nullptr;

	cObjectManager::cObjectManager()
	{
	}

	cObjectManager::~cObjectManager()
	{
	}

	void cObjectManager::Init(void* PhysicsWorld, btAlignedObjectArray<btCollisionShape*>* CollisionShapes)
	{
		mPhysicsWorld = PhysicsWorld;
		mCollisionShapesArray = CollisionShapes;
		SetupSkybox();
	}

	void cObjectManager::RenderObjects(sTransform& tCamera)
	{
		this->tCamera = tCamera;
		RenderSkyBox();

		for (auto& Object : SceneObjects)
		{
			Object.second->Update();
			Object.second->Render(tCamera, PointLights, &mCubeMapTexture);
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
			SceneObjects[Name]->Type = Cube;
		}
		break;
		case FlawedEngine::Torus:
		{
			SceneObjects[Name] = std::make_shared<cOBJModel>("Core\\Models\\OBJ\\Torus\\Torus.obj", Name, mPhysicsWorld, mCollisionShapesArray);
			sModel DefaultModel = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) };
			SceneObjects[Name]->ModelTransform(DefaultModel);
			sPhysicsProps DefaultPhysics = { 1.f, 1.0f, 0.5f };
			SceneObjects[Name]->SetPhysicsProps(DefaultPhysics);
			SceneObjects[Name]->Type = Cube;
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
			if (bool isLight = Entity->Type == PointLight)
			{
				auto LightType = PointLights.extract(OldName);
				LightType.key() = NewName;
				PointLights.insert(std::move(LightType));
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

		if (Object->second->Dead)
			return nullptr;

		return Object->second;
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

	void cObjectManager::SetupSkybox()
	{
		std::vector<std::string> Faces
		{
			"Core\\Models\\skybox\\skybox\\OldHall\\right.png",
			"Core\\Models\\skybox\\skybox\\OldHall\\left.png",
			"Core\\Models\\skybox\\skybox\\OldHall\\top.png",
			"Core\\Models\\skybox\\skybox\\OldHall\\bottom.png",
			"Core\\Models\\skybox\\skybox\\OldHall\\front.png",
			"Core\\Models\\skybox\\skybox\\OldHall\\back.png"
		};
		mCubeMapTexture = loadCubemap(Faces);
		//mCubeMapTexture = loadCubemapFromHDRI("Core\\Models\\skybox\\skybox\\scythian_tombs_2_4k.hdr");
		float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		glGenVertexArrays(1, &mskyboxVAO);
		glGenBuffers(1, &mskyboxVBO);
		glBindVertexArray(mskyboxVAO);
		glBindBuffer(GL_ARRAY_BUFFER, mskyboxVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		mSkyboxShader.Create("Core\\Models\\Shaders\\SkyboxVertex.glsl", "Core\\Models\\Shaders\\SkyboxFragment.glsl");
		mSkyboxShader.Bind();
		mSkyboxShader.SetInt("skybox", 0);
		mSkyboxShader.Unbind();
	}

	void cObjectManager::RenderSkyBox()
	{
		glDepthFunc(GL_LEQUAL);
		mSkyboxShader.Bind();
		glm::mat4 view = glm::mat4(glm::mat3(tCamera.View));
		mSkyboxShader.SetMat4f("view", view);
		mSkyboxShader.SetMat4f("projection", tCamera.Projection);
		glBindVertexArray(mskyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, mCubeMapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
	}

	uint32_t cObjectManager::loadCubemap(std::vector<std::string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}

	uint32_t cObjectManager::loadCubemapFromHDRI(const char* path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data)
		{
			int faceSize = width / 4;
			for (int i = 0; i < 6; i++)
			{
				int x = faceSize * (i % 4);
				int y = height - (faceSize * (i / 4) + faceSize);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, faceSize, faceSize, 0, GL_RGB, GL_UNSIGNED_BYTE, data + (y * width + x) * 3);
			}
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;

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