#pragma once
#include "IComponent.h"

class TurretBehavior : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<TurretBehavior> Sptr;

	TurretBehavior();
	virtual ~TurretBehavior();

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(TurretBehavior);
	virtual nlohmann::json ToJson() const override;
	static TurretBehavior::Sptr FromJson(const nlohmann::json& blob);

public:
	void Initialize(float fireRate, float rateOffset, glm::vec3 origin, glm::vec3 direction, bool disabled);
	void Update(float deltaTime);
	void Shoot();
	
private:
	float currentTimer;
	float shotDelayTimer;
	glm::vec3 shotOrigin;
	glm::vec3 shotDirection;
	bool isDisabled = false;
};