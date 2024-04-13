#include "PlayerSystem.h"

#include <Inputs/InputSystem.h>

#include <Assets/AssetSystem.h>

#include <RenderAPI/Mesh.h>
#include <RenderAPI/Material.h>
#include <RenderAPI/MeshComponent.h>
#include <RenderAPI/SpriteComponent.h>

#include <CollisionSystem.h>

#include <Minion/MovementSystem.h>

using namespace Mani;

EntityId PlayerSystem::PLAYER_ENTITY_ID = Mani::INVALID_ID;

void PlayerSystem::onInitialize(EntityRegistry& registry, SystemContainer& systemContainer)
{
	m_inputSystem = systemContainer.initializeDependency<InputSystem>();
	std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
	inputSystem->addAction(LOCAL_USERID, MOVE_ACTION);
	inputSystem->addBinding(LOCAL_USERID, MOVE_ACTION, "WASD");

	std::shared_ptr<AssetSystem> assetSystem = systemContainer.initializeDependency<AssetSystem>().lock();

	{
		// player entity
		PLAYER_ENTITY_ID = registry.create();

		SpriteComponent* spriteComponent = registry.addComponent<SpriteComponent>(PLAYER_ENTITY_ID);
		spriteComponent->sprite = assetSystem->loadJsonAsset<Sprite>("Ludumdare55/Assets/Sprites/Main_Character.sprite");

		Transform* transform = registry.addComponent<Transform>(PLAYER_ENTITY_ID);
		transform->position = glm::vec3(0.f, 1.f, 0.f);
		transform->scale = glm::vec3(-5.0f, 5.f, -5.0f);

		registry.addComponent<PlayerComponent>(PLAYER_ENTITY_ID);
		registry.addComponent<AvoidanceSourceComponent>(PLAYER_ENTITY_ID);
		SphereCollisionComponent* sphereCollision = registry.addComponent<SphereCollisionComponent>(PLAYER_ENTITY_ID);
		sphereCollision->radius = 4.f;
		sphereCollision->centerOffset = glm::vec3(-3.0f, 0.f, -3.0f);
		sphereCollision->priotity = UINT8_MAX;
	}
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

	playerTransform->position += glm::normalize(input) * player->speed * deltaTime;
}
