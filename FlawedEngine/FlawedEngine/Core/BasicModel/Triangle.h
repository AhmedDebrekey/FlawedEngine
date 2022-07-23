#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "../Shader.h"

#include <unordered_map>

namespace FlawedEngine
{
	class cTriangle : public cEntity
	{
	public:
		cTriangle();
		~cTriangle();
		virtual void Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;
		virtual void setDynamic(bool isDynamic) override;
		virtual void SetPhysics(eBasicObject Object) override;


	private:
		cModelRenderer Renderer;
	};
}