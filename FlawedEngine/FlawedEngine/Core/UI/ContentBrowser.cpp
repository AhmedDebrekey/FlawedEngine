#include "../UIManager.h"
#include "../Models/stb_image.h"

void OpenFileWithDefaultProgram(const std::string& filepath);
static std::filesystem::path SelectedDir = "";
static std::filesystem::path CopyDir = "";

void FlawedEngine::cUIManager::RenderContentBrowser()
{
	ImGui::Begin("Content Browser");

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	if (ImGui::ImageButton((ImTextureID)mBackIcon, { 20, 20 }))
	{
		if (mCurrentDir != std::filesystem::path(mBaseDir))
		{
			mCurrentDir = mCurrentDir.parent_path();
			SelectedDir = ""; 
			GetFilesandFolders(mCurrentDir);
		}
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID)mRefreshIcon, { 20, 20 }))
	{
		if (mCurrentDir != std::filesystem::path(mBaseDir))
		{
			
			GetFilesandFolders(mCurrentDir);
		}
	}
	ImGui::SameLine();
	ImGui::ImageButton((ImTextureID)mTrashIcon, { 20, 20 });
	
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			std::filesystem::path dataPath(path);
			std::filesystem::remove_all(dataPath);
			GetFilesandFolders(mCurrentDir);
		}
		ImGui::EndDragDropTarget();
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
	std::filesystem::path PrevDir = mCurrentDir;

	if (ImGui::BeginPopupContextWindow(""))
	{
		if (ImGui::MenuItem("Add folder"))
		{
			std::filesystem::create_directory(mCurrentDir / "New Folder");
			GetFilesandFolders(mCurrentDir);
		}


		if (ImGui::MenuItem("Add Script"))
		{
			std::ofstream ofs(mCurrentDir / "New Script.lua");
			//add script template into the file
			ofs << "function Create()\n"
				<< "\n"
				<< "end\n"
				<< "\n"
				<< "function Update()\n"
				<< "\n"
				<< "end\n"
				<< "\n"
				<< "function OnCollision(otherEntity)\n"
				<< "\n"
				<< "end";
			ofs.close();
			GetFilesandFolders(mCurrentDir);
		}


		if (ImGui::MenuItem("Paste"))
		{
			if (CopyDir != "")
			{
				if (std::filesystem::is_directory(CopyDir))
					std::filesystem::copy(CopyDir, mCurrentDir / CopyDir.filename());
				else
					std::filesystem::copy(CopyDir, mCurrentDir);
				CopyDir = "";
				GetFilesandFolders(mCurrentDir);
			}
		}

		if(SelectedDir != "")
		{
			if (ImGui::MenuItem("Open"))
			{
				OpenFileWithDefaultProgram(SelectedDir.string());
			}

			if (ImGui::MenuItem("Copy"))
			{
				CopyDir = SelectedDir;
			}

			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
			if (ImGui::MenuItem("Delete"))
			{
				std::filesystem::remove_all(SelectedDir);
				SelectedDir = "";
				GetFilesandFolders(mCurrentDir);
			}
			ImGui::PopStyleColor();
		}
		ImGui::EndPopup();
	}

	for (const auto& directoryEntry : mDirectories)
	{

		const auto& path = directoryEntry.path();
		std::string filenameString = path.filename().string();

		ImGui::PushID(filenameString.c_str());
		ImVec4 color = path == SelectedDir ? ImVec4(0.788f, 0.663f, 0.318f, 1) : ImVec4(0, 0, 0, 0);
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		uint32_t icon = directoryEntry.is_directory() ? mDirIcon : GetFileIcon(directoryEntry);
		ImGui::ImageButton((ImTextureID)icon, { thumbnailSize, thumbnailSize });

		if (ImGui::BeginDragDropSource())
		{
			std::filesystem::path relativePath(path);
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleColor();


		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				if (SelectedDir != path)
					SelectedDir = path;
				else
					SelectedDir = "";
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				SelectedDir = path;
			}
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && directoryEntry.is_directory())
			{
				mCurrentDir /= path.filename();
				SelectedDir = "";
				GetFilesandFolders(mCurrentDir);
			}
		}


		char buffer[256];
		strcpy(buffer, filenameString.c_str());
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
		ImGui::InputText("##filename", buffer, 256);
		ImGui::PopStyleColor();

		// Check if the user has finished editing the text
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			// Update the path with the new filename
			std::filesystem::path newPath = path.parent_path() / buffer;
			std::error_code ec;
			std::filesystem::rename(path, newPath, ec);
			if (ec)
			{
				EngineLog("Couldn't rename file" + ec.message(), Error);
			}
			else
			{
				GetFilesandFolders(mCurrentDir);
			}
		}
		
		ImGui::NextColumn();
		ImGui::PopID();
		if (mCurrentDir != PrevDir) // To prevent unrealdable memory with the deque
			break;
	}
	


	ImGui::Columns(1);
	ImGui::End();
}

bool CompareEntries(const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b)
{
	if (a.is_directory() && b.is_directory())
		return a.path().filename() < b.path().filename();

	if (a.is_regular_file() && b.is_regular_file())
	{
		std::string ext_a = a.path().extension().string();
		std::string ext_b = b.path().extension().string();

		std::transform(ext_a.begin(), ext_a.end(), ext_a.begin(), ::tolower);
		std::transform(ext_b.begin(), ext_b.end(), ext_b.begin(), ::tolower);

		if (ext_a != ext_b)
			return ext_a < ext_b;

		return a.path().filename() < b.path().filename();
	}

	return a.is_directory();
}

void FlawedEngine::cUIManager::GetFilesandFolders(const std::filesystem::path& path)
{
	if (!(std::filesystem::exists(path) && std::filesystem::is_directory(path)))
		return;

	mDirectories.clear();
	
	for (const auto& entry : std::filesystem::directory_iterator(path))
		mDirectories.push_back(entry);

	std::sort(mDirectories.begin(), mDirectories.end(), CompareEntries);
}


uint32_t FlawedEngine::cUIManager::GetFileIcon(const std::filesystem::path& path)
{
	std::string extension = path.extension().string();

	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == ".png" || extension == ".jpeg" || extension == ".jpg")
		return mImageIcon;
	else if (extension == ".fbx" || extension == ".gltf" || extension == ".obj" || extension == ".dae")
		return mModelIcon;
	else if (extension == ".lua")
		return mLuaIcon;
	else if (extension == ".json")
		return mSaveIcon;
	else
		return mFileIcon;
}

void FlawedEngine::cUIManager::LoadIcons() {
	mBaseDir = std::filesystem::current_path() / "Assets";
	mCurrentDir = std::filesystem::current_path() / "Assets";

	sTextureProps props;
	props.Wrap_s = eTextureProperties::Repeat;
	props.Wrap_t = eTextureProperties::Repeat;
	props.Min_Filter = eTextureProperties::MIPMAP_Linear;
	props.Mag_Filter = eTextureProperties::Linear;

	LoadTexture("Resources/DirIcon.png", mDirIcon, props);
	LoadTexture("Resources/FileIcon.png", mFileIcon, props);
	LoadTexture("Resources/BackIcon.png", mBackIcon, props);
	LoadTexture("Resources/RefreshIcon.png", mRefreshIcon, props);
	LoadTexture("Resources/ImageIcon.png", mImageIcon, props);
	LoadTexture("Resources/ModelIcon.png", mModelIcon, props);
	LoadTexture("Resources/LuaIcon.png", mLuaIcon, props);
	LoadTexture("Resources/SaveIcon.png", mSaveIcon, props);
	LoadTexture("Resources/TrashIcon.png", mTrashIcon, props);

	GetFilesandFolders(mCurrentDir);
}

void FlawedEngine::cUIManager::LoadTexture(const std::string& path, uint32_t& TextureID, const sTextureProps& props)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		TextureID = mGfxAPI->CreateTexture(width, height, nrChannels, data, props);
	}
	stbi_image_free(data);
}


void OpenFileWithDefaultProgram(const std::string& filepath)
{
	namespace fs = std::filesystem;

	if (!fs::exists(filepath)) {
		std::cerr << "File does not exist: " << filepath << std::endl;
		return;
	}

	std::string command;
#ifdef _WIN32
	command = "start \"\" \"" + filepath + "\"";
#elif __linux__
	// On Linux, use "xdg-open" command
	command = "xdg-open \"" + filepath + "\"";
#elif __APPLE__
	// On macOS, use "open" command
	command = "open \"" + filepath + "\"";
#else
	std::cerr << "Unsupported platform" << std::endl;
	return;
#endif

	int result = std::system(command.c_str());
	if (result != 0) {
		std::cerr << "Failed to open file: " << filepath << std::endl;
	}
}