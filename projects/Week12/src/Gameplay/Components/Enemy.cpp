#include "Enemy.h"

namespace Gameplay
{
	Enemy::Enemy() :
		navIndex(0), m_speed(1.0f)
	{ }

	void Enemy::Update(float deltaTime)
	{

		// Move towards Nav Point
		if (m_owner->GetPosition() != navPoints[navIndex].GetPosition())
		{
			glm::vec3 direction = navPoints[navIndex].GetPosition() - m_owner->GetPosition();
			direction = glm::normalize(direction);

			m_owner->SetPosition(m_owner->GetPosition() + direction * m_speed * deltaTime);			
		}

		// Check if in range of point
		float dist = glm::distance(m_owner->GetPosition(), navPoints[navIndex].GetPosition());
		if (dist < 1.0f) {
			navIndex = (navIndex < navPoints.size() - 1) ? navIndex + 1 : 0;
		}
	}

	void Enemy::SetNavPoints(std::vector<GameObject> nP)
	{
		navPoints = nP;
	}	

	void Enemy::SetOwner(GameObject::Sptr object)
	{
		m_owner = object;
	}

	void Enemy::RenderImGui()
	{ }

	nlohmann::json Enemy::ToJson() const
	{
		return { };
	}

	Enemy::Sptr Enemy::FromJson(const nlohmann::json& blob)
	{
		Enemy::Sptr result = std::make_shared<Enemy>();
		return result;
	}
}