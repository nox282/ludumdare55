#include "AggroSystem.h"

using namespace Mani;

void AggroSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, AggroComponent> aggroView(registry);
	for (const EntityId entityId : aggroView)
	{
		const Transform& transform = *registry.getComponent<Transform>(entityId);
		AggroComponent& aggroComponent = *registry.getComponent<AggroComponent>(entityId);

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

		for (const EntityId otherEntityId : aggroView)
		{
			if (entityId == otherEntityId)
			{
				continue;
			}

			const Transform& otherTransform = *registry.getComponent<Transform>(otherEntityId);
			if (glm::distance2(transform.position, otherTransform.position) < aggroComponent.aggroRange * aggroComponent.aggroRange)
			{
				const float distance = glm::distance(transform.position, otherTransform.position);
				aggroComponent.aggroMap[otherEntityId] += aggroComponent.aggroRange - distance;
			}
		}
	}
}
