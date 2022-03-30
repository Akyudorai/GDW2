#include "MovingPlatformBehavior.h"
#include "Utils/ImGuiHelper.h"

#include "Gameplay/GameObject.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"

using namespace Gameplay;

void MovingPlatformBehavior::RenderImGui()
{ }

nlohmann::json MovingPlatformBehavior::ToJson() const
{
	return {};
}

MovingPlatformBehavior::MovingPlatformBehavior()
{ }

MovingPlatformBehavior::~MovingPlatformBehavior() = default;

MovingPlatformBehavior::Sptr MovingPlatformBehavior::FromJson(const nlohmann::json & blob)
{
	MovingPlatformBehavior::Sptr result = std::make_shared<MovingPlatformBehavior>();
	return result;
}

void MovingPlatformBehavior::Update(float deltaTime)
{
	// Do nothing if disabled
	if (isDisabled) return;

	// Update Timer
	if (timer < 1.0f)
		timer += deltaTime * 0.1f;
	else if (timer > 1.0f)
	{
		timer = 1.0f;

		if (isLooping) {
			isReverse = !isReverse;
			timer = 0.0f;
		}
	}


	// Lerp Between Two Points
	if (!isReverse)
	{
		glm::vec3 position = Lerp(startPosition, endPosition, timer);
		this->GetGameObject()->SetPosition(position);
		collider->SetPosition(glm::vec3(0.0f, 0.0f, -(startPosition.z - position.z)));
	}
	else
	{
		glm::vec3 position = Lerp(endPosition, startPosition, timer);
		this->GetGameObject()->SetPosition(position);
		collider->SetPosition(glm::vec3(0.0f, 0.0f, -(startPosition.z - position.z)));
	}
}

void MovingPlatformBehavior::Activate()
{
	if (!isLooping && isCompleted) {
		timer = 0.0f;
		isReverse = !isReverse;
	}
	else {
		isDisabled = !isDisabled;
	}

}
