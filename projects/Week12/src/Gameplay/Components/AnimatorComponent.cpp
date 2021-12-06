#include "AnimatorComponent.h"

namespace Gameplay
{
	AnimatorComponent::AnimatorComponent()
		: frameIndex(0), m_timer(0.0f), m_renderer(nullptr),
		m_looping(true), m_forward(true), m_paused(false), m_speed(1.0f)
	{ }
	
	void AnimatorComponent::Update(float deltaTime)
	{		
		if (m_paused) return;

		m_timer += m_speed * deltaTime;

		if (m_timer >= 1.0f) {
			m_timer = 0;

			if (m_looping) {
				if (m_forward)
					frameIndex = (frameIndex < animations[currentAnimation].size() - 1) ? frameIndex + 1 : 0;
				else
					frameIndex = (frameIndex == 0) ? animations[currentAnimation].size() - 1 : frameIndex - 1;
			}
				
			else {
				
				if (m_forward) {	
					// Safety Check for quick switches between forward and reverse animation
					if (frameIndex >= animations[currentAnimation].size() - 1) return;

					frameIndex = frameIndex + 1;					
					if (frameIndex >= animations[currentAnimation].size() - 1) {
						m_paused = true;
						if (onAnimationCompleted)
							onAnimationCompleted();
					}
				}
				else {
					// Safety Check for quick switches between forward and reverse animation
					if (frameIndex <= 0) return;

					frameIndex = frameIndex - 1;
					if (frameIndex <= 0) {
						m_paused = true;
						if (onAnimationCompleted)
							onAnimationCompleted();
					}
				}				
			}
		}

		if (m_forward) {
			if (frameIndex <= animations[currentAnimation].size() - 2)
				m_renderer->Animate(animations[currentAnimation][frameIndex], animations[currentAnimation][frameIndex + 1], m_timer);
			else
				m_renderer->Animate(animations[currentAnimation][frameIndex], animations[currentAnimation][0], m_timer);
		}
		else {
			if (frameIndex > 0)
				m_renderer->Animate(animations[currentAnimation][frameIndex], animations[currentAnimation][frameIndex - 1], m_timer);
			else
				m_renderer->Animate(animations[currentAnimation][0], animations[currentAnimation][animations[currentAnimation].size()-1], m_timer);
		}
		
	}

	// DEPRECIATED
	void AnimatorComponent::SetFrames(std::vector<MeshResource::Sptr> frames)
	{
		
		//m_frames = std::move(frames);
	}

	void AnimatorComponent::RenderImGui()
	{ }

	nlohmann::json AnimatorComponent::ToJson() const
	{
		return { };
	}

	AnimatorComponent::Sptr AnimatorComponent::FromJson(const nlohmann::json& blob)
	{
		AnimatorComponent::Sptr result = std::make_shared<AnimatorComponent>();
		return result;
	}
}

