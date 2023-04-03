#pragma once

#include "Core.h"
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
//TODO (Me): TO BE CONTINUED
namespace FlawedEngine
{
	class cRenderer
	{
	public:
		uint32_t GenFramebuffer(uint32_t width, uint32_t height, const std::string& name);
		uint32_t GenDepthFrameBuffer(uint32_t width, uint32_t height, const std::string& name);
		
		uint32_t GenVertexBuffer(const std::string& name);
		uint32_t GenVertexBuffer(void* data, uint32_t size, const std::string& name);
		uint32_t GenIndexBuffer(void* data, uint32_t size, const std::string& name);

		uint32_t GenUniformBuffer(void* data, uint32_t size, const std::string& name);

		uint32_t Get(const std::string& name);

	private:

		std::unordered_map<std::string, uint32_t> mResources;

	public:
		static cRenderer& get();
		~cRenderer();
		
		cRenderer(cRenderer const&) = delete;
		void operator=(cRenderer const&) = delete;

	private:
		static cRenderer* sRendererInstance;

		cRenderer();
		cRenderer(cRenderer&);
	};
}

