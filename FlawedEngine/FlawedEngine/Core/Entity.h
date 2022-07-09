#pragma once

#include "Core.h"
#include "Input/Input.h"

#include <iostream>

#include <unordered_map>

namespace FlawedEngine
{
	class cEntity
	{
	public:
		sModel mTransformation; //I can't find a better name

		virtual void Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions) = 0;
		virtual void Update(/*Should be taking in the timestep, Maybe make deltatime a singleton that is avaliable from the Engine*/) = 0;
		void ModelTransform(sModel& model);
		void SetColor(glm::vec3 Color);
		void SetMaterial(sMaterial& Mat);
		virtual ~cEntity() = 0;

		sMaterial mMaterial;

		glm::mat4 mModel;

		std::vector<sVertex> mVertexBuffer;
		std::vector<uint32_t> mIndices;
		std::vector<sTexture> mTextureCoords;
		//std::vector<float> mIndexBuffer;

		cInput& Input = cInput::get();
	};

	inline cEntity::~cEntity()
	{
		mVertexBuffer.clear();
		mTextureCoords.clear();
		mIndices.clear();
	}

	inline void cEntity::ModelTransform(sModel& model)
	{//this is a big mess tbh, i am confusion
		mTransformation = model;
		glm::mat4 Model = glm::mat4(1.0f);
		Model = glm::translate(Model, mTransformation.Translation);
		Model = glm::rotate(Model, glm::radians(mTransformation.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		Model = glm::rotate(Model, glm::radians(mTransformation.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		Model = glm::rotate(Model, glm::radians(mTransformation.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		Model = glm::scale(Model, mTransformation.Scale);

		mModel = Model;
	}

	inline void cEntity::SetColor(glm::vec3 Color)
	{
		mMaterial.Color = Color;
	}

	inline void cEntity::SetMaterial(sMaterial& Mat)
	{
		mMaterial = Mat;
	}
}