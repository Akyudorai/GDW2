#pragma once
#include "../ApplicationLayer.h"


class AudioLayer : public ApplicationLayer {
public:
	MAKE_PTRS(AudioLayer);
	AudioLayer();
	virtual ~AudioLayer();

	void OnAppLoad(const nlohmann::json& config) override;
	void OnUpdate() override;
	void OnAppUnload() override;

};