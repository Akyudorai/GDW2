#pragma once
#include "../ApplicationLayer.h"


class GameManagementLayer : public ApplicationLayer {
public:
	MAKE_PTRS(GameManagementLayer);
	GameManagementLayer();
	virtual ~GameManagementLayer();

	void OnAppLoad(const nlohmann::json& config) override;
	void OnUpdate() override;

};