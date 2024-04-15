#include "AggroSystem.h"

#include <FactionComponent.h>
#include <SpatialGridSystem.h>

using namespace Mani;

void AggroSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_spatialGrid = systemContainer.initializeDependency<SpatialGridSystem>();
}

void AggroSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (m_spatialGrid.expired())
	{
		return;
	}
	std::shared_ptr<SpatialGridSystem> spatialGrid = m_spatialGrid.lock();

	RegistryView<Transform, FactionComponent, AggroComponent> aggroView(registry);
	for (const EntityId entityId : aggroView)
	{
		const Transform& transform = *registry.getComponent<Transform>(entityId);
		AggroComponent& aggroComponent = *registry.getComponent<AggroComponent>(entityId);
		FactionComponent& factionComponent = *registry.getComponent<FactionComponent>(entityId);

		std::vector<EntityId> idsToCleanUp;

		for (auto& [entityId, aggro] : aggroComponent.aggroMap)
		{
			aggro -= aggroComponent.aggroDecay;
			if (aggro < FLT_EPSILON)
			{
				idsToCleanUp.push_back(entityId);
			}
		}

		for (const EntityId entityId : idsToCleanUp)
		{
			aggroComponent.aggroMap.erase(entityId);
		}

		std::unordered_set<Mani::EntityId> nearbyEntities;
		spatialGrid->findNearby(transform.position, glm::vec2(aggroComponent.aggroRange), nearbyEntities);
		
		float maxAggro = -FLT_MAX;
		for (const EntityId otherEntityId : nearbyEntities)
		{
			if (entityId == otherEntityId || !registry.hasComponent<FactionComponent>(otherEntityId))
			{
				continue;
			}
			
			const FactionComponent& otherFactionComponent = *registry.getComponent<FactionComponent>(otherEntityId);
			if (factionComponent.factionOwnerId == otherFactionComponent.factionOwnerId)
			{
				// they're from the same faction, don't aggro.
				continue;
			}

			const Transform& otherTransform = *registry.getComponent<Transform>(otherEntityId);
			if (glm::distance2(transform.position, otherTransform.position) < aggroComponent.aggroRange * aggroComponent.aggroRange)
			{
				const float distance = glm::distance(transform.position, otherTransform.position);
				aggroComponent.aggroMap[otherEntityId] += aggroComponent.aggroRange - distance;
			}

			if (aggroComponent.aggroMap[otherEntityId] > maxAggro)
			{
				maxAggro = aggroComponent.aggroMap[otherEntityId];
				aggroComponent.aggroTarget = otherEntityId;
			}
		}
	}
}
