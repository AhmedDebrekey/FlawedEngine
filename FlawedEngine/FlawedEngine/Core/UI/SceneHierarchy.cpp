#include "../UIManager.h"

void FlawedEngine::cUIManager::RenderSceneHierarchy()
{
	ImGui::Begin("Scene Hierarchy");
	static int Cubes = 0;
	static int Spheres = 0;
	static int Cones = 0;
	static int Toruses = 0;
	static int Triangles = 0;
	static int Lights = 0;

	for (auto& Object : *ObjectMan->GetObjectsPointer())
	{
		auto Entity = Object.second;
		if (ImGui::Selectable(Object.first.c_str(), (Object.first == mSelectedEntity) ? true : false))
			mSelectedEntity = Object.first;
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		mSelectedEntity = "";

	if (ImGui::BeginPopupContextWindow("Add Entity", 1))
	{
		if (ImGui::MenuItem("Point Light"))
		{
			char buffer[20];
			sprintf_s(buffer, "Light(%i)", Lights);
			ObjectMan->AddObject(PointLight, buffer);
			mSelectedEntity = buffer;
			Lights++;
		}

		if (ImGui::MenuItem("Cube"))
		{
			char buffer[20];
			sprintf_s(buffer, "Cube(%i)", Cubes);
			ObjectMan->AddObject(Cube, buffer);
			mSelectedEntity = buffer;
			Cubes++;
		}

		if (ImGui::MenuItem("Sphere"))
		{
			char buffer[20];
			sprintf_s(buffer, "Sphere(%i)", Spheres);
			ObjectMan->AddObject(Sphere, buffer);
			mSelectedEntity = buffer;
			Spheres++;
		}

		if (ImGui::MenuItem("Cone"))
		{
			char buffer[20];
			sprintf_s(buffer, "Cone(%i)", Cones);
			ObjectMan->AddObject(Cone, buffer);
			mSelectedEntity = buffer;
			Cones++;
		}

		if (ImGui::MenuItem("Torus"))
		{
			char buffer[20];
			sprintf_s(buffer, "Torus(%i)", Toruses);
			ObjectMan->AddObject(Torus, buffer);
			mSelectedEntity = buffer;
			Toruses++;
		} //Left out the Trianlge for no reason, It was just a proof of concept.
		// later could be used as a billbord, (rectangles ofc)

		ImGui::EndPopup();
	}
	ImGui::End();
}