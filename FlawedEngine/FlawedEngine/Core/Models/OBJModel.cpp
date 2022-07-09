#include "OBJModel.h"


namespace FlawedEngine
{
	cOBJModel::cOBJModel(const char* FilePath)
	{
		if (!LoadModel(FilePath))
		{
			std::cout << "Failed To Load 3D Model from: " << FilePath << std::endl;
		}

		Populate();
		
		Renderer.Init(mVertexBuffer, mTextureCoords, mIndices);
	}

	cOBJModel::~cOBJModel()
	{
	}

	bool cOBJModel::LoadModel(const char* FilePath)
	{
		bool Loaded = mLoader.LoadFile(FilePath);

		return Loaded;
	}

	void cOBJModel::Populate()
	{
		for (auto CurrentMesh : mLoader.LoadedMeshes)
		{
			for (auto &Vertex : mLoader.LoadedVertices)
			{
				//3 postional float, 3 Normal float, 2 UV Coords
				glm::vec3 Position = glm::vec3(Vertex.Position.X, Vertex.Position.Y, Vertex.Position.Z);
				glm::vec3 Normal = glm::vec3(Vertex.Normal.X, Vertex.Normal.Y, Vertex.Normal.Z);
				glm::vec2 TexCoords = glm::vec2(Vertex.TextureCoordinate.X, Vertex.TextureCoordinate.Y);
				mVertexBuffer.push_back({ Position, Normal, TexCoords });
			}

			for (uint32_t i = 0; i < CurrentMesh.Indices.size(); i += 3)
			{
				mIndices.push_back(CurrentMesh.Indices.at(i));
				mIndices.push_back(CurrentMesh.Indices.at(i + 1));
				mIndices.push_back(CurrentMesh.Indices.at(i + 2));
			}
		}
	}

	void cOBJModel::Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions)
	{
		Trans.Model = mModel;
		Renderer.Draw(Trans, mMaterial, LightPositions);
	}

	void cOBJModel::Update()
	{

	}
}