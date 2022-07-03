#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"

namespace FlawedEngine
{
	class cCube : public cEntity
	{
	public:
		cCube();
		~cCube();
		virtual void Render(Transform& Trans) override;
		virtual void Update() override;

	private:
		cModelRenderer Renderer;
	};
}
