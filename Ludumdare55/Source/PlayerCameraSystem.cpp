#include "PlayerCameraSystem.h"

#include <Camera/CameraSystem.h>

#include "PlayerSystem.h"

using namespace Mani;

void PlayerCameraSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	std::shared_ptr<CameraSystem> cameraSystem = systemContainer.initializeDependency<CameraSystem>().lock();
	systemContainer.initializeDependency<PlayerSystem>();

	const EntityId playerEntityId = *RegistryView<Transform, PlayerComponent>(registry).begin();
	const Transform* playerTransform = registry.getComponent<Transform>(playerEntityId);
	if (playerTransform == nullptr)
	{
		return;
	}

	Transform* cameraTransform = cameraSystem->getCameraTransform(registry);
	if (cameraTransform == nullptr)
	{
		return;
	}

	cameraTransform->position = playerTransform->position + glm::vec3(0.f, 1.f, 0.f) * Y_OFFSET * .85f;
}


void PlayerCameraSystem::tick(float deltaTime, EntityRegistry& registry)
{
	const EntityId cameraEntityId = *RegistryView<Transform, CameraComponent>(registry).begin();

	const Transform* playerTransform = registry.getComponent<Transform>(PlayerSystem::PLAYER_ENTITY_ID);
	if (playerTransform == nullptr)
	{
		return;
	}

	Transform* cameraTransform = registry.getComponent<Transform>(cameraEntityId);
	if (cameraTransform == nullptr)
	{
		return;
	}

	cameraTransform->rotation = glm::angleAxis(glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));

	const glm::vec3 target = playerTransform->position + glm::vec3(0.f, 1.f, 0.f) * Y_OFFSET;
	const glm::vec3 delta = target - cameraTransform->position;
	if (glm::length2(delta) <= FLT_EPSILON)
	{
		return;
	}

	cameraTransform->position += glm::normalize(delta) * CAMERA_SPEED * deltaTime;	
}
