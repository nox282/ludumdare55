#pragma	once

#include <Core/CoreFwd.h>
#include <Events/Event.h>

struct AttackComponent
{
	AttackComponent() {}
	AttackComponent(
		float inAttackRange,
		float inDamage,
		float inAttackCooldown,
		float inAttackCooldownElapsed
	) : attackRange(inAttackRange),
		damage(inDamage),
		attackCooldown(inAttackCooldown),
		attackCooldownElapsed(inAttackCooldownElapsed)
	{}

	float attackRange = 5.f;
	float damage = 5.f;
	float attackCooldown = 5.f;
	float attackCooldownElapsed = FLT_MAX;
};

struct AttackEventData
{
	Mani::EntityId source = Mani::INVALID_ID;
	Mani::EntityId target = Mani::INVALID_ID;
	float damage = 0.f;
};

class AttackSystem : public Mani::SystemBase
{
public:
	Mani::Event<Mani::EntityRegistry&, const AttackEventData&> onAttackEvent;

	virtual std::string_view getName() const override { return "AttackSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	std::weak_ptr<class VFXSystem> m_vfxSystem;
	std::weak_ptr<class SpatialGridSystem> m_spatialGridSystem;

	void doAttack(Mani::EntityRegistry& registry, Mani::EntityId entityId, Mani::EntityId targetEntityId, float damage);
};