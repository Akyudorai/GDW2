#ifndef DEV_STAGE_H
#define DEV_STAGE_H

#include "Scene.h"
#include <vector>
#include "../../NOU/include/NOU/Entity.h"
#include "Game/PlayerController.h"

using namespace nou;

namespace GAME
{

	class Dev_Stage : public Scene
	{
	public:
		explicit Dev_Stage();
		virtual ~Dev_Stage();

		Dev_Stage(const Dev_Stage&) = delete;
		Dev_Stage(Dev_Stage&&) = delete;
		Dev_Stage& operator = (const Dev_Stage&) = delete;
		Dev_Stage& operator = (Dev_Stage&&) = delete;

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
