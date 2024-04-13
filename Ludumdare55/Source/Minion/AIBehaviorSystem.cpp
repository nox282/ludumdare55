#include "AIBehaviorSystem.h"

#include <Utils/MathUtils.h>

#include <glm/fwd.hpp>
#include <glm/gtc/random.hpp>

#include <Minion/AggroSystem.h>

using namespace Mani;

void AIBehaviorSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, AIBehaviorComponent> agentView(registry);
	for (const EntityId entityId : agentView)
	{
		AIBehaviorComponent& behaviorComponent = *registry.getComponent<AIBehaviorComponent>(entityId);
		if (behaviorComponent.ownerId != INVALID_ID)
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
	AggroComponent* aggroComponent = registry.getComponent<AggroComponent>(entityId);

	if (aggroComponent == nullptr)
	{
		return false;
	}
	
	Transform& targetTransform = *registry.getComponent<Transform>(aggroComponent->aggroTarget);
	if (glm::distance2(targetTransform.position, transform.position) >= aggroComponent->aggroRange * aggroComponent->aggroRange)
	{
		return false;
	}

	behaviorComponent.desiredVelocity = MathUtils::normalize(targetTransform.position - transform.position);
	return true;
}

void AIBehaviorSystem::controlledAgentTick(float deltaTime, Mani::EntityRegistry& registry, EntityId entityId)
{
	Transform& transform = *registry.getComponent<Transform>(entityId);
	AIBehaviorComponent& behaviorComponent = *registry.getComponent<AIBehaviorComponent>(entityId);
	
	Transform& ownerTransform = *registry.getComponent<Transform>(behaviorComponent.ownerId);

	if (glm::distance2(ownerTransform.position, transform.position) < behaviorComponent.returnToOwnerDistance * behaviorComponent.returnToOwnerDistance)
	{
		if (aggroAgentTick(deltaTime, registry, entityId))
		{
			return;
		}
	}

	if (Transform* ownerTransform = registry.getComponent<Transform>(behaviorComponent.ownerId))
	{
		behaviorComponent.desiredVelocity = ownerTransform->position - transform.position;
		behaviorComponent.wanderElapsedInSeconds = 0.f;
	}
}

void AIBehaviorSystem::neutralAgentTick(float deltaTime, Mani::EntityRegistry& registry, EntityId entityId)
{
	if (aggroAgentTick(deltaTime, registry, entityId))
	{
		return;
	}

	AIBehaviorComponent& behaviorComponent = *registry.getComponent<AIBehaviorComponent>(entityId);
	behaviorComponent.wanderElapsedInSeconds += deltaTime;
	if (behaviorComponent.wanderElapsedInSeconds >= behaviorComponent.wanderTimeoutInSeconds)
	{
		behaviorComponent.wanderElapsedInSeconds = 0.f;
		behaviorComponent.desiredVelocity = MathUtils::normalize(glm::sphericalRand(1.f));
	}
}
