#include "../UIManager.h"
#include <ImGui/imfilebrowser.h>

void FlawedEngine::cUIManager::RenderSceneHierarchy()
{
	ImGui::Begin("Scene Hierarchy");
	static int Cubes = 0;
	static int Spheres = 0;
	static int Cones = 0;
	static int Toruses = 0;
	static int Triangles = 0;
	static int Lights = 0;

	for (auto& Object : *mObjectMan->GetObjectsPointer())
	{
		auto Entity = Object.second;

		glm::vec3 textcolor(40, 140, 170);
		if (!Entity->mShouldRender)
		{
			textcolor = glm::vec3(200, 100, 100);
		}

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(textcolor.r, textcolor.g, textcolor.b, 255));
		if (ImGui::Selectable(Object.first.c_str(), (Object.first == mSelectedEntity) ? true : false))
			mSelectedEntity = Object.first;
		ImGui::PopStyleColor();

	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		mSelectedEntity = "";

	static ImGui::FileBrowser fileDialog;

	if (ImGui::BeginPopupContextWindow("Add Entity", 1))
	{
		if (ImGui::MenuItem("Point Light"))
		{
			char buffer[20];
			sprintf_s(buffer, "Light(%i)", Lights);
			mObjectMan->AddObject(PointLight, buffer);
			mSelectedEntity = buffer;
			Lights++;
		}

		if (ImGui::MenuItem("Cube"))
		{
			char buffer[20];
			sprintf_s(buffer, "Cube(%i)", Cubes);
			mObjectMan->AddObject(Cube, buffer);
			mSelectedEntity = buffer;
			Cubes++;
		}

		if (ImGui::MenuItem("Sphere"))
		{
			char buffer[20];
			sprintf_s(buffer, "Sphere(%i)", Spheres);
			mObjectMan->AddObject(Sphere, buffer);
			mSelectedEntity = buffer;
			Spheres++;
		}

		if (ImGui::MenuItem("Cone"))
		{
			char buffer[20];
			sprintf_s(buffer, "Cone(%i)", Cones);
			mObjectMan->AddObject(Cone, buffer);
			mSelectedEntity = buffer;
			Cones++;
		}

		if (ImGui::MenuItem("Torus"))
		{
			char buffer[20];
			sprintf_s(buffer, "Torus(%i)", Toruses);
			mObjectMan->AddObject(Torus, buffer);
			mSelectedEntity = buffer;
			Toruses++;
		}
		//Left out the Trianlge for no reason, It was just a proof of concept.
		// later could be used as a billbord, (rectangles ofc)

		if (ImGui::MenuItem("Custome"))
			fileDialog.Open();

		ImGui::EndPopup();
	}

	fileDialog.Display();
	static int Objects = 0;
	fileDialog.SetTypeFilters({ ".obj", ".gltf", ".fbx", ".dae" });

	if (fileDialog.HasSelected())
	{
		//std::cout << "Selected filename" << fileDialog.GetSelected().string() << std::endl;
		char buffer[20];
		sprintf_s(buffer, "Object(%i)", Objects);
		mObjectMan->LoadObject(fileDialog.GetSelected().string().c_str(), buffer);
		mSelectedEntity = buffer;
		Objects++;
		fileDialog.ClearSelected();
	}
	ImGui::End();
}