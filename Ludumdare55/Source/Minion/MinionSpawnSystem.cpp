#include "MinionSpawnSystem.h"

#include <Assets/AssetSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/SpriteComponent.h>

#include <Minion/AIBehaviorSystem.h>
#include <Minion/MovementSystem.h>
#include <Minion/AggroSystem.h>
#include <PlayerSystem.h>
#include <CollisionSystem.h>

using namespace Mani;

void MinionSpawnSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();

	for (uint32_t i = 0; i < 50; ++i)
	{
		createMinion(registry, "Ludumdare55/Assets/Sprites/Minion_Eye.sprite", PlayerSystem::PLAYER_ENTITY_ID);
	}

	for (uint32_t i = 0; i < 50; ++i)
	{
		createMinion(registry, "Ludumdare55/Assets/Sprites/Minion_Eye.sprite", INVALID_ID);
	}
}

void MinionSpawnSystem::onDeinitialize(EntityRegistry& registry)
{
}

void MinionSpawnSystem::tick(float deltaTime, EntityRegistry& registry)
{
}

EntityId MinionSpawnSystem::createMinion(EntityRegistry& registry, const std::filesystem::path& spritePath, EntityId ownerId)
{
	if (m_assetSystem.expired())
	{
		return INVALID_ID;
	}

	std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();

	const EntityId minionEntityId = registry.create();

	SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(minionEntityId);
	spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>(spritePath);
	
	Transform* transform = registry.addComponent<Transform>(minionEntityId);
	transform->position = glm::vec3(0.f, 1.0f, 0.f);
	transform->scale = glm::vec3(1.f, 1.f, 1.f);

	AIBehaviorComponent* aiBehaviorComponent = registry.addComponent<AIBehaviorComponent>(minionEntityId);
	aiBehaviorComponent->ownerId = ownerId;

	registry.addComponent<MovementComponent>(minionEntityId);
	registry.addComponent<AvoidanceSourceComponent>(minionEntityId);
	registry.addComponent<AggroComponent>(minionEntityId);

	SphereCollisionComponent* sphereCollision = registry.addComponent<SphereCollisionComponent>(minionEntityId);
	sphereCollision->radius = .5f;

	return minionEntityId;
}