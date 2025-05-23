#include "Model.h"
#include "../Models/stb_image.h"

namespace FlawedEngine
{
	void cModel::processNode(aiNode* node, const aiScene* scene)
	{
		for (uint32_t i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* Mesh = scene->mMeshes[node->mMeshes[i]];
			mCPUMeshes.push_back(processMesh(Mesh, scene));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void PrintMaterialTextures(const aiMaterial* material)
	{
		static const std::map<aiTextureType, std::string> textureTypeNames = {
			{ aiTextureType_DIFFUSE,       "Diffuse" },
			{ aiTextureType_SPECULAR,      "Specular" },
			{ aiTextureType_AMBIENT,       "Ambient" },
			{ aiTextureType_EMISSIVE,      "Emissive" },
			{ aiTextureType_HEIGHT,        "Height" },
			{ aiTextureType_NORMALS,       "Normals" },
			{ aiTextureType_SHININESS,     "Shininess" },
			{ aiTextureType_OPACITY,       "Opacity" },
			{ aiTextureType_DISPLACEMENT,  "Displacement" },
			{ aiTextureType_LIGHTMAP,      "Lightmap" },
			{ aiTextureType_REFLECTION,    "Reflection" },
			{ aiTextureType_BASE_COLOR,    "Base Color" },
			{ aiTextureType_NORMAL_CAMERA, "Normal Camera" },
			{ aiTextureType_EMISSION_COLOR,"Emission Color" },
			{ aiTextureType_METALNESS,     "Metalness" },
			{ aiTextureType_DIFFUSE_ROUGHNESS, "Diffuse Roughness" },
			{ aiTextureType_AMBIENT_OCCLUSION, "Ambient Occlusion" },
			{ aiTextureType_UNKNOWN,       "Unknown" }
		};

		for (const auto& [type, name] : textureTypeNames)
		{
			unsigned int count = material->GetTextureCount(type);

			std::cout << name << ": " << count << std::endl;
		}
		std::cout << std::endl;
	}

	MeshCPUData cModel::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<sVertex> Vertecies;
		std::vector<uint32_t> Indecides;
		std::vector<sTexture> Textures;

		for (uint32_t i = 0; i < mesh->mNumVertices; i++)
		{
			sVertex Vertex;
			SetVertexBoneDataToDefault(Vertex);

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
		
		std::cout << scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str() << std::endl;
		PrintMaterialTextures(scene->mMaterials[mesh->mMaterialIndex]);


		std::vector<sTexture> diffuseMaps = loadMaterialTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse");
		Textures.insert(Textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<sTexture> specularMaps = loadMaterialTextures(Material, aiTextureType_SPECULAR, "texture_specular");
		Textures.insert(Textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<sTexture> normalMaps = loadMaterialTextures(Material, aiTextureType_NORMALS, "texture_normal");
		Textures.insert(Textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<sTexture> heightMaps = loadMaterialTextures(Material, aiTextureType_AMBIENT, "texture_height");
		Textures.insert(Textures.end(), heightMaps.begin(), heightMaps.end());

		ExtractBoneWeightForVertices(Vertecies, mesh, scene);


		return MeshCPUData(Vertecies, Indecides, Textures);
	}

	std::vector<sTexture> cModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
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
				Texture.ID = -1;//TextureFromFile(str.C_Str(), mDirectory, false, mGfxAPI);
				Texture.Type = typeName;
				Texture.Path = str.C_Str();
				Textures.push_back(Texture);
				mLoadedTextures.push_back(Texture);
			}
		}
		return Textures;
	}

	void cModel::ExtractBoneWeightForVertices(std::vector<sVertex>& vertices, aiMesh* mesh, const aiScene* scene)
	{
		auto& boneInfoMap = m_BoneInfoMap;
		int& boneCount = m_BoneCounter;

		for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
		{
			int boneID = -1;
			std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				sBoneInfo newBoneInfo;
				newBoneInfo.id = boneCount;
				newBoneInfo.offset = ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);
				boneInfoMap[boneName] = newBoneInfo;
				boneID = boneCount;
				boneCount++;
			}
			else
			{
				boneID = boneInfoMap[boneName].id;
			}
			assert(boneID != -1);
			auto weights = mesh->mBones[boneIndex]->mWeights;
			int numWeights = mesh->mBones[boneIndex]->mNumWeights;

			for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
			{
				int vertexId = weights[weightIndex].mVertexId;
				float weight = weights[weightIndex].mWeight;
				assert(vertexId <= vertices.size());
				SetVertexBoneData(vertices[vertexId], boneID, weight);
			}
		}
	}

	void cModel::SetVertexBoneDataToDefault(sVertex& vertex)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
		{
			vertex.mBoneIDs[i] = -1;
			vertex.mWeights[i] = 0.0f;
		}
	}

	void cModel::SetVertexBoneData(sVertex& vertex, int boneID, float weight)
	{
		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			if (vertex.mBoneIDs[i] < 0)
			{
				if (weight == 0)
					break;

				vertex.mWeights[i] = weight;
				vertex.mBoneIDs[i] = boneID;
				break;
			}
		}
	}

}