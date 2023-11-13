#include "../UIManager.h"

void FlawedEngine::cUIManager::RenderViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("ViewPort", 0, ImGuiWindowFlags_NoScrollbar);

	// Get the size of the window in screen coordinates
	int gwindowWidth, gwindowHeight;
	glfwGetWindowSize((GLFWwindow*)mWindow, &gwindowWidth, &gwindowHeight);

	// Get the size of the framebuffer in pixels
	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize((GLFWwindow*)mWindow, &framebufferWidth, &framebufferHeight);

	ViewportSize = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
	ViewportPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };

	if (PrevViewportSize != ViewportSize)
	{
		//resize framebuffer
		InitFrameBuffer();
		mCamera->UpdateProjection(glm::perspective(glm::radians(mCamera->FoV()), ViewportSize.x / ViewportSize.y, mCamera->NearPlane(), mCamera->FarPlane()));
	}

	PrevViewportSize = ViewportSize;
	ImGui::Image((void*)TextureColorBuffer, { ViewportSize.x, ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsMouseDown(2) && ImGui::IsWindowHovered())
		mSelectedEntity = "";

	//Gizmo
	RenderGizmo(); 
	ImGui::End();
	ImGui::PopStyleVar();
}