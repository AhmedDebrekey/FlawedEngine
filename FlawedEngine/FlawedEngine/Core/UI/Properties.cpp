#include "../UIManager.h"
#include <ImGui/imfilebrowser.h>

void FlawedEngine::cUIManager::RenderProperties()
{
	ImGui::Begin("Properties");
	auto Entity = mObjectMan.GetObjectByName(mSelectedEntity.c_str());
	if (Entity)
	{
		bool ChangeName = ImGui::Button("Name");
		static char NewName[20] = "";
		ImGui::SameLine();
		ImGui::InputTextWithHint(std::string("##Name" + mSelectedEntity).c_str(), "Name", NewName, IM_ARRAYSIZE(NewName));
		if (ChangeName)
		{
			if (!((NewName != NULL) && (NewName[0] == '\0')))
			{
				mObjectMan.ChangeName(mSelectedEntity.c_str(), NewName);
				mSelectedEntity = NewName;
			}
		}
		if (Entity->Type == PointLight)
		{
			sModel LightModel = Entity->GetModel();
			DrawVec3("Translation", LightModel.Translation, 0.0f);
			Entity->ModelTransform(LightModel);
			mObjectMan.ChangeLightPosition(mSelectedEntity.c_str(), LightModel.Translation);

			sLight* LightProps = nullptr;
			LightProps = mObjectMan.GetLightProps(mSelectedEntity.c_str());

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
				Entity->mRigidBody->getMotionState()->getWorldTransform(Trans);

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
				Entity->mRigidBody->getMotionState()->setWorldTransform(Trans);

				//Scale..........
				btVector3 myscale = Entity->mRigidBody->getCollisionShape()->getLocalScaling();
				glm::vec3 scale(myscale.x(), myscale.y(), myscale.z());
				DrawVec3("Scale", scale, 1.0f);
				myscale = btVector3(scale.x, scale.y, scale.z);
				Entity->mRigidBody->getCollisionShape()->setLocalScaling(myscale);

				//AngularFactor..........
				static glm::vec3 AngularForce = Entity->mAngularForce;
				DrawVec3("AngularForce", AngularForce, 1.0f);
				Entity->mAngularForce = AngularForce;
				Entity->mRigidBody->setAngularFactor(btVector3(AngularForce.x, AngularForce.y, AngularForce.z));

				glm::vec3 AABBOffset = Entity->mAABBOffset;
				DrawVec3("AABBOffset", AABBOffset);
				Entity->ApplyAABBOffest(AABBOffset);
				

				ImGui::Text("Activation State: %i", Entity->GetActivationState());

				if (!Entity->mDynamic)
				{
					Entity->mRigidBody->setWorldTransform(Trans);
				}

				if (ImGui::Button(std::string("AABB:##" + mSelectedEntity).c_str()))
					Entity->mShowAABB = !Entity->mShowAABB;

				if (Entity->mShowAABB)
				{
					mObjectMan.AddObject(Cube, std::string("AABB" + Entity->mName).c_str());
					auto aabb = mObjectMan.GetObjectByName(std::string("AABB" + Entity->mName).c_str());
					if (aabb)
					{
						auto position = Entity->mAABB.mCenter;
						auto scale = Entity->mAABB.mExtents;
						sModel DefaultModel = { position + Entity->mTransformation.Translation, glm::vec3(0.0f, 0.0f, 0.0f), scale };
						aabb->ModelTransform(DefaultModel);
					}
					
				}
			}

			glm::vec3* EntityColor = Entity->GetColor();
			ImGui::ColorEdit3(std::string("Color:##" + mSelectedEntity).c_str(), &EntityColor->x);

			ImGui::DragFloat(std::string("Reflectivity:##" + mSelectedEntity).c_str(), &Entity->mMaterial.Reflectivity, 0.01f, 0.0f, 1.0f);

			ImGui::Checkbox(std::string("Physics:##" + mSelectedEntity).c_str(), &Entity->mPhysics);

			if (Entity->mPhysics)
			{
				ImGui::Checkbox(std::string("Dynamic:##" + mSelectedEntity).c_str(), &Entity->mDynamic);
				Entity->SetPhysics(Entity->Type, mObjectMan.GetPhysicsWorld());
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
			mObjectMan.RemoveObject(mSelectedEntity.c_str());
		}

		if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete)) { mObjectMan.RemoveObject(mSelectedEntity.c_str()); }

		static ImGui::FileBrowser ScriptDialog;

		{
			PanelRefreshCallback refreshCallback = [&]() {
				Entity->ReloadScript();
			};
			DragDropCallback dragDropCallback = [&]() {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path dataPath(path);
					std::string ext = dataPath.extension().string();
					std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

					if (ext == ".lua")
					{
						std::function<bool(int)> InputFunc = std::bind(&cUIManager::isKeyDown, this, std::placeholders::_1);
						Entity->SetupScripting(dataPath.string().c_str(), InputFunc);
					}
				}
			};

			std::filesystem::path scriptPath= Entity->mScriptPath;

			DrawDragDropPanel("Script", scriptPath.filename().string().c_str(), refreshCallback, dragDropCallback, 75);
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

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path dataPath(path);

					std::string ext = dataPath.extension().string();
					std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

					if (ext == ".fbx" || ext == ".gltf" || ext == ".dae")
					{
						Entity->AddAnimation(dataPath.string().c_str());

					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		static ImGui::FileBrowser ChangeAnimDialog;

		{
			if (ImGui::Button("Change Animation"))
				ChangeAnimDialog.Open();

			ChangeAnimDialog.Display();
			ChangeAnimDialog.SetTypeFilters({ ".obj", ".gltf", ".fbx", ".dae" });

			if (ChangeAnimDialog.HasSelected())
			{
				std::cout << "Selected filename" << ChangeAnimDialog.GetSelected().string() << std::endl;
				Entity->ChangeAnimation(ChangeAnimDialog.GetSelected().string().c_str());
				ChangeAnimDialog.ClearSelected();
			}
		}


	}
	ImGui::End();
}