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
		virtual void Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics() override;

	private:
		cModelRenderer Renderer;
	};
}
