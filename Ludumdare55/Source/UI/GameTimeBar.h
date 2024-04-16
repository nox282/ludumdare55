#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class AssetSystem;
	class CameraSystem;
}

struct GameTimeBarComponent
{
	float ratio = 0.f;
};

struct GameTimeBarShadowComponent
{
	Mani::EntityId ownerId = Mani::INVALID_ID;
};

class GameTimeBarSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "GameTimeBarSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;

private:
	std::weak_ptr<class GameTimeSystem> m_gameTimeSystem;
	std::weak_ptr<class Mani::CameraSystem> m_cameraSystem;

	void updateTimeBar(GameTimeBarComponent& timeBar) const;
};