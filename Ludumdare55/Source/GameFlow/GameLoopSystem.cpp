#include "GameLoopSystem.h"

std::weak_ptr<Mani::WorldSystem> GameLoopSystem::s_worldSystem;
std::shared_ptr<Mani::World> GameLoopSystem::s_world;
bool GameLoopSystem::s_shouldRestart = false;
bool GameLoopSystem::s_shouldQuit = false;