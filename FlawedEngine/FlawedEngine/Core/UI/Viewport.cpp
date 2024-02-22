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

	mViewportSize = { ImGui::GetWindowWidth(), ImGui::GetWindowHeight() };
	mViewportPos = { ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };

	if (mPrevViewportSize != mViewportSize)
	{
		//resize framebuffer
		InitFrameBuffer();
		mCamera->SetAspectRatio(static_cast<float>(mViewportSize.x) / mViewportSize.y);
	}

	mPrevViewportSize = mViewportSize;
	ImGui::Image((void*)mTextureColorBuffer, { mViewportSize.x, mViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsMouseDown(2) && ImGui::IsWindowHovered())
		mSelectedEntity = "";

	RenderGizmo();

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::filesystem::path objectPath(path);
			mObjectMan->LoadObject(objectPath.string().c_str(), objectPath.filename().string().c_str());
		}
		ImGui::EndDragDropTarget();
	}


	ImGui::End();
	ImGui::PopStyleVar();
}