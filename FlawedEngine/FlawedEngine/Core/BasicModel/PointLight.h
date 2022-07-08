#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"

namespace FlawedEngine
{
	class cPointLight : public cEntity
	{
	public:
		cPointLight();
		~cPointLight();
		virtual void Render(Transform& Trans) override;
		virtual void Update() override;

		glm::vec3 mPosition;
	private:
		cModelRenderer Renderer;
		glm::vec3 mColor;
	};
}

