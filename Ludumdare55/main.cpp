#include <Core/CoreFwd.h>

#include <Assets/AssetSystem.h>

#include <Inputs/InputSystem.h>
#include <FloatingCamera/FloatingCameraControllerSystem.h>

#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <GameSystem.h>

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
						.createSystem<GameSystem>();

	app.run();

	return EXIT_SUCCESS;
}