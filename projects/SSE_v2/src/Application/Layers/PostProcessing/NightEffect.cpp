#include "NightEffect.h"
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
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/NightEffect.glsl" }
	});
	if (x) {
		_TexNoise = ResourceManager::CreateAsset<Texture2D>("textures/NoiseTex.png");
		_TexMask = ResourceManager::CreateAsset<Texture2D>("textures/MaskTex.png");
	}
}

NightEffect::~NightEffect() = default;

void NightEffect::Apply(const Framebuffer::Sptr & gBuffer)
{
	t += 0.01f;
	_shader->Bind();
	_TexNoise->Bind(1);
	_TexMask->Bind(2);
	_shader->SetUniform("t", t);
	_shader->SetUniform("lumi", l);
	_shader->SetUniform("ampcolor", c);
}

void NightEffect::RenderImGui()
{
	ImGui::SliderFloat("Strength", &t, 0.0f, 10.0f);
	ImGui::SliderFloat("Lumi", &l, 0.0f, 50.0f);
	ImGui::SliderFloat("C", &c, 0.0f, 50.0f);
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
