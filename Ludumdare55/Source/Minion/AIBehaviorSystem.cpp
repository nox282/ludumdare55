#include "AIBehaviorSystem.h"

#include <Utils/MathUtils.h>

#include <glm/fwd.hpp>
#include <glm/gtc/random.hpp>

#include <RenderAPI/SpriteComponent.h>

#include <FactionComponent.h>

#include <Minion/AggroSystem.h>
#include <Minion/MovementSystem.h>

#include <PlayerSystem.h>

using namespace Mani;

void AIBehaviorSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, FactionComponent, AIBehaviorComponent> agentView(registry);
	for (const EntityId entityId : agentView)
	{
		FactionComponent& factionComponent = *registry.getComponent<FactionComponent>(entityId);
		if (factionComponent.factionOwnerId != INVALID_ID)
		{
			controlledAgentTick(deltaTime, registry, entityId);
		}
		else
		{
			neutralAgentTick(deltaTime, registry, entityId);
		}
	}
}

bool AIBehaviorSystem::aggroAgentTick(float deltaTime, Mani::EntityRegistry& registry, Mani::EntityId entityId)
{
	Transform& transform = *registry.getComponent<Transform>(entityId);
	AIBehaviorComponent& behaviorComponent = *registry.getComponent<AIBehaviorComponent>(entityId);
	FactionComponent& factionComponent = *registry.getComponent<FactionComponent>(entityId);
	AggroComponent* aggroComponent = registry.getComponent<AggroComponent>(entityId);

	if (aggroComponent == nullptr)
	{
		return false;
	}
	
	if (Transform* targetTransform = registry.getComponent<Transform>(aggroComponent->aggroTarget))
	{
		// we have an aggro target, do aggro behavior
		if (Transform* ownerTransform = registry.getComponent<Transform>(factionComponent.factionOwnerId))
		{
			// if we have an owner, check if we're in aggro range.
			const float aggroRangeSquared = aggroComponent->aggroRange * aggroComponent->aggroRange;
			if (glm::distance2(ownerTransform->position, targetTransform->position) >= aggroRangeSquared)
			{
				return false;
			}
		}
	
		behaviorComponent.desiredVelocity = MathUtils::normalize(targetTransform->position - transform.position);
		return true;
	}

	return false;
}

void AIBehaviorSystem::controlledAgentTick(float deltaTime, Mani::EntityRegistry& registry, EntityId entityId)
{
	Transform& transform = *registry.getComponent<Transform>(entityId);
	AIBehaviorComponent& behaviorComponent = *registry.getComponent<AIBehaviorComponent>(entityId);
	FactionComponent& factionComponent = *registry.getComponent<FactionComponent>(entityId);

	if (MovementComponent* movementComponent = registry.getComponent<MovementComponent>(entityId))
	{
		movementComponent->maxSpeed = 8.f;
		movementComponent->alignementRatio = .2f;
		movementComponent->coherenceRatio = .1f;
		movementComponent->seperationRatio = .8f;
	}

	Transform& ownerTransform = *registry.getComponent<Transform>(factionComponent.factionOwnerId);
	if (glm::distance2(ownerTransform.position, transform.position) < behaviorComponent.returnToOwnerDistance * behaviorComponent.returnToOwnerDistance)
	{
		if (aggroAgentTick(deltaTime, registry, entityId))
		{
			return;
		}
	}

	if (Transform* ownerTransform = registry.getComponent<Transform>(factionComponent.factionOwnerId))
	{
		behaviorComponent.desiredVelocity = ownerTransform->position - transform.position;
		behaviorComponent.wanderElapsedInSeconds = 0.f;
	}
}

void AIBehaviorSystem::neutralAgentTick(float deltaTime, Mani::EntityRegistry& registry, EntityId entityId)
{
	if (MovementComponent* movementComponent = registry.getComponent<MovementComponent>(entityId))
	{
		movementComponent->maxSpeed = 4.f;
		movementComponent->alignementRatio = 0.f;
		movementComponent->coherenceRatio = 0.f;
		movementComponent->seperationRatio = 1.f;
	}

	if (aggroAgentTick(deltaTime, registry, entityId))
	{
		return;
	}
	
	AIBehaviorComponent& behaviorComponent = *registry.getComponent<AIBehaviorComponent>(entityId);
	const Transform& transform = *registry.getComponent<Transform>(entityId);
	if (const Transform* playerTransform = registry.getComponent<Transform>(PlayerSystem::PLAYER_ENTITY_ID))
	{
		behaviorComponent.desiredVelocity = MathUtils::normalize(playerTransform->position - transform.position);
	}
}
