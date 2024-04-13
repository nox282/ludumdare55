#include <Core/CoreFwd.h>

#include <Assets/AssetSystem.h>

#include <Inputs/InputSystem.h>
#include <FloatingCamera/FloatingCameraControllerSystem.h>

#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <GameSystem.h>
#include <PlayerSystem.h>
#include <PlayerCameraSystem.h>
#include <CollisionSystem.h>

#include <Minion/MovementSystem.h>
#include <Minion/MinionSpawnSystem.h>
#include <Minion/AIBehaviorSystem.h>
#include <Minion/AggroSystem.h>

using namespace Mani;

int main(int argc, char** argv)
{
	Application app;
	SystemContainer& appSystemContainer = app.getSystemContainer();
	appSystemContainer.createSystem<OpenGLSystem>();
	std::shared_ptr<WorldSystem> worldSystem = appSystemContainer.initializeDependency<WorldSystem>().lock();

	std::shared_ptr<World> world = worldSystem->createWorld();
	
	SystemContainer& worldSystemContainer = world->getSystemContainer();
	worldSystemContainer.createSystem<OpenGLRenderSystem>()
						.createSystem<PlayerSystem>()
						.createSystem<PlayerCameraSystem>()
						.createSystem<AIBehaviorSystem>()
						.createSystem<MovementSystem>()
						.createSystem<MinionSpawnSystem>()
						.createSystem<CollisionSystem>()
						.createSystem<AggroSystem>()
						.createSystem<GameSystem>();

	app.run();

	return EXIT_SUCCESS;
}