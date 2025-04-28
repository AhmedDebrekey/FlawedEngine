#include "Cube.h"

namespace FlawedEngine
{
	cCube::cCube()
	{
		//https://pastebin.com/DXKEmvap
        mVertexBuffer = {
            // Front face
            { glm::vec3(-1, -1,  1), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
            { glm::vec3(1, -1,  1), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
            { glm::vec3(1,  1,  1), glm::vec3(0, 0, 1), glm::vec2(1, 1) },
            { glm::vec3(-1,  1,  1), glm::vec3(0, 0, 1), glm::vec2(0, 1) },

            // Back face
            { glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 0) },
            { glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 0) },
            { glm::vec3(-1,  1, -1), glm::vec3(0, 0, -1), glm::vec2(1, 1) },
            { glm::vec3(1,  1, -1), glm::vec3(0, 0, -1), glm::vec2(0, 1) },

            // Left face
            { glm::vec3(-1, -1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 0) },
            { glm::vec3(-1, -1,  1), glm::vec3(-1, 0, 0), glm::vec2(1, 0) },
            { glm::vec3(-1,  1,  1), glm::vec3(-1, 0, 0), glm::vec2(1, 1) },
            { glm::vec3(-1,  1, -1), glm::vec3(-1, 0, 0), glm::vec2(0, 1) },

            // Right face
            { glm::vec3(1, -1,  1), glm::vec3(1, 0, 0), glm::vec2(0, 0) },
            { glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0) },
            { glm::vec3(1,  1, -1), glm::vec3(1, 0, 0), glm::vec2(1, 1) },
            { glm::vec3(1,  1,  1), glm::vec3(1, 0, 0), glm::vec2(0, 1) },

            // Top face
            { glm::vec3(-1, 1,  1), glm::vec3(0, 1, 0), glm::vec2(0, 0) },
            { glm::vec3(1, 1,  1), glm::vec3(0, 1, 0), glm::vec2(1, 0) },
            { glm::vec3(1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(1, 1) },
            { glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(0, 1) },

            // Bottom face
            { glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(0, 0) },
            { glm::vec3(1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(1, 0) },
            { glm::vec3(1, -1,  1), glm::vec3(0, -1, 0), glm::vec2(1, 1) },
            { glm::vec3(-1, -1,  1), glm::vec3(0, -1, 0), glm::vec2(0, 1) },
        };

        mIndices = {
            0, 1, 2, 2, 3, 0,       // Front
            4, 5, 6, 6, 7, 4,       // Back
            8, 9,10,10,11, 8,       // Left
           12,13,14,14,15,12,       // Right
           16,17,18,18,19,16,       // Top
           20,21,22,22,23,20        // Bottom
        };


		Renderer.Init(mVertexBuffer, mTextureCoords, mIndices);

	}

	cCube::~cCube()
	{

	}

	void cCube::Render(sTransform& Trans, std::unordered_map<std::string, sLight>& LightPositions, uint32_t* SkyBox, sGBufferObjects* GeometryObject)
	{
		Trans.Model = mModel;
		Renderer.Draw(Trans, mMaterial, LightPositions, SkyBox);
	}

	void cCube::ShadowRender(sTransform& Trans, glm::mat4& LightSpaceMatrix, uint32_t DepthMap)
	{
	}

	void cCube::Update()
	{

	}
	void cCube::SetAABB(glm::vec3& Scale)
	{
	}
	void cCube::AddAnimation(const char*)
	{
	}
}