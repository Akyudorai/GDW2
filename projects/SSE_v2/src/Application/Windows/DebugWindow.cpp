#include "DebugWindow.h"
#include "Application/Application.h"
#include "Application/ApplicationLayer.h"
#include "Application/Layers/RenderLayer.h"

#include "Utils/ResourceManager/Resources.h"

DebugWindow::DebugWindow() :
	IEditorWindow()
{
	Name = "Debug";
	SplitDirection = ImGuiDir_::ImGuiDir_None;
	SplitDepth = 0.5f;
	Requirements = EditorWindowRequirements::Menubar;
}

DebugWindow::~DebugWindow() = default;

void DebugWindow::RenderMenuBar() 
{
	Application& app = Application::Get();
	RenderLayer::Sptr renderLayer = app.GetLayer<RenderLayer>(); 

	BulletDebugMode physicsDrawMode = app.CurrentScene()->GetPhysicsDebugDrawMode();
	if (BulletDebugDraw::DrawModeGui("Physics Debug Mode:", physicsDrawMode)) { 
		app.CurrentScene()->SetPhysicsDebugDrawMode(physicsDrawMode);
	}

	ImGui::Separator();

	RenderFlags flags = renderLayer->GetRenderFlags();
	bool changed = false;
	bool temp = *(flags & RenderFlags::EnableColorCorrection);
	if (ImGui::Checkbox("Enable Color Correction", &temp)) {
		changed = true;
		flags = (flags & ~*RenderFlags::EnableColorCorrection) | (temp ? RenderFlags::EnableColorCorrection : RenderFlags::None);
	}

	if (changed) {
		renderLayer->SetRenderFlags(flags);
	}

	ImGui::Separator();
	
	if (ImGui::Checkbox("Toggle Ambient Lighting", &renderAmbientLighting)) {
		for (auto& mat : Resources::GetMaterialMap())
		{
			if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation")
			{
				mat.second->Set("u_Lights.ToggleAmbience", renderAmbientLighting);
			}
		}
	}

	if (renderAmbientLighting) {
		float aStr = amb_Strength;
		glm::vec3 aCol = amb_Color;
		ImGui::InputFloat("Ambience Strength", &amb_Strength);
		ImGui::InputFloat3("Ambience Color", &amb_Color.x);		

		if (aStr != amb_Strength || aCol != amb_Color) {
			for (auto& mat : Resources::GetMaterialMap())
			{
				if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation")
				{
					mat.second->Set("u_Lights.AmbienceStrength", amb_Strength);
					mat.second->Set("u_Lights.Color", amb_Color);
				}
			}
		}
	}

	if (ImGui::Checkbox("Toggle Diffuse Lighting", &renderDiffuseLighting)) {
		for (auto& mat : Resources::GetMaterialMap())
		{
			if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation")
			{
				mat.second->Set("u_Lights.ToggleDiffuse", renderDiffuseLighting);
			}
		}
	}

	if (ImGui::Checkbox("Toggle Specular Lighting", &renderSpecularLighting)) {
		for (auto& mat : Resources::GetMaterialMap())
		{
			if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation")
			{
				mat.second->Set("u_Lights.ToggleSpecular", renderSpecularLighting);				
			}
		}
	}

	if (renderSpecularLighting)
	{
		float sStr = spec_Strength;
		ImGui::InputFloat("Specular Strength", &spec_Strength);
		if (sStr != spec_Strength)
		{
			for (auto& mat : Resources::GetMaterialMap())
			{
				if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation")
				{
					mat.second->Set("u_Lights.SpecularStrength", spec_Strength);
				}
			}
		}
	}

	if (ImGui::Checkbox("Toggle Inversion Effect", &renderInversionEffect)) {
		for (auto& mat : Resources::GetMaterialMap())
		{
			if (mat.second->GetShader()->GetDebugName() == "Deferred - GBuffer Generation")
			{				
				mat.second->Set("u_Lights.ToggleInversion", renderInversionEffect);
			}
		}
	}
}
