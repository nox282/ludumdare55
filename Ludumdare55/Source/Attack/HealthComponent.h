#pragma once

struct HealthComponent
{
	HealthComponent() {}
	HealthComponent(float inHealth) : health(inHealth) {}
	float health = 50.0f;
	float maxHealth = 50.0f;
};