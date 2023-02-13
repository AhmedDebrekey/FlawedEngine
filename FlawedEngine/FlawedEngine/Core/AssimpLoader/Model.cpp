#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../Models/stb_image.h"

namespace FlawedEngine
{
	void cModel::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox)
	{

		if (mRidigBody != nullptr && mRidigBody->getMotionState() && isPhysicsSet)
		{
			btTransform btTrans;
			mRidigBody->getMotionState()->getWorldTransform(btTrans);

			btVector3 ObjectTransform = btTrans.getOrigin();
			btVector3 Scale = mRidigBody->getCollisionShape()->getLocalScaling();

			glm::vec4 Rotation;
			btQuaternion quat = mRidigBody->getCenterOfMassTransform().getRotation();
			btVector3 v = quat.getAxis();
			Rotation.x = v.x();
			Rotation.y = v.y();
			Rotation.z = v.z();
			Rotation.w = quat.getAngle();

			glm::mat4 Model = glm::mat4(1.0f);
			Model = glm::translate(Model, glm::vec3(ObjectTransform.getX(), ObjectTransform.getY(), ObjectTransform.getZ()));
			Model = glm::rotate(Model, Rotation.w, glm::vec3(Rotation.x, Rotation.y, Rotation.z));
			Model = glm::scale(Model, glm::vec3(Scale.getX(), Scale.getY(), Scale.getZ()));
			Trans.Model = Model;
		}
		else
			Trans.Model = mModel;

		for (uint32_t i = 0; i < mMeshes.size(); i++)
		{
			mMeshes[i].Draw(Trans, mMaterial, LightPositions, SkyBox, Shader);
		}
	}

	void cModel::Update()
	{
	}

	void cModel::SetPhysics(eBasicObject Object, void* PhysicsWorld)
	{
	}

	void cModel::UnSetPhysics()
	{
	}

	void cModel::setDynamic(bool IsDynamic)
	{
	}

	void cModel::SetupScripting(const char*)
	{
	}

	void cModel::SendEntity(cEntity* Entity)
	{
	}

	void cModel::SendInputToScripting(std::function<bool(int)>)
	{
	}

	void cModel::loadModel(std::string path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
		mDirectory = path.substr(0, path.find_last_of('\\'));

		processNode(scene->mRootNode, scene);

		Shader.Create("Core/Models/Shaders/Vertex.glsl", "Core/Models/Shaders/Fragment.glsl");
		Shader.Bind();
		Shader.Unbind();
	}

	void cModel::processNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* Mesh = scene->mMeshes[node->mMeshes[i]];
			mMeshes.push_back(processMesh(Mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	cMesh cModel::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<sVertex> Vertecies;
		std::vector<uint32_t> Indecides;
		std::vector<sTexture> Textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			sVertex Vertex;

			glm::vec3 Vector;
			//Positions
			Vector.x = mesh->mVertices[i].x;
			Vector.y = mesh->mVertices[i].y;
			Vector.z = mesh->mVertices[i].z;
			Vertex.Postion = Vector;
			//Normals
			if (mesh->HasNormals())
			{
				Vector.x = mesh->mNormals[i].x;
				Vector.y = mesh->mNormals[i].y;
				Vector.z = mesh->mNormals[i].z;
				Vertex.Normal = Vector;
			}
			//Tex Coords
			if (mesh->mTextureCoords[0])
			{
				glm::vec2 vec;
				// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).

				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				Vertex.TexCoords = vec;
				//Tangents
				if (mesh->HasTangentsAndBitangents())
				{
					Vector.x = mesh->mTangents[i].x;
					Vector.y = mesh->mTangents[i].y;
					Vector.z = mesh->mTangents[i].z;
					Vertex.Tangent = Vector;
					//Bitangents
					Vector.x = mesh->mBitangents[i].x;
					Vector.y = mesh->mBitangents[i].y;
					Vector.z = mesh->mBitangents[i].z;
					Vertex.BiTangent = Vector;
				}
			}
			else
					Vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			
			Vertecies.push_back(Vertex);

		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
				Indecides.push_back(face.mIndices[j]);
		}
		
		aiMaterial* Material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<sTexture> diffuseMaps = loadMaterialTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse");
		Textures.insert(Textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<sTexture> specularMaps = loadMaterialTextures(Material, aiTextureType_SPECULAR, "texture_specular");
		Textures.insert(Textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<sTexture> normalMaps = loadMaterialTextures(Material, aiTextureType_HEIGHT, "texture_normal");
		Textures.insert(Textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<sTexture> heightMaps = loadMaterialTextures(Material, aiTextureType_AMBIENT, "texture_height");
		Textures.insert(Textures.end(), heightMaps.begin(), heightMaps.end());

		return cMesh(Vertecies, Indecides, Textures);
	}

	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
	{
		std::string filename = std::string(path);
		filename = directory + '\\' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width = 0, height = 0, nrComponents = 0;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	std::vector<sTexture> cModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<sTexture> Textures;
		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			
			bool skip = false;
			for (uint32_t j = 0; j < mLoadedTextures.size(); j++)
			{
				if (std::strcmp(mLoadedTextures[j].Path.data(), str.C_Str()) == 0)
				{
					Textures.push_back(mLoadedTextures[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				sTexture Texture;
				Texture.ID = TextureFromFile(str.C_Str(), mDirectory, false);
				Texture.Type = typeName;
				Texture.Path = str.C_Str();
				Textures.push_back(Texture);
				mLoadedTextures.push_back(Texture);
			}
		}
		return Textures;
	}
}