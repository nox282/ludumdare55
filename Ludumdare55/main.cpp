#include <Core/CoreFwd.h>

#include <Assets/AssetSystem.h>

#include <Inputs/InputSystem.h>
#include <FloatingCamera/FloatingCameraControllerSystem.h>

#include <OpenGL/OpenGLSystem.h>
#include <OpenGL/Render/OpenGLRenderSystem.h>

#include <GameFlow/GameLoopSystem.h>

using namespace Mani;

int main(int argc, char** argv)
{
	Application app;
	SystemContainer& appSystemContainer = app.getSystemContainer();
	appSystemContainer.createSystem<OpenGLSystem>()
					  .createSystem<GameLoopSystem>();

	app.run();

	return EXIT_SUCCESS;
}