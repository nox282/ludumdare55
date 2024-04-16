#include "GameTimeSystem.h"

#include <Inputs/InputSystem.h>

#include <Minion/MovementSystem.h>
#include <Minion/AIBehaviorSystem.h>

#include <Attack/DeathSystem.h>
#include <Attack/AttackSystem.h>

#include <GameFlow/NeutralSpawnerSystem.h>
#include <GameFlow/GameLoopSystem.h>

#include <PlayerSystem.h>
#include <PlayerCameraSystem.h>
#include <UI/PopupSystem.h>

using namespace Mani;

void GameTimeSystem::onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
{
	m_movementSystem = systemContainer.initializeDependency<MovementSystem>();
	m_aIBehaviorSystem = systemContainer.initializeDependency<AIBehaviorSystem>();
	m_playerSystem = systemContainer.initializeDependency<PlayerSystem>();
	m_deathSystem = systemContainer.initializeDependency<DeathSystem>();
	m_attackSystem = systemContainer.initializeDependency<AttackSystem>();
	m_neutralSpawnerSystem = systemContainer.initializeDependency<NeutralSpawnerSystem>();
	m_playerCameraSystem = systemContainer.initializeDependency<PlayerCameraSystem>();

	m_popupSystem = systemContainer.initializeDependency<PopupSystem>();
	m_inputSystem = systemContainer.initializeDependency<InputSystem>();

	std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
	inputSystem->addAction(LOCAL_USERID, START_GAME);
	inputSystem->addAction(LOCAL_USERID, RESTART_GAME);
	inputSystem->addAction(LOCAL_USERID, END_GAME);

	inputSystem->addBinding(LOCAL_USERID, START_GAME, "W");
	inputSystem->addBinding(LOCAL_USERID, START_GAME, "A");
	inputSystem->addBinding(LOCAL_USERID, START_GAME, "S");
	inputSystem->addBinding(LOCAL_USERID, START_GAME, "D");
	inputSystem->addBinding(LOCAL_USERID, RESTART_GAME, "Space");
	inputSystem->addBinding(LOCAL_USERID, END_GAME, "Escape");

	onActionEventHandle = inputSystem->onActionEvent.subscribe(std::bind_front(&GameTimeSystem::onActionEvent, this));

	disableGameSystems();
	
	std::shared_ptr<PopupSystem> popupSystem = m_popupSystem.lock();
	{
		PopupSystem::Request request;
		request.spritePath = "Ludumdare55/Assets/Sprites/Title.sprite";
		request.startPosition = glm::vec3(8.f, 0.f, 25.0f);
		request.finishPosition = glm::vec3(8.f, 0.f, 5.f);
		request.scale = glm::vec3(-15.f, 15.f, 15.f);
		request.speed = 40.f;

		popupSystem->SpawnPopupAsync(request);
	}

	{
		PopupSystem::Request request;
		request.spritePath = "Ludumdare55/Assets/Sprites/Keys_WASD.sprite";
		request.startPosition = glm::vec3(5.5f, 0.f, -30.0f);
		request.finishPosition = glm::vec3(5.5f, 0.f, -7.f);
		request.scale = glm::vec3(-10.f, 10.f, 10.f);
		request.speed = 40.f;

		popupSystem->SpawnPopupAsync(request);
	}

	onBeforePlayerDestroyedHandle = m_deathSystem.lock()->onBeforePlayerDestroyed.subscribe(std::bind_front(&GameTimeSystem::onBeforePlayerDestroyed, this));
}

void GameTimeSystem::onDeinitialize(Mani::EntityRegistry& registry)
{
	if (!m_inputSystem.expired())
	{
		std::shared_ptr<InputSystem> inputSystem = m_inputSystem.lock();
		inputSystem->onActionEvent.unsubscribe(onActionEventHandle);
	}

	if (!m_deathSystem.expired())
	{
		std::shared_ptr<DeathSystem> deathSystem = m_deathSystem.lock();
		deathSystem->onBeforePlayerDestroyed.unsubscribe(onBeforePlayerDestroyedHandle);
	}
}

void GameTimeSystem::tick(float deltaTime, Mani::EntityRegistry& registry)
{
	if (!m_gameIsRunning)
	{
		return;
	}

	if (!m_gameHasStarted)
	{
		onGameStarted(registry);
		m_gameHasStarted = true;
	}

	m_elapsed += deltaTime;
	if (m_elapsed >= m_gameTime)
	{
		onGameFinished(registry, true);
	}
}

void GameTimeSystem::onGameStarted(Mani::EntityRegistry& registry)
{
	if (!m_movementSystem.expired())
	{
		m_movementSystem.lock()->enable();
	}
	if (!m_aIBehaviorSystem.expired())
	{
		m_aIBehaviorSystem.lock()->enable();
	}
	if (!m_playerSystem.expired())
	{
		m_playerSystem.lock()->enable();
	}
	if (!m_deathSystem.expired())
	{
		m_deathSystem.lock()->enable();
	}
	if (!m_attackSystem.expired())
	{
		m_attackSystem.lock()->enable();
	}
	if (!m_neutralSpawnerSystem.expired())
	{
		m_neutralSpawnerSystem.lock()->enable();
	}
	if (!m_playerCameraSystem.expired())
	{
		m_playerCameraSystem.lock()->enable();
	}

	if (!m_popupSystem.expired())
	{
		
		std::shared_ptr<PopupSystem> popupSystem = m_popupSystem.lock();
		popupSystem->ClearAllPopupsAsync(registry);
	}
}

void GameTimeSystem::onGameFinished(Mani::EntityRegistry& registry, bool isWin)
{
	disableGameSystems();

	if (!m_popupSystem.expired())
	{
		std::shared_ptr<PopupSystem> popupSystem = m_popupSystem.lock();
		{
			PopupSystem::Request request;
			if (isWin)
			{
				request.spritePath = "Ludumdare55/Assets/Sprites/Victory.sprite";
			}
			else
			{
				request.spritePath = "Ludumdare55/Assets/Sprites/Defeat.sprite";
			}

			request.startPosition = glm::vec3(8.f, 0.f, 25.0f);
			request.finishPosition = glm::vec3(8.f, 0.f, 5.f);
			request.scale = glm::vec3(-15.f, 15.f, 15.f);
			request.speed = 20.f;

			popupSystem->SpawnPopupAsync(request);
		}
		{
			PopupSystem::Request request;
			request.spritePath = "Ludumdare55/Assets/Sprites/Keys_Spacebar.sprite";
			request.startPosition = glm::vec3(5.5f, 0.f, -25.0f);
			request.finishPosition = glm::vec3(5.5f, 0.f, -5.f);
			request.scale = glm::vec3(-10.f, 10.f, 10.f);
			request.speed = 20.f;

			popupSystem->SpawnPopupAsync(request);
		}
	}

	m_gameIsRunning = false;
}

void GameTimeSystem::disableGameSystems()
{
	if (!m_movementSystem.expired())
	{
		m_movementSystem.lock()->disable();
	}
	if (!m_aIBehaviorSystem.expired())
	{
		m_aIBehaviorSystem.lock()->disable();
	}
	if (!m_playerSystem.expired())
	{
		m_playerSystem.lock()->disable();
	}
	if (!m_deathSystem.expired())
	{
		m_deathSystem.lock()->disable();
	}
	if (!m_attackSystem.expired())
	{
		m_attackSystem.lock()->disable();
	}
	if (!m_neutralSpawnerSystem.expired())
	{
		m_neutralSpawnerSystem.lock()->disable();
	}
}

void GameTimeSystem::onActionEvent(uint32_t userId, const Mani::InputAction& inputAction)
{
	if (inputAction.name == RESTART_GAME)
	{
		if (m_elapsed >= m_gameTime)
		{
			GameLoopSystem::restartGame();
		}
	}
	else if (inputAction.name == START_GAME)
	{
		if (!m_gameIsRunning && !m_gameHasStarted)
		{
			m_gameIsRunning = true;
		}
	}
	else if (inputAction.name == END_GAME)
	{
		GameLoopSystem::quitGame();
	}
}

void GameTimeSystem::onBeforePlayerDestroyed(Mani::EntityRegistry& registry, const Mani::EntityId entityId)
{
	m_elapsed = m_gameTime;
	onGameFinished(registry, false);
	if (!m_playerCameraSystem.expired())
	{
		m_playerCameraSystem.lock()->disable();
	}
}
