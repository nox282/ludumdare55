#pragma once

#include <ECS/Entity.h>

struct FactionComponent
{
	Mani::EntityId factionOwnerId = Mani::INVALID_ID;
};