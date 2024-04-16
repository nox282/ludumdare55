#include "DeathSystem.h"

#include <Attack/HealthComponent.h>
#include <Minion/MinionComponent.h>

#include <PlayerSystem.h>

using namespace Mani;

void DeathSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	std::vector<EntityId> entitiesToDestroy;
	RegistryView<HealthComponent> healthView(registry);
	for (const EntityId entityId : healthView)
	{
		const HealthComponent& healthComponent = *registry.getComponent<HealthComponent>(entityId);
		if (healthComponent.health <= 0.f)
		{
			entitiesToDestroy.push_back(entityId);
		}
	}

	for (const EntityId entityId : entitiesToDestroy)
	{
		if (registry.hasComponent<MinionComponent>(entityId))
		{
			onBeforeMinionDestroyed.broadcast(registry, entityId);
		}

		if (registry.hasComponent<PlayerComponent>(entityId))
		{
			onBeforePlayerDestroyed.broadcast(registry, entityId);
		}

		registry.destroy(entityId);
	}
}
