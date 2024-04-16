#pragma once

#include <Core/CoreFwd.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>
#include <GameSystem.h>

class GameLoopSystem : public Mani::SystemBase
{
public:
	virtual std::string_view getName() const override { return "GameLoopSystem"; }
	virtual Mani::ETickGroup getTickGroup() const override { return Mani::ETickGroup::PostTick; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override 
	{
		if (s_shouldRestart)
		{
			s_shouldRestart = false;

			if (s_world != nullptr)
			{
				s_worldSystem.lock()->destroyWorld(s_world);
			}

			createWorld();
		}

		if (s_shouldQuit)
		{
			Mani::Application::get().stop();
		}
	}

	static void restartGame()
	{
		if (s_worldSystem.expired())
		{
			return;
		}

		s_shouldRestart = true;
	}

	static void quitGame()
	{
		s_shouldQuit = true;
	}

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer)
	{
		s_worldSystem = systemContainer.initializeDependency<Mani::WorldSystem>();
		createWorld();
	}

private:
	static std::weak_ptr<Mani::WorldSystem> s_worldSystem;
	static std::shared_ptr<Mani::World> s_world;
	static bool s_shouldRestart;
	static bool s_shouldQuit;

	static void createWorld()
	{
		if (s_worldSystem.expired())
		{
			return;
		}
		s_world = s_worldSystem.lock()->createWorld();
		Mani::SystemContainer& worldSystemContainer = s_world->getSystemContainer();
		worldSystemContainer.createSystem<Mani::OpenGLRenderSystem>()
			.createSystem<GameSystem>();
	}
};
