#include "HealthbarSystem.h"

#include <Assets/AssetSystem.h>

#include <RenderAPI/SpriteComponent.h>

#include <Attack/HealthComponent.h>

using namespace Mani;

void HealthbarSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
}

void HealthbarSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, HealthbarComponent> healthbarView(registry);
	for (const EntityId entityId : healthbarView)
	{
		HealthbarComponent& healthbar = *registry.getComponent<HealthbarComponent>(entityId);
		Transform& transform = *registry.getComponent<Transform>(entityId);

		const HealthComponent* healthComponent = registry.getComponent<HealthComponent>(healthbar.ownerId);
		if (healthComponent == nullptr)
		{
			continue;
		}

		const Transform* ownerTransform = registry.getComponent<Transform>(healthbar.ownerId);
		if (ownerTransform == nullptr)
		{
			continue;
		}

		const float delta = healthComponent->health - healthbar.currentValue;
		const float sign = (delta > 0.f) ? 1.f : (delta < 0.f) ? -1.f : 0.f;
		healthbar.ratio = healthComponent->health / healthComponent->maxHealth;

		healthbar.targetValue = healthComponent->health;
		healthbar.currentValue += sign * healthbar.speed * deltaTime;

		transform.scale.x = -healthbar.ratio * 5.f;
		transform.position = glm::vec3(
			ownerTransform->position.x + healthbar.horizontalOffset,
			3.f,
			ownerTransform->position.z + healthbar.verticalOffset
		);	
	}

	RegistryView<Transform, HealthbarShadowComponent> healthbarShadowView(registry);
	for (const EntityId entityId : healthbarShadowView)
	{
		HealthbarShadowComponent& healthbar = *registry.getComponent<HealthbarShadowComponent>(entityId);
		Transform& transform = *registry.getComponent<Transform>(entityId);

		const Transform* ownerTransform = registry.getComponent<Transform>(healthbar.ownerId);
		if (ownerTransform == nullptr)
		{
			continue;
		}
		
		transform.scale.x = -5.f;
		transform.scale.z = 2.f;
		transform.position = glm::vec3(
			ownerTransform->position.x + healthbar.horizontalOffset,
			2.f,
			ownerTransform->position.z + healthbar.verticalOffset
		);
	}
}

EntityId HealthbarSystem::spawnHealthbar(Mani::EntityRegistry& registry, const EntityId ownerId)
{
	if (m_assetSystem.expired())
	{
		return INVALID_ID;
	}

	const HealthComponent* healthComponent = registry.getComponent<HealthComponent>(ownerId);
	if (healthComponent == nullptr)
	{
		return INVALID_ID;
	}

	std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();

	EntityId entityId = registry.create();
	registry.addComponent<Transform>(entityId);
	
	HealthbarComponent* healthbar = registry.addComponent<HealthbarComponent>(entityId);
	healthbar->ownerId = ownerId;
	healthbar->currentValue = healthComponent->health;
	healthbar->targetValue = healthComponent->health;
	healthbar->ratio = healthComponent->health / healthComponent->maxHealth;

	SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(entityId);
	spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Healthbar.sprite");

	EntityId entityId2 = registry.create();
	registry.addComponent<Transform>(entityId2);

	HealthbarShadowComponent* healthbarShadow = registry.addComponent<HealthbarShadowComponent>(entityId2);
	healthbarShadow->ownerId = entityId;

	SpriteComponent* spriteComponent2 = registry.addComponent<SpriteComponent>(entityId2);
	spriteComponent2->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Healthbar.sprite");
	spriteComponent2->color = glm::vec4(0.f, 0.f, 0.f, 1.f);

	return entityId;
}
