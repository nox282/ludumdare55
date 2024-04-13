#pragma once

#include <Core/CoreFwd.h>

class PlayerCameraSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "PlayerCameraSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;

private:
	const float Y_OFFSET = 50.f;
	const float CAMERA_SPEED = 5.f;
};