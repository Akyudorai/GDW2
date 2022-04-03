#pragma once

#include "Gameplay/Components/IComponent.h"
#include "Gameplay/MeshResource.h"
#include "Gameplay/Components/RenderComponent.h"
#include <vector>

namespace Gameplay
{
	class AnimatorComponent : public Gameplay::IComponent
	{
	public:
		typedef std::shared_ptr<AnimatorComponent> Sptr;

		AnimatorComponent();

		// IComponent
		virtual void Update(float deltaTime);
		virtual void RenderImGui() override;
		virtual nlohmann::json ToJson() const override;
		static AnimatorComponent::Sptr FromJson(const nlohmann::json& data);
		MAKE_TYPENAME(AnimatorComponent);

	public:
		void SetFrames(std::vector<MeshResource::Sptr> frames);
		void SetRenderer(RenderComponent& component) {
			m_renderer = &component;
		}

		void SetLooping(bool state) {
			m_looping = state;
		}

		void SetReverse(bool state) {
			m_forward = !state;
		}

		void SetPause(bool state) {
			m_paused = state;
		}

		void SetSpeed(float speed) {
			m_speed = speed;
		}

		bool IsReversed() { return !m_forward; }
		bool IsPaused() { return m_paused; }

		void AddAnimation(std::string name, std::vector<MeshResource::Sptr> frames) {
			animations.emplace(name, frames);
		}

		void Play() {
			SetPause(false);
		}

		void PlayAnimation(std::string name) {
			if (animations.count(name) > 0) {
				SetAnimation(name);
				Play();
			}
		}

		void SetAnimation(std::string name) {
			if (animations.count(name) > 0) {
				currentAnimation = name;
				frameIndex = 0;
				m_timer = 0.0f;
			}
		}

	public:
		std::function<void()> onAnimationCompleted;
		std::string currentAnimation = "";

	protected:
		RenderComponent* m_renderer = nullptr;
		int frameIndex = 0;
		float m_timer;
		bool m_looping;
		bool m_forward;
		bool m_paused;
		float m_speed = 1.0f;


		std::map<std::string, std::vector<MeshResource::Sptr>> animations;
	};
}
