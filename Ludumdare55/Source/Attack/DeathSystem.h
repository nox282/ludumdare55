#pragma once

#include <Core/CoreFwd.h>

class DeathSystem : public Mani::SystemBase
{
public:

	Mani::Event<Mani::EntityRegistry& /*registry*/, const Mani::EntityId /*entityId*/> onBeforeMinionDestroyed;
	Mani::Event<Mani::EntityRegistry& /*registry*/, const Mani::EntityId /*entityId*/> onBeforePlayerDestroyed;

	virtual std::string_view getName() const override { return "HealthSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;
};