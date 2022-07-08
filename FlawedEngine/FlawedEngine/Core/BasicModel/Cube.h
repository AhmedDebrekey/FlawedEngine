#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include <unordered_map>

namespace FlawedEngine
{
	class cCube : public cEntity
	{
	public:
		cCube();
		~cCube();
		virtual void Render(Transform& Trans, std::unordered_map<std::string, glm::vec3>& LightPositions) override;
		virtual void Update() override;

	private:
		cModelRenderer Renderer;
	};
}
