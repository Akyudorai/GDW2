#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"

class MovingPlatformBehavior : public Gameplay::IComponent
{
public:
	typedef std::shared_ptr<MovingPlatformBehavior> Sptr;

	MovingPlatformBehavior();
	virtual ~MovingPlatformBehavior();

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(MovingPlatformBehavior);
	virtual nlohmann::json ToJson() const override;
	static MovingPlatformBehavior::Sptr FromJson(const nlohmann::json& blob);

public:
	void Update(float deltaTime);
	void SetStartPosition(glm::vec3 position) { startPosition = position; }
	void SetEndPosition(glm::vec3 position) { endPosition = position; }
	void SetSpeed(float speed) { moveSpeed = speed; }
	void SetLooping(bool state) { isLooping = state; }
	void SetCollider(Gameplay::Physics::BoxCollider::Sptr reference) { collider = reference; }
	void Activate();

private:
	Gameplay::Physics::BoxCollider::Sptr collider;
	float moveSpeed = 1.0f;
	float timer = 0.0f;
	glm::vec3 startPosition = glm::vec3(0, 0, 0);
	glm::vec3 endPosition = glm::vec3(0, 0, 0);

	bool isDisabled = false;
	bool isCompleted = true;
	bool isLooping = false;
	bool isReverse = false;

	template<typename T>
	T Lerp(const T& p0, const T& p1, float t) {
		return (1.0f - t) * p0 + t * p1;
	}
};