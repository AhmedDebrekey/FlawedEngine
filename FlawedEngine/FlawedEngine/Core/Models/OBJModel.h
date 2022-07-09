#pragma once
#include "../Entity.h"
#include "../Models/ModelRenderer.h"
#include "OBJ_Loader.h"
#include <unordered_map>
#include "../Core.h"

namespace FlawedEngine
{
	class cOBJModel : public cEntity
	{
	public:
		cOBJModel(const char* FilePath);
		~cOBJModel();
		virtual void Render(Transform& Trans, std::unordered_map<std::string, sLight>& LightPositions) override;
		virtual void Update() override;

		bool LoadModel(const char* FilePath);
		void Populate();

	private:
		cModelRenderer Renderer;
		objl::Loader mLoader;
	};
}
