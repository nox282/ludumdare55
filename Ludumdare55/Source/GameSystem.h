#pragma once

#include <Core/CoreFwd.h>

class GameSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "GameSystem"; }

	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;
};