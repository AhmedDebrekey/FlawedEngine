#pragma once

#include "GraphicsAPI.h"

namespace FlawedEngine
{
	class cOpenGLAPI : public cGraphicsAPI
	{
	public:
		cOpenGLAPI ();
		~cOpenGLAPI ();

        // Initialization and cleanup
        virtual void Initialize() override;
        virtual void Shutdown() override;

        // Framebuffer operations
        virtual unsigned int CreateFramebuffer() override;
        virtual void DeleteFramebuffer(unsigned int framebuffer) override;
        virtual void BindFramebuffer(unsigned int framebuffer) override;

        // Texture operations
        virtual unsigned int CreateTexture(int width, int height, bool isDepth = false, unsigned char* data = nullptr, int nrComponents = 0) override;
        virtual void DeleteTexture(unsigned int texture) override;
        virtual void BindTexture(unsigned int texture, eTextureType type = eTextureType::Texture2D) override;
        virtual void ActiveTexture(unsigned int unit) override;
        virtual unsigned int GetTextureType(eTextureType type) override;

        // Drawing operations
        virtual void SetViewport(int x, int y, int width, int height) override;
        virtual void ClearColorBuffer(const sColor& color) override;
        virtual void ClearDepthBuffer() override;
        virtual void DrawElements(size_t count) override;

        // State management
        virtual void SetDepthTest(bool enabled) override;
        virtual void SetBlendMode(eBlendMode mode) override;
        virtual void EnableFaceCulling(bool enabled) override;

        // Buffer operations
        virtual unsigned int CreateBuffer(eBufferType type, const void* data, size_t size) override;
        virtual void DeleteBuffer(unsigned int buffer) override;
        virtual void BindBuffer(eBufferType type, unsigned int buffer) override;
        virtual void BindBufferBase(eBufferType type, unsigned int index, unsigned int buffer) override;
        virtual void BindBufferSubData(eBufferType type, int offset, size_t size, void* data) override;

        // Vertex array management
        virtual unsigned int CreateVertexArray() override;
        virtual void BindVertexArray(unsigned int VAO) override;

        // Buffer swapping
        virtual void Present() override;

        //Type Management
        virtual unsigned int GetBufferType(eBufferType type) override;
	};
}