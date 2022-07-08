#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"

#include <unordered_map>

namespace FlawedEngine
{
	class cPointLight : public cEntity
	{
	public:
		cPointLight();
		~cPointLight();
		virtual void Render(Transform& Trans, std::unordered_map<std::string, glm::vec3>& LightPositions) override;
		virtual void Update() override;

		glm::vec3 mPosition;
	private:
		cModelRenderer Renderer;
		glm::vec3 mColor;
	};
}

