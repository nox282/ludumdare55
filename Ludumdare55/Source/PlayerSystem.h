#pragma once

#include <Core/CoreFwd.h>
#include <memory>

namespace Mani
{
	class InputSystem;
}

struct PlayerComponent
{
	float speed = 10.f;
};

class PlayerSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "PlayerSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	static Mani::EntityId PLAYER_ENTITY_ID;
	static const uint8_t MINION_CAP;
protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<Mani::InputSystem> m_inputSystem;

	const std::string MOVE_ACTION = "PlayerMove";
};