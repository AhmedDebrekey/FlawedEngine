#include "../UIManager.h"

void FlawedEngine::cUIManager::SelectEntity()
{
	if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_MouseLeft))
	{
		ImVec2 CursorPos = ImGui::GetMousePos();
		// Mouse coordinates in window space
		float mouseX = CursorPos.x;
		float mouseY = CursorPos.y;

		// Projection and view matrices
		glm::mat4 projection = mCamera.Projection();
		glm::mat4 view = mCamera.View();

		// Camera position in world space
		glm::vec3 cameraPos = mCamera.Postion();

		float viewportX = mViewportPos.x; // The x coordinate of the top left corner of the viewport in pixels
		float viewportY = mViewportPos.y; // The y coordinate of the top left corner of the viewport in pixels
		// Viewport dimensions
		int viewportWidth = mViewportSize.x;
		int viewportHeight = mViewportSize.y;

		// Normalize the mouse position to the range [0, 1]
		float normalizedMouseX = (mouseX - viewportX) / viewportWidth;
		float normalizedMouseY = (mouseY - viewportY) / viewportHeight;

		// Convert the normalized mouse position to clip space
		glm::vec4 clipSpacePosition(
			2.0f * normalizedMouseX - 1.0f,
			1.0f - 2.0f * normalizedMouseY,
			0.1f,
			1.0f
		);

		// Convert the clip space position to eye space
		glm::mat4 inverseProjectionMatrix = glm::inverse(projection);
		glm::vec4 eyeSpacePosition = inverseProjectionMatrix * clipSpacePosition;

		// Convert the eye space position to world space
		glm::mat4 inverseViewMatrix = glm::inverse(view);
		glm::dvec4 worldSpacePosition = inverseViewMatrix * eyeSpacePosition;

		// Convert the world space position from homogeneous coordinates to Cartesian coordinates
		worldSpacePosition /= worldSpacePosition.w;

		// Convert the world space position from a dvec4 to a vec3
		glm::vec3 worldCoords(worldSpacePosition);

		glm::vec3 rayDir = worldCoords - cameraPos;
		rayDir = glm::normalize(rayDir);

		glm::vec3 RayTo = cameraPos + (50.f * rayDir);

		btDynamicsWorld* dynamicWorld = (btDynamicsWorld*)mPhysicsWorld;

		btVector3 RayBegin(worldCoords.x, worldCoords.y, worldCoords.z);
		btVector3 RayEnd(RayTo.x, RayTo.y, RayTo.z);

		btCollisionWorld::ClosestRayResultCallback rayCallback(RayBegin, RayEnd);
		dynamicWorld->rayTest(RayBegin, RayEnd, rayCallback);
		if (mObjectMan.mMousePicking)
		{
			if (rayCallback.hasHit())
			{
				// The ray intersected an object
				const btCollisionObject* object = rayCallback.m_collisionObject;
				btVector3 intersectionPoint = rayCallback.m_hitPointWorld;
				btVector3 intersectionNormal = rayCallback.m_hitNormalWorld;

				const char* Name = static_cast<const char*>(object->getCollisionShape()->getUserPointer());
				mSelectedEntity = Name;
			}
		}
		/*
		{
			ObjectMan->AddObject(Cube, "Ray Start");
			auto Entity = ObjectMan->GetObjectByName("Ray Start");
			sModel EntityModel = Entity->GetModel();
			EntityModel.Translation = worldCoords;
			EntityModel.Scale = glm::vec3(0.05);
			Entity->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
			Entity->ModelTransform(EntityModel);
		}
		{
			ObjectMan->AddObject(Cube, "Ray end");
			auto Entity = ObjectMan->GetObjectByName("Ray end");
			sModel EntityModel = Entity->GetModel();
			EntityModel.Translation = RayTo;
			EntityModel.Scale = glm::vec3(0.05);
			Entity->SetColor(glm::vec3(1.0f, 0.0f, 0.0f));
			Entity->ModelTransform(EntityModel);
		}
		*/
	}
}