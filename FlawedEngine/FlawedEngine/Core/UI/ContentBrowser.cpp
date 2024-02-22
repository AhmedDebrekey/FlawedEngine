#include "../UIManager.h"
#include <filesystem>

void FlawedEngine::cUIManager::RenderContentBrowser()
{
	// Get the project path and the current directory
	std::string projectPath = std::filesystem::current_path().string(); // Change this to your actual project path
	static std::string currentDir = projectPath;

	// Begin the content browser panel
	ImGui::Begin("Content Browser");

	// Display the current directory as a text input
	char dirInput[256];
	strcpy(dirInput, currentDir.c_str());
	if (ImGui::InputText("##Directory", dirInput, IM_ARRAYSIZE(dirInput)))
	{
		// If the user edits the text input, change the current directory
		currentDir = std::string(dirInput);
	}

	// Display the parent directory button
	if (currentDir != projectPath)
	{
		// If the current directory is not the root, show a button to go back to the parent directory
		if (ImGui::Button("<-"))
		{
			// Remove the last folder from the current directory
			currentDir = currentDir.substr(0, currentDir.find_last_of("/\\"));
		}
		ImGui::SameLine();
	}

	// Display the refresh button
	if (ImGui::Button("Refresh"))
	{
		// Refresh the content browser by doing nothing
	}

	// Get the list of files and folders in the current directory
	std::vector<std::string> files;
	std::vector<std::string> folders;
	for (const auto& entry : std::filesystem::directory_iterator(currentDir))
	{
		// For each entry in the directory, check if it is a file or a folder
		if (entry.is_directory())
		{
			// If it is a folder, add it to the folders vector
			folders.push_back(entry.path().filename().string());
		}
		else
		{
			// If it is a file, add it to the files vector
			files.push_back(entry.path().filename().string());
		}
	}

	// Sort the files and folders alphabetically
	std::sort(files.begin(), files.end());
	std::sort(folders.begin(), folders.end());

	// Display the folders as buttons
	for (const auto& folder : folders)
	{
		// For each folder, create a button with its name
		if (ImGui::Button(folder.c_str()))
		{
			// If the user clicks on the button, change the current directory to the folder
			currentDir += "/" + folder;
		}
	}

	// Display the files as selectable labels
	static int selected = -1; // The index of the selected file, -1 if none
	for (int i = 0; i < files.size(); i++)
	{
		// For each file, create a selectable label with its name
		if (ImGui::Selectable(files[i].c_str(), i == selected))
		{
			// If the user clicks on the label, change the selected index to the file index
			selected = i;
		}
	}

	// End the content browser panel
	ImGui::End();
}
