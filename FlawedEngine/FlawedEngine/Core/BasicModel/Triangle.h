#pragma once

#include "../Entity.h"
#include "../Models/ModelRenderer.h"

namespace FlawedEngine
{
	class cTriangle : public cEntity
	{
	public:
		cTriangle();
		~cTriangle();
		virtual void Render() override;

		virtual void GetInfo() override;
	private:
		cModelRenderer Renderer;
		
	};
}

