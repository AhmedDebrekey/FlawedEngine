#include "../UIManager.h"

void FlawedEngine::cUIManager::RenderGizmo()
{
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_T)) { mGizmoType = ImGuizmo::OPERATION::TRANSLATE; }
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R)) { mGizmoType = ImGuizmo::OPERATION::ROTATE; }
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_G)) { mGizmoType = ImGuizmo::OPERATION::SCALE; }

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
	ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

	{
		auto Entity = mObjectMan.GetObjectByName(mSelectedEntity.c_str());
		if (Entity)
		{
			sModel Model = Entity->GetModel();
			ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(Model.Translation), glm::value_ptr(Model.Rotation), glm::value_ptr(Model.Scale), mTmpMatrix);

			glm::mat4* Transform = Entity->GetModelMatrix();
			ImGuizmo::Manipulate(glm::value_ptr(mCamera.View()), glm::value_ptr(mCamera.Projection()),
				(ImGuizmo::OPERATION)mGizmoType, ImGuizmo::LOCAL, mTmpMatrix);

			

			if (ImGuizmo::IsUsing())
			{
				if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Tab))
				{
					static int Copies = 0;

					if (Entity->mIsCostume)
					{
						char buffer[20];
						sprintf_s(buffer, "Copy(%i)", Copies);
						mObjectMan.LoadObject(Entity->mFilePath.c_str(), buffer);
						mSelectedEntity = buffer;
						Copies++;

						auto newEntity = mObjectMan.GetObjectByName(buffer);
						newEntity->mTransformation = Entity->mTransformation;

					}
					else
					{
						char buffer[20];
						sprintf_s(buffer, "Copy(%i)", Copies);
						mObjectMan.AddObject(Entity->Type, buffer);
						mSelectedEntity = buffer;
						Copies++;

						auto newEntity = mObjectMan.GetObjectByName(buffer);
						newEntity->mTransformation = Entity->mTransformation;
					}
				}
				else
				{
					glm::vec3 translation, rotation, scale;
					ImGuizmo::DecomposeMatrixToComponents(mTmpMatrix, glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

					Model.Translation = translation;
					Model.Rotation = rotation;
					Model.Scale = scale;

					if (!Entity->mPhysics)
					{
						Entity->ModelTransform(Model);
					}
					else
					{
						btTransform Trans;
						Entity->mRigidBody->getMotionState()->getWorldTransform(Trans);

						//Translation......
						btVector3 FinalTranslation(translation.x, translation.y, translation.z);
						Trans.setOrigin(FinalTranslation);

						//Rotation........
						btQuaternion quat = btQuaternion(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
						Trans.setRotation(quat);
						Entity->mRigidBody->getMotionState()->setWorldTransform(Trans);

						//Scale..........
						btVector3 myscale = btVector3(scale.x, scale.y, scale.z);
						Entity->mRigidBody->getCollisionShape()->setLocalScaling(myscale);

						if (!Entity->mDynamic)
						{
							Entity->mRigidBody->setWorldTransform(Trans);
						}
						Entity->ModelTransform(Model);
					}
				}
			}
		}
	}
}
