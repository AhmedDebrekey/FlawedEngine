#pragma once

#include "Core.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

class cUIManager
{
public:
	cUIManager();
	~cUIManager();
	void Init(void* Window);
	void UpdateUI();
	void RenderUI();

private:
	void InitRendering();
	void InitFrameBuffer();

private:
	uint32_t FrameBuffer;
	uint32_t TextureColorBuffer;
	uint32_t RenderBufferObject;

	ImVec2 ViewportSize;
};

