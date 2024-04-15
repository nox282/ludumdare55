#pragma once

#include <Core/CoreFwd.h>
#include <Events/Event.h>

struct HurtComponent
{
	float elapsed = FLT_MAX;
	float duration = .15f;
};

class HealthSystem : public Mani::SystemBase
{
public:	
	virtual std::string_view getName() const override { return "HealthSystem"; }
	virtual bool shouldTick(Mani::EntityRegistry& registry) const override { return true; }

	virtual void tick(float deltaTime, Mani::EntityRegistry& registry) override;

protected:
	virtual void onInitialize(Mani::EntityRegistry& registry, Mani::SystemContainer& systemContainer) override;
	virtual void onDeinitialize(Mani::EntityRegistry& registry) override;

private:
	Mani::EventHandle onAttackEventHandle;
	
	std::weak_ptr<class AttackSystem> m_attackSystem;
	std::weak_ptr<class VFXSystem> m_vfxSystem;

	void onAttackEvent(Mani::EntityRegistry& registry, const struct AttackEventData& eventData);
};