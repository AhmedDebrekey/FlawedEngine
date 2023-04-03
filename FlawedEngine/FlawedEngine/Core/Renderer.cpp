#include "Renderer.h"

namespace FlawedEngine
{
	cRenderer* cRenderer::sRendererInstance = nullptr;

	cRenderer::cRenderer()
	{

	}

	cRenderer::~cRenderer()
	{

	}

	uint32_t cRenderer::GenFramebuffer(uint32_t width, uint32_t height, const std::string& name)
	{
		return 0;
	}

	uint32_t cRenderer::GenDepthFrameBuffer(uint32_t width, uint32_t height, const std::string& name)
	{
		return 0;
	}

	uint32_t cRenderer::GenVertexBuffer(const std::string& name)
	{
		uint32_t vao;
		glGenVertexArrays(1, &vao);

		mResources[name] = vao;

		return vao;
	}

	uint32_t cRenderer::GenVertexBuffer(void* data, uint32_t size, const std::string& name)
	{
		uint32_t vbo;
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

		mResources[name] = vbo;

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return vbo;
	}

	uint32_t cRenderer::GenIndexBuffer(void* data, uint32_t size, const std::string& name)
	{
		return 0;
	}

	uint32_t cRenderer::GenUniformBuffer(void* data, uint32_t size, const std::string& name)
	{
		return 0;
	}

	uint32_t cRenderer::Get(const std::string& name)
	{
		return 0;
	}

	cRenderer& cRenderer::get()
	{
		if (sRendererInstance == nullptr)
		{
			sRendererInstance = new cRenderer();
		}
		return *sRendererInstance;
	}
}