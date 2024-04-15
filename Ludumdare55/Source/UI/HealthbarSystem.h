#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class AssetSystem;
}

struct HealthbarComponent
{
	Mani::EntityId ownerId;
	float currentValue = 0.f;
	float targetValue = 0.f;
	float speed = 5.f;
	float ratio = 0.f;
	float horizontalOffset = 5.f;
	float verticalOffset = 5.f;
};

struct HealthbarShadowComponent
{
	Mani::EntityId ownerId;
	float horizontalOffset = .1f;
	float verticalOffset = -.08f;
};

class HealthbarSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "HealthbarSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	Mani::EntityId spawnHealthbar(Mani::EntityRegistry& registry, const Mani::EntityId ownerId);

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;

private:
	std::weak_ptr<Mani::AssetSystem> m_assetSystem;
};