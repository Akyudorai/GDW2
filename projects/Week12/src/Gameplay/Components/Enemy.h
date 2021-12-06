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

		// IComponent		
		virtual void Update(float deltaTime);
		virtual void RenderImGui() override;
		virtual nlohmann::json ToJson() const override;
		static Enemy::Sptr FromJson(const nlohmann::json& data);
		MAKE_TYPENAME(Enemy);

	public:
		void SetNavPoints(std::vector<GameObject> nP);
		void SetOwner(GameObject::Sptr object);
	protected:
		
		GameObject::Sptr m_owner;
		float m_speed = 1.0f;

		int navIndex = 0;
		std::vector<GameObject> navPoints;
	};
}