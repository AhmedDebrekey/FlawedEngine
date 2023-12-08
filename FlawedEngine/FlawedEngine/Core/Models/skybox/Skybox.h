#pragma once

#include <vector>
#include <string>
#include "../../Shader.h"
#include "../../Graphics/GraphicsAPI.h"
namespace FlawedEngine
{
	class cSkybox
	{
	public:
		cSkybox();
		~cSkybox();

		void Setup(void* GfxAPI);
		void RenderSkyBox(const sTransform& camera);

		void ToggleSkyBox();

		uint32_t GetSkyTexture();
	private:
		cShader mSkyboxShader;
		uint32_t mSkyBoxVAO;
		uint32_t mSkyBoxVBO;
		bool mShouldRenderSkyBox = true;

		uint32_t mCubeMapTexture;

		cGraphicsAPI* mGfxAPI = nullptr;
	};
}