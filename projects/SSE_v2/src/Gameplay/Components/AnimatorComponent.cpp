#include "AnimatorComponent.h"

namespace Gameplay
{
	AnimatorComponent::AnimatorComponent()
		: m_timer(0.0f), m_renderer(nullptr),
		m_looping(true), m_forward(true), m_paused(true), m_speed(1.0f)
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
				m_renderer->Animate(animations[currentAnimation][0], animations[currentAnimation][animations[currentAnimation].size() - 1], m_timer);
		}

	}

	/*void AnimatorComponent::AddAnimation(std::string name, std::vector<MeshResource::Sptr> frames, float duration) {

		AnimationClip clip;
		{
			clip.Name = name;
			clip.Frames = frames;
			clip.FrameDuration = duration;
			clip.CurrFrame = 0;

			if (clip.Frames.size() == 0) clip.NextFrame = 0;
			else clip.NextFrame = 1;
		}

		if (animations.size() == 0) currentClip = clip;
		animations.push_back(clip);
	}

	void AnimatorComponent::Play(std::string name) {

		for (int i = 0; i < animations.size(); i++)
		{
			if (animations[i].Name == name) {
				currentClip = animations[i];
				SetPause(false);
				return;
			}
		}

		LOG_ERROR("No animation clip with the name " + name);
	}

	void AnimatorComponent::SetReverse(bool state)
	{
		m_forward = !state;

		if (m_forward)
		{
			if (currentClip.Frames.size() == 0) currentClip.NextFrame = 0;
			else currentClip.NextFrame = currentClip.CurrFrame + 1;
		}

		else
		{
			if (currentClip.Frames.size() == 0) currentClip.NextFrame = 0;
			else currentClip.NextFrame = currentClip.CurrFrame - 1;
		}
	}*/

	void AnimatorComponent::RenderImGui()
	{
		ImGui::DragFloat("m_Timer", &m_timer);

		ImGui::Checkbox("Paused", &m_paused);
		ImGui::Checkbox("Forward Animation?", &m_forward);
		ImGui::Checkbox("Looping?", &m_looping);
	}

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
