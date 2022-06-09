#pragma once

#include <vector>
#include <string>

namespace FlawedEngine
{
	typedef unsigned int uint32_t;
#define MAX_BONE_INFLUENCE 4
	struct vec3//temp
	{
		float x, y, z;
	};
	struct vec2//temp
	{
		float x, y;
	};

	struct sVertex
	{
		vec3 Postion;
		vec3 Normal;
		vec2 TexCoords;

		vec3 Tangent;
		vec3 BiTangent;

		int mBoneIDs[MAX_BONE_INFLUENCE];
		float mWeights[MAX_BONE_INFLUENCE];
	};

	struct sTexture
	{
		uint32_t ID;
		std::string Type;
		std::string Path;
	};

	class cEntity
	{
	public:
		vec3 Postition, Rotation, Scale; 

		virtual void Render() = 0;
		virtual ~cEntity() = 0;

		struct sMaterial
		{
			float Shininess;
			float Roughness;
			//...
		};

		sMaterial mMaterial;

		std::vector<sVertex> mVertexBuffer;
		std::vector<sTexture> mTextureCoords;
		//std::vector<float> mIndexBuffer;
	};

	inline cEntity::~cEntity()
	{
		mVertexBuffer.clear();
		mTextureCoords.clear();
		//mIndexBuffer.clear();
	}
}