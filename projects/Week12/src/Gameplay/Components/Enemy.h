#pragma once

#include "Gameplay/Components/IComponent.h"
#include "Gameplay/GameObject.h"
#include <vector>

namespace Gameplay
{
	class Enemy : public IComponent {
	public:
		typedef std::shared_ptr<Enemy> Sptr;

		Enemy();
		virtual ~Enemy();

		// IComponent		
		virtual void Awake() override;
		virtual void Update(float deltaTime);
		virtual void RenderImGui() override;
		virtual nlohmann::json ToJson() const override;
		static Enemy::Sptr FromJson(const nlohmann::json& data);
		MAKE_TYPENAME(Enemy);

	public:
		void SetNavPoints(std::vector<GameObject> nP);
		void SetOwner(GameObject::Sptr object);
		void SetTarget(GameObject::Sptr object);
		void SetFleeTarget(GameObject::Sptr object);
		
	protected:
		void Steering(float deltaTime);		
		glm::vec3 GenerateWanderCoordinate();
	protected:
		
		GameObject::Sptr m_owner;
		GameObject::Sptr m_target;
		GameObject::Sptr m_fleeTarget;

		float m_speed = 50.0f;

		glm::vec3 m_wanderTarget;
		float m_wanderTime = 0.0f;

		float m_detectionRange = 10.0f;
		
		int navIndex = 0;
		std::vector<GameObject> navPoints;
	};
}