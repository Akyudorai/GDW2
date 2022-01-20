#ifndef SCENE_H
#define SCENE_H

#include "GLM/glm.hpp"

namespace GAME 
{
	class Scene 
	{
	public:
		explicit Scene();
		virtual ~Scene() = 0;

		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator = (const Scene&) = delete;
		Scene& operator = (Scene&&) = delete;

		// 4 abstract functions and make them polymorphic.  
		// Also known as "pure virtuals"
		virtual bool OnCreate() = 0;
		virtual void OnDestroy() = 0;
		virtual void Update(const float deltaTime) = 0;
		virtual void Render() const = 0;
		virtual void HandleEvents() = 0;
		virtual void OnResizeWindow(const int, const int) = 0;

	protected:

	};
}

#endif