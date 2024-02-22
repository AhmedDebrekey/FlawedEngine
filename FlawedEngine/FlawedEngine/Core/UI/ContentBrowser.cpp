#include "../UIManager.h"

void RenderIcons(const std::vector<std::filesystem::directory_entry>& entries, std::filesystem::path& currentDir, bool isFile, uint32_t fileIcon, uint32_t dirIcon, float size)
{
	for (const auto& directoryEntry : entries) {
		const auto& path = directoryEntry.path();
		std::string filenameString = path.filename().string();

		ImGui::PushID(filenameString.c_str());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		uint32_t icon = isFile ? fileIcon : dirIcon;
		ImGui::ImageButton((ImTextureID)icon, { size, size });

		if (ImGui::BeginDragDropSource())
		{
			std::filesystem::path relativePath(path);
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
				currentDir /= path.filename();

		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}
}

void FlawedEngine::cUIManager::RenderContentBrowser()
{
	ImGui::Begin("Content Browser");


	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if (ImGui::ImageButton((ImTextureID)mBackIcon, { 20, 20 }))
	{
		if (mCurrentDir != std::filesystem::path(mBaseDir))
			mCurrentDir = mCurrentDir.parent_path();
	}

	ImGui::PopStyleColor();

	ImGui::SameLine();

	std::string directory = "Assets\\" + std::filesystem::relative(mCurrentDir, mBaseDir).string();
	ImGui::Text(directory.c_str());

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	std::vector<std::filesystem::directory_entry> directories;
	std::vector<std::filesystem::directory_entry> files;

	for (const auto& directoryEntry : std::filesystem::directory_iterator(mCurrentDir)) {
		if (directoryEntry.is_directory())
			directories.push_back(directoryEntry);
		else
			files.push_back(directoryEntry);
	}

	RenderIcons(directories, mCurrentDir, false, mFileIcon, mDirIcon, thumbnailSize);
	RenderIcons(files, mCurrentDir, true, mFileIcon, mDirIcon, thumbnailSize);


	ImGui::Columns(1);
	ImGui::End();
}
