#pragma once

#include <filesystem>
#include <Attack/HealthComponent.h>
#include <Attack/AttackSystem.h>
#include <CollisionSystem.h>
#include <unordered_map>

struct MinionTemplate
{
	MinionTemplate() {};
	MinionTemplate(
		const std::filesystem::path& inSpritePath,
		const HealthComponent& inHealth,
		const AttackComponent& inAttack,
		const SphereCollisionComponent& inCollision,
		glm::vec3 inScale
	) : spritePath(inSpritePath),
		health(inHealth),
		attack(inAttack),
		collision(inCollision),
		scale(inScale)
	{};

	std::filesystem::path spritePath;
	HealthComponent health;
	AttackComponent attack;
	SphereCollisionComponent collision;
	glm::vec3 scale = glm::vec3(1.0f);
};

class MinionTemplates
{
public:
	static const std::unordered_map<std::string, MinionTemplate>& get()
	{
		static std::unordered_map<std::string, MinionTemplate> templates;
		if (templates.empty())
		{
			{
				templates["Minion_Eye"] =
					MinionTemplate(
						"Ludumdare55/Assets/Sprites/Minion_Eye_01.sprite",
						HealthComponent(25.f),
						AttackComponent(
							5.f, // attackRange
							5.f, // damage
							5.f, // attackCooldown
							FLT_MAX // attackCooldownElapsed
						),
						SphereCollisionComponent(
							glm::vec3(0.f), // centerOffset
							.85f, // radius
							0 // priotity
						),
						glm::vec3(3.f, 3.f, 3.f) // scale
					);

				templates["Minion_Skull"] =
					MinionTemplate(
						"Ludumdare55/Assets/Sprites/Minion_Skull.sprite",
						HealthComponent(25.f),
						AttackComponent(
							5.f, // attackRange
							5.f, // damage
							5.f, // attackCooldown
							FLT_MAX // attackCooldownElapsed
						),
						SphereCollisionComponent(
							glm::vec3(0.f), // centerOffset
							1.0f, // radius
							0 // priotity
						),
						glm::vec3(2.f, 2.f, 2.f) // scale
					);

				templates["Minion_Fire"] =
					MinionTemplate(
						"Ludumdare55/Assets/Sprites/Minion_Fire_01.sprite",
						HealthComponent(25.f),
						AttackComponent(
							5.f, // attackRange
							5.f, // damage
							5.f, // attackCooldown
							FLT_MAX // attackCooldownElapsed
						),
						SphereCollisionComponent(
							glm::vec3(.25f, 0.f, .25f), // centerOffset
							1.5f, // radius
							0 // priotity
						),
						glm::vec3(3.f, 3.f, 3.f) // scale
					);

				templates["Minion_Hand"] =
					MinionTemplate(
						"Ludumdare55/Assets/Sprites/Minion_Hand.sprite",
						HealthComponent(25.f),
						AttackComponent(
							5.f, // attackRange
							5.f, // damage
							5.f, // attackCooldown
							FLT_MAX // attackCooldownElapsed
						),
						SphereCollisionComponent(
							glm::vec3(0.f), // centerOffset
							.75f, // radius
							0 // priotity
						),
						glm::vec3(1.75f, 1.75f, 1.75f) // scale
					);
			}
		}

		return templates;
	}
};

class MinionTiers
{
public:
	static const std::unordered_map<uint8_t, std::vector<std::string>>& get()
	{
		static std::unordered_map<uint8_t, std::vector<std::string>> tiers;
		if (tiers.empty())
		{
			tiers[0] = {
				"Minion_Eye",
				"Minion_Skull",
				"Minion_Fire",
				"Minion_Hand",
			};
		}

		return tiers;
	}
};