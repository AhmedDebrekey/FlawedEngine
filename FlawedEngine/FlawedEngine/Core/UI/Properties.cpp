#include "../UIManager.h"
#include <ImGui/imfilebrowser.h>

void FlawedEngine::cUIManager::RenderProperties()
{
	ImGui::Begin("Properties");
	auto Entity = ObjectMan->GetObjectByName(mSelectedEntity.c_str());
	if (Entity)
	{
		bool ChangeName = ImGui::Button("Name");
		static char NewName[20] = "";
		ImGui::SameLine();
		ImGui::InputTextWithHint(std::string("##Name" + mSelectedEntity).c_str(), "Name", NewName, IM_ARRAYSIZE(NewName));
		if (ImGui::IsItemActive())
		{
			// The text box is  highlighted
			mCamera->DisableInput();
			ImGui::GetIO().WantCaptureKeyboard = true;
		}
		else
		{
			// The text box is not being edited or highlighted
			mCamera->EnableInput();
			ImGui::GetIO().WantCaptureKeyboard = false;
		}
		if (ChangeName)
		{
			if (!((NewName != NULL) && (NewName[0] == '\0')))
			{
				ObjectMan->ChangeName(mSelectedEntity.c_str(), NewName);
				mSelectedEntity = NewName;
			}
		}
		if (Entity->Type == PointLight)
		{
			sModel LightModel = Entity->GetModel();
			DrawVec3("Translation", LightModel.Translation, 0.0f);
			Entity->ModelTransform(LightModel);
			ObjectMan->ChangeLightPosition(mSelectedEntity.c_str(), LightModel.Translation);

			sLight* LightProps = nullptr;
			LightProps = ObjectMan->GetLightProps(mSelectedEntity.c_str());

			ImGui::ColorEdit3(std::string("LightColor:##" + mSelectedEntity).c_str(), &LightProps->ambient.x);
			Entity->SetColor(LightProps->ambient);

			ImGui::ColorEdit3(std::string("LightDiffuse:##" + mSelectedEntity).c_str(), &LightProps->diffuse.x);
			ImGui::ColorEdit3(std::string("LightSpecular:##" + mSelectedEntity).c_str(), &LightProps->specular.x);
			ImGui::DragFloat(std::string("LightConstant:##" + mSelectedEntity).c_str(), &LightProps->constant, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(std::string("LightLinear:##" + mSelectedEntity).c_str(), &LightProps->linear, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(std::string("LightQuadratic:##" + mSelectedEntity).c_str(), &LightProps->quadratic, 0.01f, 0.0f, 1.0f);
		}
		else
		{
			if (!Entity->mPhysics)
			{
				Entity->UnSetPhysics();
				sModel EntityModel = Entity->GetModel();
				DrawVec3("Translation", EntityModel.Translation, 0.0f);
				DrawVec3("Rotation", EntityModel.Rotation, 0.0f);
				DrawVec3("Scale", EntityModel.Scale, 1.0f);
				Entity->ModelTransform(EntityModel);
			}
			else
			{
				btTransform Trans;
				Entity->mRidigBody->getMotionState()->getWorldTransform(Trans);

				//Translation......
				btVector3 Origin = Trans.getOrigin();
				glm::vec3 Translation = glm::vec3(Origin.x(), Origin.y(), Origin.z());
				DrawVec3("Translation", Translation);
				btVector3 FinalTranslation(Translation.x, Translation.y, Translation.z);
				Trans.setOrigin(FinalTranslation);

				//Rotation........
				glm::vec3 untranslation, rotation, unscale;
				ImGuizmo::DecomposeMatrixToComponents(mTmpMatrix, glm::value_ptr(untranslation), glm::value_ptr(rotation), glm::value_ptr(unscale));
				DrawVec3("Rotation", rotation);
				ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(untranslation), glm::value_ptr(rotation), glm::value_ptr(unscale), mTmpMatrix);
				btQuaternion quat = btQuaternion(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
				Trans.setRotation(quat);
				Entity->mRidigBody->getMotionState()->setWorldTransform(Trans);

				//Scale..........
				btVector3 myscale = Entity->mRidigBody->getCollisionShape()->getLocalScaling();
				glm::vec3 scale(myscale.x(), myscale.y(), myscale.z());
				DrawVec3("Scale", scale, 1.0f);
				myscale = btVector3(scale.x, scale.y, scale.z);
				Entity->mRidigBody->getCollisionShape()->setLocalScaling(myscale);

				ImGui::Text("Activation State: %i", Entity->GetActivationState());

				if (!Entity->mDynamic)
				{
					Entity->mRidigBody->setWorldTransform(Trans);
				}
			}

			glm::vec3* EntityColor = Entity->GetColor();
			ImGui::ColorEdit3(std::string("Color:##" + mSelectedEntity).c_str(), &EntityColor->x);

			ImGui::Checkbox(std::string("Physics:##" + mSelectedEntity).c_str(), &Entity->mPhysics);

			if (Entity->mPhysics)
			{
				ImGui::Checkbox(std::string("Dynamic:##" + mSelectedEntity).c_str(), &Entity->mDynamic);
				Entity->SetPhysics(Entity->Type, ObjectMan->GetPhysicsWorld());
				Entity->setDynamic(Entity->mDynamic);
			}
			else
				Entity->mDynamic = false;
		}
		
		if (ImGui::Button("Render"))
		{
			Entity->isInvisible();
		}

		if (ImGui::Button("Remove"))
		{
			ObjectMan->RemoveObject(mSelectedEntity.c_str());
		}

		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete)) { ObjectMan->RemoveObject(mSelectedEntity.c_str()); }

		static ImGui::FileBrowser ScriptDialog;

		{
			if (ImGui::Button("Add Script"))	
				ScriptDialog.Open();

			ScriptDialog.Display();
			if (ScriptDialog.HasSelected())
			{
				std::cout << "Selected filename" << ScriptDialog.GetSelected().string() << std::endl;
				Entity->SetupScripting(ScriptDialog.GetSelected().string().c_str());
				Entity->SendInputToScripting(std::bind(&cUIManager::isKeyDown, this, std::placeholders::_1));
				ScriptDialog.ClearSelected();
			}
		}
		static ImGui::FileBrowser AnimationDialog;

		{
			if (ImGui::Button("Add Animation"))
				AnimationDialog.Open();

			AnimationDialog.Display();
			AnimationDialog.SetTypeFilters({ ".obj", ".gltf", ".fbx", ".dae" });
			if (AnimationDialog.HasSelected())
			{
				std::cout << "Selected filename" << AnimationDialog.GetSelected().string() << std::endl;
				Entity->AddAnimation(AnimationDialog.GetSelected().string().c_str());
				AnimationDialog.ClearSelected();
			}
		}

	}
	ImGui::End();
}