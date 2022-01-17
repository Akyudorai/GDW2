#ifndef Level_2_H
#define Level_2_H

#include "Scene.h"
#include <vector>
#include "../../NOU/include/NOU/Entity.h"
#include "Game/PlayerController.h"

using namespace nou;

namespace GAME
{

	class Level_2 : public Scene
	{
	public:
		explicit Level_2();
		virtual ~Level_2();

		Level_2(const Level_2&) = delete;
		Level_2(Level_2&&) = delete;
		Level_2& operator = (const Level_2&) = delete;
		Level_2& operator = (Level_2&&) = delete;

		virtual bool OnCreate();
		virtual void OnDestroy();
		virtual void Update(const float deltaTime);
		virtual void Render() const;
		virtual void HandleEvents();
		virtual void OnResizeWindow(const int, const int);

	private:
		void LoadLevel();

	private:

		Entity camera;
		Entity body;
		Entity shadow;
		Entity wall_1;
		Entity wall_2;
		Entity wall_3;
		Entity winBox;

		PlayerController pc;

		std::vector<Entity*> entities;
	};
}

#endif;