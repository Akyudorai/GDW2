#pragma once

namespace GAME
{
	class Health
	{
	public:
		Entity* m_owner;
		float currentHealth, maximumHealth;

	public:
		void Initialize(float maxHealth) {			
			maximumHealth = maxHealth;
			currentHealth = maximumHealth;
		}

		void Damage();

	public:
		Health(Entity& owner) {
			m_owner = &owner;
		}
		virtual ~Health() = default;

		Health(Health&&) = default;
		Health& operator=(Health&&) = default;
	};
}