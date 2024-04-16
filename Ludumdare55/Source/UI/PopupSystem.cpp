#include "PopupSystem.h"

#include <Assets/AssetSystem.h>
#include <RenderAPI/SpriteComponent.h>
#include <Camera/CameraSystem.h>

#include <Utils/MathUtils.h>

using namespace Mani;

void PopupSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
	m_cameraSystem = systemContainer.initializeDependency<CameraSystem>();
}

void PopupSystem::SpawnPopupAsync(const Request& request)
{
	m_queue.push_back(request);
}

void PopupSystem::ClearAllPopupsAsync(Mani::EntityRegistry& registry)
{
	for (const EntityId entityId : m_popups)
	{
		PopupComponent& popup = *registry.getComponent<PopupComponent>(entityId);
		popup.isAlive = false;

	}
}

void PopupSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (m_cameraSystem.expired())
	{
		return;
	}

	std::shared_ptr<CameraSystem> cameraSystem = m_cameraSystem.lock();
	Transform* cameraTransform = cameraSystem->getCameraTransform(registry);

	if (!m_queue.empty() && !m_assetSystem.expired())
	{
		std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();

		for (const Request& request : m_queue)
		{
			const EntityId popupEntityId = registry.create();

			Transform* transform = registry.addComponent<Transform>(popupEntityId);
			transform->position = cameraTransform->position + request.startPosition;
			transform->position.y = 11.f;
			transform->scale = request.scale;

			PopupComponent* popup = registry.addComponent<PopupComponent>(popupEntityId);
			popup->startPosition = request.startPosition;
			popup->finishPosition = request.finishPosition;
			popup->speed = request.speed;
			popup->isAlive = true;

			SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(popupEntityId);
			spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>(request.spritePath);
			m_popups.insert(popupEntityId);
		}

		m_queue.clear();
	}

	std::vector<EntityId> entitiesToDestroy;
	RegistryView<Transform, PopupComponent> popupView(registry);
	for (const EntityId entityId: popupView)
	{
		Transform& transform = *registry.getComponent<Transform>(entityId);
		PopupComponent& popup = *registry.getComponent<PopupComponent>(entityId);

		if (!popup.isAlive)
		{
			popup.elapsed += deltaTime;
		}

		if (popup.elapsed > popup.ttl)
		{
			entitiesToDestroy.push_back(entityId);
			continue;
		}

		glm::vec3 trueFinishPosition = cameraTransform->position + (popup.isAlive ? popup.finishPosition : popup.startPosition);
		trueFinishPosition.y = 11.f;

		const glm::vec3 delta = trueFinishPosition - transform.position;
		const float speed = popup.isAlive ? popup.speed : popup.speed;
		const float step = speed * deltaTime;
		if (glm::length2(delta) <= step * step)
		{
			transform.position = trueFinishPosition;
			if (!popup.isAlive)
			{
				entitiesToDestroy.push_back(entityId);
				MANI_LOG(Log, "Popup to destroy {}", entitiesToDestroy.size());
			}
			continue;
		}

		const glm::vec3 direction = MathUtils::normalize(delta);
		transform.position += direction * step;
	}

	for (const EntityId entityId : entitiesToDestroy)
	{
		MANI_LOG(Log, "Popup {} destroyed", entityId);
		registry.destroy(entityId);
		m_popups.erase(entityId);
	}
}