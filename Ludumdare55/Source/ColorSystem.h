#pragma once

#include <Core/CoreFwd.h>

class ColorSystem : public Mani::SystemBase
{
	virtual std::string_view getName() const override { return "ColorSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;
};