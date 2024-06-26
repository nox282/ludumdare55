#pragma once

#include <Core/CoreFwd.h>
#include <unordered_map>

struct AggroComponent
{
	float aggroRange = 15.f;
	float aggroDecay = 1.f;
	std::unordered_map<Mani::EntityId, float> aggroMap;
	Mani::EntityId aggroTarget = Mani::INVALID_ID;
};

class AggroSystem : public Mani::SystemBase
{
	virtual std::string_view getName() const override { return "AggroSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;

private:
	std::weak_ptr<class SpatialGridSystem> m_spatialGrid;
};

