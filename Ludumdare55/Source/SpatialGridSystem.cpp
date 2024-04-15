#include "SpatialGridSystem.h"

#include <Attack/DeathSystem.h>
#include <Minion/MinionTemplate.h>

#include <algorithm>

using namespace Mani;

void SpatialGridSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_deathSystem = systemContainer.initializeDependency<DeathSystem>();
	std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();
	onMinionDestroyedHandled = deathSystem->onBeforeMinionDestroyed.subscribe(std::bind_front(&SpatialGridSystem::onBeforeMinionDestroyed, this));
}

void SpatialGridSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
	if (!m_deathSystem.expired())
	{
		std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();
		deathSystem->onBeforeMinionDestroyed.unsubscribe(onMinionDestroyedHandled);
	}
}

void SpatialGridSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	RegistryView<Transform, SpatialGridClientComponent> gridView(registry);
	for (const EntityId entityId : gridView)
	{
		SpatialGridClientComponent& client = *registry.getComponent<SpatialGridClientComponent>(entityId);
		Transform& transform = *registry.getComponent<Transform>(entityId);
		remove(entityId, client);
		insert(entityId, transform, client);
	}
}

void SpatialGridSystem::findNearby(const glm::vec3& position, const glm::vec2& boxExtents, std::unordered_set<Mani::EntityId>& entities)
{
	SpatialGrid::Indices minIndices;
	SpatialGrid::Indices maxIndices;
	getCellMinMaxIndices(position.x, position.z, boxExtents, minIndices, maxIndices);

	for (uint32_t xi = minIndices.first; xi <= maxIndices.first; xi++)
	{
		for (uint32_t yi = minIndices.second; yi <= maxIndices.second; yi++)
		{
			for (const EntityId idToCopy : m_grid[toKey(xi, yi)])
			{
				entities.insert(idToCopy);
			}
		}
	}
}

uint64_t SpatialGridSystem::toKey(uint32_t xi, uint32_t yi)
{
	return ((uint64_t)yi << 32) | xi;
}

SpatialGrid::Indices SpatialGridSystem::getCellIndices(float x, float y) const
{
	const glm::vec2 delta = glm::vec2(x, y) - (-m_config.gridExtents);

	const uint32_t xi = static_cast<uint32_t>(std::floor(delta.x / m_config.cellSize));
	const uint32_t yi = static_cast<uint32_t>(std::floor(delta.y / m_config.cellSize));

	return { xi, yi };
}

void SpatialGridSystem::insert(const EntityId entityId, const Transform& transform, SpatialGridClientComponent& client)
{
	const float x = transform.position.x;
	const float y = transform.position.z;

	SpatialGrid::Indices minIndices;
	SpatialGrid::Indices maxIndices;
	getCellMinMaxIndices(x, y, client.boxExtents, minIndices, maxIndices);

	client.minIndices = minIndices;
	client.maxIndices = maxIndices;

	for (uint32_t xi = minIndices.first; xi <= maxIndices.first; xi++)
	{
		for (uint32_t yi = minIndices.second; yi <= maxIndices.second; yi++)
		{
			m_grid[toKey(xi, yi)].insert(entityId);
		}
	}
}

void SpatialGridSystem::remove(const Mani::EntityId entityId, const SpatialGridClientComponent& client)
{
	const SpatialGrid::Indices minIndices = client.minIndices;
	const SpatialGrid::Indices maxIndices = client.maxIndices;

	for (uint32_t xi = minIndices.first; xi <= maxIndices.first; xi++)
	{
		for (uint32_t yi = minIndices.second; yi <= maxIndices.second; yi++)
		{
			m_grid[toKey(xi, yi)].erase(entityId);
		}
	}
}

void SpatialGridSystem::getCellMinMaxIndices(float x, float y, const glm::vec2& boxExtents, SpatialGrid::Indices& outMin, SpatialGrid::Indices& outMax) const
{
	outMin = getCellIndices(x, y);
	outMax = getCellIndices(x + boxExtents.x * 2.f, y + boxExtents.y * 2.f);
}

void SpatialGridSystem::onBeforeMinionDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId)
{
	if (SpatialGridClientComponent* client = registry.getComponent<SpatialGridClientComponent>(entityId))
	{
		remove(entityId, *client);
	}	
}
