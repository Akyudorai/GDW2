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
		inline void SetRenderer(RenderComponent& component) { m_renderer = &component; VAO = m_renderer->GetMesh(); }
		inline void SetLooping(bool state) { m_looping = state; }
		inline void SetPause(bool state) { m_paused = state; }
		inline void SetSpeed(float speed) { m_speed = speed; }
		void SetReverse(bool state);

		inline bool IsReversed() { return !m_forward; }
		inline bool IsPaused() { return m_paused; }

		void AddAnimation(std::string name, std::vector<MeshResource::Sptr> frames, float duration);
		void Play(std::string name);

	public:
		std::function<void()> onAnimationCompleted;

		struct AnimationClip
		{
			std::string Name;
			std::vector<Gameplay::MeshResource::Sptr> Frames;

			int CurrFrame;
			int NextFrame;
			float FrameDuration;
		};

	protected:
		RenderComponent* m_renderer = nullptr;
		float m_timer;
		bool m_looping;
		bool m_forward;
		bool m_paused;
		float m_speed = 1.0f;

		VertexArrayObject::Sptr VAO;
		AnimationClip currentClip;

		std::vector<AnimationClip> animations;
	};
}