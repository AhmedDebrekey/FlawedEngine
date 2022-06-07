#pragma once

#include <vector>

namespace FlawedEngine
{
	class cEntity
	{
	public:
		float Postition, Rotation, Scale; //TODO: Give appropriate type later...

		virtual void Render() = 0;

		virtual ~cEntity() = 0;

		struct sMaterial
		{
			float Shininess;
			float Roughness;
			//...
		};

		sMaterial mMaterial;

		//Give appriopriate types
		std::vector<float> mVertexBuffer;
		std::vector<float> mIndexBuffer;
		std::vector<float> mTexturePos;
		std::vector<float> mNormals;
	};
}