#include "Enemy.h"

#include "GLM/gtc/random.hpp"

namespace Gameplay
{
	Enemy::Enemy() :
		navIndex(0), m_speed(5.0f), m_wanderTarget(0.0f)
	{ }
	
	Enemy::~Enemy() = default;

	void Enemy::Awake()
	{

	}

	void Enemy::Update(float deltaTime)
	{
		//Steering(deltaTime);

				

		// Move towards Nav Point
		//if (m_owner->GetPosition() != navPoints[navIndex].GetPosition())
		//{
		//	glm::vec3 direction = navPoints[navIndex].GetPosition() - m_owner->GetPosition();
		//	direction = glm::normalize(direction);

		//	m_owner->SetPosition(m_owner->GetPosition() + direction * m_speed * deltaTime);			
		//}

		//// Check if in range of point
		//float dist = glm::distance(m_owner->GetPosition(), navPoints[navIndex].GetPosition());
		//if (dist < 1.0f) {
		//	navIndex = (navIndex < navPoints.size() - 1) ? navIndex + 1 : 0;
		//}
	}

	void Enemy::Steering(float deltaTime)
	{
		float pursuitDist = glm::distance(m_owner->GetPosition(), m_target->GetPosition());
		float fleeDist = glm::distance(m_owner->GetPosition(), m_fleeTarget->GetPosition());

		// Flee
		if (fleeDist <= pursuitDist)
		{
			if (fleeDist < m_detectionRange * 1.5f)
			{
				glm::vec3 direction = m_owner->GetPosition() - m_fleeTarget->GetPosition();
				direction = glm::normalize(direction);

				glm::vec3 velocity;
				velocity = direction * m_speed;

				m_owner->LookAt(m_owner->GetPosition() + direction);
				m_owner->SetPosition(m_owner->GetPosition() + velocity * deltaTime);
			}
		}

		// Pursue
		else if (pursuitDist < m_detectionRange && pursuitDist > 3.0f)
		{
			glm::vec3 direction = m_target->GetPosition() - m_owner->GetPosition();
			direction = glm::normalize(direction);

			// Arrival Behavior
			glm::vec3 velocity;
			velocity = direction * m_speed * (pursuitDist / 10.0f);

			m_owner->LookAt(m_owner->GetPosition() + direction);
			m_owner->SetPosition(m_owner->GetPosition() + velocity * deltaTime);
		}

		// WANDER
		//else 
		//{
		//	// While idle, begin wander time countdown
		//	if (m_wanderTime > 0 && glm::distance(m_owner->GetPosition(), m_wanderTarget) < 1.0f)
		//	{
		//		m_wanderTime -= deltaTime;
		//	}

		//	// Once timer is up, generate new coordinate
		//	else if (m_wanderTime <= 0)
		//	{
		//		m_wanderTarget = GenerateWanderCoordinate();
		//		m_wanderTime = 2.0f;
		//	}

		//	// Steer towards new coordinate
		//	else
		//	{
		//		if (m_wanderTarget == glm::vec3(0)) {
		//			m_wanderTarget = GenerateWanderCoordinate();
		//			m_wanderTime = 2.0f;
		//		}

		//		glm::vec3 direction = m_wanderTarget - m_owner->GetPosition();
		//		direction = glm::normalize(direction);

		//		glm::vec3 velocity;
		//		velocity = direction * m_speed * (glm::distance(m_wanderTarget, m_owner->GetPosition() / 10.0f));

		//		m_owner->LookAt(m_owner->GetPosition() + direction);
		//		//m_owner->SetPosition(m_owner->GetPosition() + velocity * deltaTime);
		//	}
		//}
	

		// Radius Check
		
	}

	glm::vec3 Enemy::GenerateWanderCoordinate()
	{
		glm::vec3 pos = m_owner->GetPosition();
		float randX = glm::linearRand(pos.x - 5.0f, pos.x + 5.0f);
		float randZ = glm::linearRand(pos.z - 5.0f, pos.z + 5.0f);

		glm::vec3 newCoordinate = glm::vec3(0.0f);
		newCoordinate.x = randX;
		newCoordinate.y = m_owner->GetPosition().y;
		newCoordinate.z = randZ;

		return newCoordinate;
	}

	void Enemy::SetNavPoints(std::vector<GameObject> nP)
	{
		navPoints = nP;
	}	

	void Enemy::SetOwner(GameObject::Sptr object)
	{
		m_owner = object;
	}

	void Enemy::SetTarget(GameObject::Sptr object)
	{
		m_target = object;
	}

	void Enemy::SetFleeTarget(GameObject::Sptr object)
	{
		m_fleeTarget = object;
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