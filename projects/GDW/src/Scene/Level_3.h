#ifndef Level_3_H
#define Level_3_H

#include "Scene.h"
#include <vector>
#include "../../NOU/include/NOU/Entity.h"
#include "Game/PlayerController.h"

using namespace nou;

namespace GAME
{

	class Level_3 : public Scene
	{
	public:
		explicit Level_3();
		virtual ~Level_3();

		Level_3(const Level_3&) = delete;
		Level_3(Level_3&&) = delete;
		Level_3& operator = (const Level_3&) = delete;
		Level_3& operator = (Level_3&&) = delete;

		virtual bool OnCreate();
		virtual void OnDestroy();
		virtual void Update(const float deltaTime);
		virtual void Render() const;
		virtual void HandleEvents();
		virtual void OnResizeWindow(const int, const int);

	private:

		Entity camera;
		Entity body;
		Entity shadow;
		Entity wall_1;
		Entity wall_2;
		Entity wall_3;

		PlayerController pc;

		std::vector<Entity*> entities;
	};
}

#endif
