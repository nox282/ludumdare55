#pragma once

#include <Core/CoreFwd.h>
#include <unordered_map>
#include <unordered_set>

struct SpatialGridConfig
{
	float cellSize = 15.f;
	glm::vec2 gridExtents = glm::vec2(25'000.f, 25'000.f);
};

namespace SpatialGrid
{
	using Indices = std::pair<uint32_t, uint32_t>;
}

struct SpatialGridClientComponent 
{
	glm::vec2 boxExtents;
	SpatialGrid::Indices minIndices;
	SpatialGrid::Indices maxIndices;
};

class SpatialGridSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "SpatialGridSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }
	virtual Mani::ETickGroup getTickGroup() const override { return Mani::ETickGroup::PreTick; };

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	void findNearby(const glm::vec3& position, const glm::vec2& boxExtents, std::unordered_set<Mani::EntityId>& entities);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	SpatialGridConfig m_config;
	std::weak_ptr<class DeathSystem> m_deathSystem;
	std::unordered_map<uint64_t, std::unordered_set<Mani::EntityId>> m_grid;

	Mani::EventHandle onMinionDestroyedHandled;

	static uint64_t toKey(uint32_t xi, uint32_t yi);
	SpatialGrid::Indices getCellIndices(float x, float y) const;
	void insert(const Mani::EntityId entityId, const Mani::Transform& transform, SpatialGridClientComponent& client);
	void remove(const Mani::EntityId entityId, const SpatialGridClientComponent& client);

	void getCellMinMaxIndices(float x, float y, const glm::vec2& boxExtents, SpatialGrid::Indices& outMin, SpatialGrid::Indices& outMax) const;

	void onBeforeMinionDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId);
};