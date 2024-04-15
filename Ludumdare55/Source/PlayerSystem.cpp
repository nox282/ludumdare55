#include "PlayerSystem.h"

#include <Inputs/InputSystem.h>

#include <Assets/AssetSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/SpriteComponent.h>

#include <CollisionSystem.h>

#include <Minion/MovementSystem.h>
#include <Minion/AggroSystem.h>

#include <Attack/AttackSystem.h>
#include <Attack/HealthSystem.h>
#include <Attack/HealthComponent.h>

#include <FactionComponent.h>
#include <SpatialGridSystem.h>
#include <CollisionSystem.h>

#include <UI/HealthbarSystem.h>
#include <ShadowSystem.h>

using namespace Mani;

EntityId PlayerSystem::PLAYER_ENTITY_ID = Mani::INVALID_ID;
const uint8_t PlayerSystem::MINION_CAP = 50;

void PlayerSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_inputSystem = systemContainer.initializeDependency<InputSystem>();
	std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
	inputSystem->addAction(LOCAL_USERID, MOVE_ACTION);
	inputSystem->addBinding(LOCAL_USERID, MOVE_ACTION, "WASD");

	std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();

	{
		const float PLAYER_RADIUS = 4.f;

		// player entity
		PLAYER_ENTITY_ID = registry.create();

		SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(PLAYER_ENTITY_ID);
		spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Character_Nox_Idle.sprite");

		Transform* transform = registry.addComponent<Transform>(PLAYER_ENTITY_ID);
		transform->position = glm::vec3(0.f, 1.f, 0.f);
		transform->scale = glm::vec3(5.0f, 5.0f, 5.0f);

		registry.addComponent<PlayerComponent>(PLAYER_ENTITY_ID);
		registry.addComponent<AvoidanceSourceComponent>(PLAYER_ENTITY_ID);

		SphereCollisionComponent* sphereCollision = registry.addComponent<SphereCollisionComponent>(PLAYER_ENTITY_ID);
		sphereCollision->radius = 3.0f;
		sphereCollision->centerOffset = glm::vec3(1.0f, 0.f, 1.f);
		sphereCollision->priotity = UINT8_MAX;

		FactionComponent* factionComponent = registry.addComponent<FactionComponent>(PLAYER_ENTITY_ID);
		factionComponent->factionOwnerId = PLAYER_ENTITY_ID;

		AttackComponent* attackComponent = registry.addComponent<AttackComponent>(PLAYER_ENTITY_ID);
		attackComponent->damage = 15.f;
		attackComponent->attackRange = 7.5f;
		attackComponent->attackCooldown = 1.f;

		registry.addComponent<HurtComponent>(PLAYER_ENTITY_ID);
		HealthComponent* healthComponent = registry.addComponent<HealthComponent>(PLAYER_ENTITY_ID);
		healthComponent->health = 300.f;
		healthComponent->maxHealth = 300.f;

		SpatialGridClientComponent* spatialGridClient = registry.addComponent<SpatialGridClientComponent>(PLAYER_ENTITY_ID);
		spatialGridClient->boxExtents = glm::vec2(PLAYER_RADIUS, PLAYER_RADIUS);
	}

	std::shared_ptr<HealthbarSystem> healthbarSystem = systemContainer.initializeDependency<HealthbarSystem>().lock();
	healthbarSystem->spawnHealthbar(registry, PLAYER_ENTITY_ID);

	std::shared_ptr<ShadowSystem> shadowSystem = systemContainer.initializeDependency<ShadowSystem>().lock();
	shadowSystem->spawnShadow(registry, PLAYER_ENTITY_ID);
}

void PlayerSystem::onDeinitialize(EntityRegistry& registry)
{
}

void PlayerSystem::tick(float deltaTime, EntityRegistry& registry)
{
	if (m_inputSystem.expired())
	{
		return;
	}
	
	std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
	
	const InputAction* moveAction = inputSystem->getAction(LOCAL_USERID, MOVE_ACTION);
	if (moveAction == nullptr)
	{
		return;
	}

	glm::vec3 input = glm::vec3(moveAction->x, 0.f, moveAction->z);
	if (glm::length2(input) <= FLT_EPSILON)
	{
		return;
	}
	
	PlayerComponent* player = registry.getComponent<PlayerComponent>(PLAYER_ENTITY_ID);
	Transform* playerTransform = registry.getComponent<Transform>(PLAYER_ENTITY_ID);
	if (player != nullptr && playerTransform != nullptr)
	{
		playerTransform->position += glm::normalize(input) * player->speed * deltaTime;
	}
}
