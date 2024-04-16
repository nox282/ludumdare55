#pragma once

#include <Core/CoreFwd.h>

namespace Mani
{
	class InputSystem;
	struct InputAction;
}

class GameTimeSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "FloorSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

	float getElapsed() const { return m_elapsed; }
	float getGameTime() const { return m_gameTime; }

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

	void onGameStarted(Mani::EntityRegistry& registry);
	void onGameFinished(Mani::EntityRegistry& registry, bool isWin);
	void disableGameSystems();

	std::weak_ptr<class MovementSystem> m_movementSystem;
	std::weak_ptr<class AIBehaviorSystem> m_aIBehaviorSystem;
	std::weak_ptr<class PlayerSystem> m_playerSystem;
	std::weak_ptr<class DeathSystem> m_deathSystem;
	std::weak_ptr<class AttackSystem> m_attackSystem;
	std::weak_ptr<class NeutralSpawnerSystem> m_neutralSpawnerSystem;
	std::weak_ptr<class PlayerCameraSystem> m_playerCameraSystem;

	std::weak_ptr<Mani::InputSystem> m_inputSystem;
	std::weak_ptr<class PopupSystem> m_popupSystem;

	Mani::EventHandle onActionEventHandle;
	Mani::EventHandle onBeforePlayerDestroyedHandle;

	const float m_gameTime = 300.0f;
	float m_elapsed = 0.f;
	bool m_gameIsRunning = false;
	bool m_gameHasStarted = false;

	const std::string START_GAME = "START_GAME";
	const std::string RESTART_GAME = "RESTART_GAME";
	const std::string END_GAME = "END_GAME";

	void onActionEvent(uint32_t userId, const Mani::InputAction& inputAction);
	void onBeforePlayerDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId);
};