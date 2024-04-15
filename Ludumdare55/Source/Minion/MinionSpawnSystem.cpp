#include "MinionSpawnSystem.h"

#include <Assets/AssetSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/SpriteComponent.h>

#include <Minion/AIBehaviorSystem.h>
#include <Minion/MovementSystem.h>
#include <Minion/AggroSystem.h>
#include <Minion/MinionComponent.h>
#include <PlayerSystem.h>
#include <CollisionSystem.h>

#include <Attack/AttackSystem.h>
#include <Attack/DeathSystem.h>
#include <Attack/HealthSystem.h>
#include <Attack/HealthComponent.h>

#include <FactionComponent.h>
#include <SpatialGridSystem.h>

using namespace Mani;

void MinionSpawnSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_assetSystem = systemContainer.initializeDependency<AssetSystem>();
	m_deathSystem = systemContainer.initializeDependency<DeathSystem>();
	std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();

	onMinionDestroyedHandle = deathSystem->onBeforeMinionDestroyed.subscribe(std::bind_front(&MinionSpawnSystem::onBeforeMinionDestroyed, this));
}

void MinionSpawnSystem::onDeinitialize(EntityRegistry& registry)
{
	if (!m_deathSystem.expired())
	{
		std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();
		deathSystem->onBeforeMinionDestroyed.unsubscribe(onMinionDestroyedHandle);
	}
}

void MinionSpawnSystem::tick(float deltaTime, EntityRegistry& registry)
{
}

EntityId MinionSpawnSystem::spawnMinion(EntityRegistry& registry, const MinionTemplate& minionTemplate, EntityId ownerId, const glm::vec3& spawnPosition)
{
	if (m_assetSystem.expired())
	{
		return INVALID_ID;
	}

	std::shared_ptr<AssetSystem> assetSystem = m_assetSystem.lock();

	const EntityId minionEntityId = registry.create();

	SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(minionEntityId);
	spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>(minionTemplate.spritePath);
	
	Transform* transform = registry.addComponent<Transform>(minionEntityId);
	transform->position = glm::vec3(spawnPosition.x, 1.0f, spawnPosition.z);
	transform->scale = glm::vec3(1.f, 1.f, 1.f) * minionTemplate.scale;

	registry.addComponent<AIBehaviorComponent>(minionEntityId);
	
	FactionComponent* factionComponent = registry.addComponent<FactionComponent>(minionEntityId);
	factionComponent->factionOwnerId = ownerId;

	registry.addComponent<MovementComponent>(minionEntityId);
	registry.addComponent<AvoidanceSourceComponent>(minionEntityId);
	registry.addComponent<AggroComponent>(minionEntityId);

	SphereCollisionComponent* sphereCollision = registry.addComponent<SphereCollisionComponent>(minionEntityId);
	sphereCollision->centerOffset = minionTemplate.collision.centerOffset;
	sphereCollision->radius = minionTemplate.collision.radius;
	sphereCollision->priotity = minionTemplate.collision.priotity;

	AttackComponent* attackComponent = registry.addComponent<AttackComponent>(minionEntityId);
	attackComponent->attackRange = minionTemplate.attack.attackRange;
	attackComponent->damage = minionTemplate.attack.damage;
	attackComponent->attackCooldown = minionTemplate.attack.attackCooldown;
	attackComponent->attackCooldownElapsed = minionTemplate.attack.attackCooldownElapsed;

	HealthComponent* healthComponent = registry.addComponent<HealthComponent>(minionEntityId);
	healthComponent->health = minionTemplate.health.health;

	registry.addComponent<HurtComponent>(minionEntityId);

	MinionComponent* minionComponent = registry.addComponent<MinionComponent>(minionEntityId);
	minionComponent->minionTemplate = minionTemplate;

	SpatialGridClientComponent* spatialGridClient = registry.addComponent<SpatialGridClientComponent>(minionEntityId);
	spatialGridClient->boxExtents = glm::vec2(minionTemplate.collision.radius);
	return minionEntityId;
}

void MinionSpawnSystem::onBeforeMinionDestroyed(Mani::EntityRegistry& registry, const EntityId entityId)
{
	const FactionComponent* faction = registry.getComponent<FactionComponent>(entityId);
	if (faction == nullptr)
	{
		return;
	}

	if (faction->factionOwnerId == PlayerSystem::PLAYER_ENTITY_ID)
	{
		m_playerMinion--;
		return;
	}

	if (m_playerMinion >= PlayerSystem::MINION_CAP)
	{
		MANI_LOG(Log, "Minion cap hit");
		return;
	}
	
	glm::vec3 spawnPosition = glm::vec3(0.f);
	if (const Transform* transform = registry.getComponent<Transform>(PlayerSystem::PLAYER_ENTITY_ID))
	{
		spawnPosition = transform->position;
	}

	if (const MinionComponent* minion = registry.getComponent<MinionComponent>(entityId))
	{
		spawnMinion(registry, minion->minionTemplate, PlayerSystem::PLAYER_ENTITY_ID, spawnPosition);
		m_playerMinion++;
	}
}
