#include "ShadowSystem.h"

#include <RenderAPI/SpriteComponent.h>
#include <Assets/AssetSystem.h>

using namespace Mani;

void ShadowSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
}

void ShadowSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, ShadowComponent> shadowView(registry);
	for (const EntityId entityId : shadowView)
	{
		ShadowComponent& shadow = *registry.getComponent<ShadowComponent>(entityId);
		Transform& transform = *registry.getComponent<Transform>(entityId);

		const Transform* ownerTransform = registry.getComponent<Transform>(shadow.ownerId);
		if (ownerTransform == nullptr)
		{
			continue;
		}
		transform.scale.x = 3.f;
		transform.scale.z = 1.75f;
		transform.position = glm::vec3(
			ownerTransform->position.x + shadow.xOffset,
			0.5f,
			ownerTransform->position.z + shadow.zOffset
		);
	}
}

Mani::EntityId ShadowSystem::spawnShadow(Mani::EntityRegistry& registry, const Mani::EntityId ownerId)
{
	if (m_assetSystem.expired())
	{
		return INVALID_ID;
	}

	std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();

	EntityId entityId = registry.create();
	registry.addComponent<Transform>(entityId);

	ShadowComponent* shadowComponent = registry.addComponent<ShadowComponent>(entityId);
	shadowComponent->ownerId = ownerId;

	SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(entityId);
	spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Shadow.sprite");

	return entityId;
}

