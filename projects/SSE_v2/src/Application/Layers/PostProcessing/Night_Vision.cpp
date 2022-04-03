#include "Night_Vision.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "../RenderLayer.h"
#include "Application/Application.h"

NightEffect::NightEffect() :
	NightEffect(true) {}

NightEffect::NightEffect(bool x) :
	PostProcessingLayer::Effect(),
	_shader(nullptr),
	_TexNoise(nullptr),
	_TexMask(nullptr)
{
	Name = "NightEffect";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/Night_Vision.glsl" }
	});
	if (x) {
		_TexNoise = ResourceManager::CreateAsset<Texture2D>("textures/2D/NoiseTex.png");
		_TexMask = ResourceManager::CreateAsset<Texture2D>("textures/2D/MaskTex.png");
	}
}

NightEffect::~NightEffect() = default;

void NightEffect::Apply(const Framebuffer::Sptr & gBuffer)
{
	timer += 0.01f;
	_shader->Bind();
	_TexNoise->Bind(1);
	_TexMask->Bind(2);
	_shader->SetUniform("iTime", timer);
	_shader->SetUniform("luminanceThreshold", light);
	_shader->SetUniform("colorAmplification", color);
}

void NightEffect::RenderImGui()
{
	/*const auto& cam = Application::Get().CurrentScene()->MainCamera;

	if (cam != nullptr) {
		ImGui::DragFloat("Focal Depth", &cam->FocalDepth, 0.1f, 0.1f, 100.0f);
		ImGui::DragFloat("Lens Dist. ", &cam->LensDepth,  0.01f, 0.001f, 50.0f);
		ImGui::DragFloat("Aperture   ", &cam->Aperture,   0.1f, 0.1f, 60.0f);
	}*/
	ImGui::SliderFloat("Strength", &timer, 0.0f, 10.0f);
	ImGui::SliderFloat("Luminence", &light, 0.0f, 30.0f);
	ImGui::SliderFloat("Color", &color, 0.0f, 40.0f);
}

NightEffect::Sptr NightEffect::FromJson(const nlohmann::json & data)
{
	NightEffect::Sptr final = std::make_shared<NightEffect>();
	final->Enabled = JsonGet(data, "enabled", true);
	return final;
}

nlohmann::json NightEffect::ToJson() const
{
	return {
		{ "enabled", Enabled }
	};
}
