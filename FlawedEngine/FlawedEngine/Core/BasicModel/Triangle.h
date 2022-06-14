#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "../Shader.h"


namespace FlawedEngine
{
	class cTriangle : public cEntity
	{
	public:
		cTriangle();
		~cTriangle();
		virtual void Render(Transform& Trans) override;
		virtual void Update() override;

	private:
		cModelRenderer Renderer;
	};
}