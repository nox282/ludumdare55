#include "MovementSystem.h"

#include <Utils/MathUtils.h>

#include <Minion/AIBehaviorSystem.h>
#include <SpatialGridSystem.h>

using namespace Mani;

void MovementSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_spatialGrid = systemContainer.initializeDependency<SpatialGridSystem>();
}

void MovementSystem::tick(float deltaTime, EntityRegistry& registry)
{
	RegistryView<Transform, MovementComponent> minionView(registry);
	for (const EntityId minionEntityId : minionView)
	{
		Transform& transform = *registry.getComponent<Transform>(minionEntityId);
		MovementComponent& movementComponent = *registry.getComponent<MovementComponent>(minionEntityId);
		
		glm::vec3 desiredVelocity = glm::vec3(0.f);
		if (AIBehaviorComponent* behaviorComponent = registry.getComponent<AIBehaviorComponent>(minionEntityId))
		{
			desiredVelocity = MathUtils::normalize(behaviorComponent->desiredVelocity);
		}

		const glm::vec3 coherence = movementComponent.coherenceRatio <= FLT_EPSILON ? glm::vec3(0.f) :
			MathUtils::normalize(doCoherence(registry, minionEntityId, transform, movementComponent));
		const glm::vec3 separation = movementComponent.seperationRatio <= FLT_EPSILON ? glm::vec3(0.f) :
			MathUtils::normalize(doSeparation(registry, minionEntityId, transform, movementComponent));
		const glm::vec3 alignement = movementComponent.alignementRatio <= FLT_EPSILON ? glm::vec3(0.f) :
			MathUtils::normalize(doAlignement(registry, minionEntityId, transform, movementComponent));

		glm::vec3 acceleration = desiredVelocity +
			(coherence * movementComponent.coherenceRatio) + 
			(separation * movementComponent.seperationRatio) +
			(alignement * movementComponent.alignementRatio);
		acceleration.y = 0.f;
		
		movementComponent.velocity += acceleration;
		movementComponent.velocity = glm::clamp(movementComponent.velocity, -movementComponent.maxSpeed, movementComponent.maxSpeed);
		transform.position += movementComponent.velocity * deltaTime;
	}
}

glm::vec3 MovementSystem::doCoherence(EntityRegistry& registry, const EntityId entityId, const Transform& transform, const MovementComponent& movementComponent) const
{
	uint32_t entityInCoherenceCount = 0;
	glm::vec3 coherence(0.f);
	if (!m_spatialGrid.expired())
	{ 
		std::shared_ptr<SpatialGridSystem> spatialGridSystem = m_spatialGrid.lock();
		std::unordered_set<EntityId> nearbyEntities;
		spatialGridSystem->findNearby(transform.position, glm::vec2(movementComponent.coherenceRadius), nearbyEntities);
		for (const EntityId otherEntityId : nearbyEntities)
		{
			if (otherEntityId == entityId || !registry.hasComponent<MovementComponent>(otherEntityId))
			{
				continue;
			}

			const Transform* otherTransform = registry.getComponent<Transform>(otherEntityId);
			if (glm::distance2(transform.position, otherTransform->position) < movementComponent.coherenceRadius * movementComponent.coherenceRadius)
			{
				entityInCoherenceCount++;
				coherence += otherTransform->position;
			}
		}

		if (entityInCoherenceCount > 0)
		{
			coherence /= static_cast<float>(entityInCoherenceCount);
		}
	}

	return coherence;
}

glm::vec3 MovementSystem::doSeparation(EntityRegistry& registry, const EntityId entityId, const Transform& transform, const MovementComponent& movementComponent) const
{
	glm::vec3 separation(0.f);
	if (!m_spatialGrid.expired())
	{
		std::shared_ptr<SpatialGridSystem> spatialGridSystem = m_spatialGrid.lock();
		std::unordered_set<EntityId> nearbyEntities;
		spatialGridSystem->findNearby(transform.position, glm::vec2(movementComponent.seperationRadius), nearbyEntities);
		for (const EntityId otherEntityId : nearbyEntities)
		{
			if (otherEntityId == entityId || !registry.hasComponent<AvoidanceSourceComponent>(otherEntityId))
			{
				continue;
			}

			const Transform* otherTransform = registry.getComponent<Transform>(otherEntityId);

			if (glm::distance2(transform.position, otherTransform->position) < movementComponent.seperationRadius * movementComponent.seperationRadius)
			{
				const glm::vec3 delta = transform.position - otherTransform->position;
				separation = delta - separation;
			}
		}
	}
	return separation;
}

glm::vec3 MovementSystem::doAlignement(EntityRegistry& registry, const EntityId entityId, const Transform& transform, const MovementComponent& movementComponent) const
{
	uint32_t entityCount = 0;
	glm::vec3 alignement(0.f);
	if (!m_spatialGrid.expired())
	{
		std::shared_ptr<SpatialGridSystem> spatialGridSystem = m_spatialGrid.lock();
		std::unordered_set<EntityId> nearbyEntities;
		spatialGridSystem->findNearby(transform.position, glm::vec2(movementComponent.alignementRadius), nearbyEntities);
		for (const EntityId otherEntityId : nearbyEntities)
		{
			if (otherEntityId == entityId || !registry.hasComponent<MovementComponent>(otherEntityId))
			{
				continue;
			}

			const Transform* otherTransform = registry.getComponent<Transform>(otherEntityId);

			entityCount++;
			if (glm::distance2(transform.position, otherTransform->position) < movementComponent.alignementRadius * movementComponent.alignementRadius)
			{
				alignement += otherTransform->position;
			}
		}

		if (entityCount > 0)
		{
			alignement /= static_cast<float>(entityCount);
		}
	}

	return alignement;
}
