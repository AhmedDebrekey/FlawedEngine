#pragma once
#include "../Core.h"

namespace FlawedEngine
{
    class cGraphicsAPI {
    public:
        // Initialization and cleanup
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        // Framebuffer operations
        virtual unsigned int CreateFramebuffer(unsigned int texturebuffer) = 0;
        virtual void DeleteFramebuffer(unsigned int framebuffer) = 0;
        virtual void BindFramebuffer(unsigned int framebuffer) = 0;
        virtual void AttachRenderBufferToFrameBuffer(unsigned int renderbuffer) = 0;

        // Renderbuffer operations
        virtual unsigned int CreateRenderBuffer(int width, int height) = 0;
        virtual void BindRenderBuffer(unsigned int renderbuffer) = 0;
        virtual void DeleteRenderBuffer(unsigned int renderBuffer) = 0;

        // Texture operations
        virtual unsigned int CreateTexture(int width, int height, int nrComponents, unsigned char* data, sTextureProps props) = 0;
        virtual unsigned int CreateDepthTexture(int width, int height) = 0;
        virtual void DeleteTexture(unsigned int texture) = 0;
        virtual void BindTexture(unsigned int texture, eTextureType type = eTextureType::Texture2D) = 0;
        virtual void ActiveTexture(unsigned int unit) = 0;

        // Drawing operations
        virtual void SetViewport(int x, int y, int width, int height) = 0;
        virtual void ClearColorBuffer(const sColor& color) = 0;
        virtual void ClearDepthBuffer() = 0;
        virtual void DrawElements(size_t count) = 0;

        // State management
        virtual void SetDepthTest(bool enabled) = 0;
        virtual void SetBlendMode(eBlendMode mode) = 0;
        virtual void EnableFaceCulling(bool enabled) = 0;

        // Buffer operations
        virtual unsigned int CreateBuffer(eBufferType type, const void* data, size_t size, eDrawType draw) = 0;
        virtual void DeleteBuffer(unsigned int buffer) = 0;
        virtual void BindBuffer(eBufferType type, unsigned int buffer) = 0;
        virtual void BindBufferBase(eBufferType type, unsigned int index, unsigned int buffer) = 0;
        virtual void BindBufferSubData(eBufferType type, int offset, size_t size, void* data) = 0;

        // Vertex array management
        virtual unsigned int CreateVertexArray() = 0;
        virtual void BindVertexArray(unsigned int VAO) = 0;

        // Buffer swapping
        virtual void Present() = 0;

        // Type management
        virtual unsigned int GetBufferType(eBufferType type) = 0;
        virtual unsigned int GetTextureType(eTextureType type) = 0;
        virtual unsigned int GetTextureProp(eTextureProperties prop) = 0;
        virtual unsigned int GetDrawType(eDrawType type) = 0;
    };
}