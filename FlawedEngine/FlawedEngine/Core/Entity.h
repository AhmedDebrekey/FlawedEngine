#pragma once

#include "Core.h"
#include "Input/Input.h"

#include <iostream> //temp for the destructor, remove later

namespace FlawedEngine
{
	class cEntity
	{
	public:
		glm::vec3 Postition, Rotation, Scale; 

		virtual void Render(Transform& Trans) = 0;
		virtual void Update() = 0;
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

		cInput& Input = cInput::get();
	};

	inline cEntity::~cEntity()
	{
		std::cout << "Entity Has Been Destructed" << std::endl;
		mVertexBuffer.clear();
		mTextureCoords.clear();
		//mIndexBuffer.clear();
	}
}