#ifndef Level_1_H
#define Level_1_H

#include "Scene.h"
#include <vector>
#include "../../NOU/include/NOU/Entity.h"
#include "Game/PlayerController.h"

using namespace nou;

namespace GAME 
{

	class Level_1 : public Scene
	{
	public:
		explicit Level_1();
		virtual ~Level_1();

		Level_1(const Level_1&) = delete;
		Level_1(Level_1&&) = delete;
		Level_1& operator = (const Level_1&) = delete;
		Level_1& operator = (Level_1&&) = delete;

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
		Entity damageBox;
		Entity winBox;

		PlayerController pc;

		std::vector<Entity*> entities;		
	};
}

#endif