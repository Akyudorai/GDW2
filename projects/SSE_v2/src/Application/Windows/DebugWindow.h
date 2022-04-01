#pragma once
#include "Application/IEditorWindow.h"

#include <GLM/glm.hpp>

/**
 * Handles displaying debug information
 */
class DebugWindow final : public IEditorWindow {
public:
	MAKE_PTRS(DebugWindow);
	DebugWindow();
	virtual ~DebugWindow();

	// Inherited from IEditorWindow

	virtual void RenderMenuBar() override;

protected:
	bool renderAmbientLighting;
	float amb_Strength;
	glm::vec3 amb_Color;
	bool renderDiffuseLighting;	
	bool renderSpecularLighting;
	float spec_Strength;
	bool renderInversionEffect;
	bool renderFilmGrainEffect;

};