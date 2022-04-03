#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "Night_Vision.h" 
#include "Application/Application.h"


NightEffect::NightEffect() :
	NightEffect(true) {}

NightEffect::NightEffect(bool active) :
	PostProcessingLayer::Effect(),
	_shader(nullptr),
	_TexNoise(nullptr),
	_TexMask(nullptr)
{
	Name = "Night Vision Effect";
	_format = RenderTargetType::ColorRgb8;

	_shader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
		{ ShaderPartType::Vertex, "shaders/vertex_shaders/fullscreen_quad.glsl" },
		{ ShaderPartType::Fragment, "shaders/fragment_shaders/post_effects/Night_Vision.glsl" }
	});
	if (active)
	{
		_TexNoise = ResourceManager::CreateAsset<Texture2D>("textures/flashlight.png");
		_TexMask = ResourceManager::CreateAsset<Texture2D>("textures/flashlight.png");
	}

}
NightEffect::~NightEffect() = default;

void NightEffect::Apply(const Framebuffer::Sptr & gBuffer, VertexArrayObject::Sptr _quads)
{
	_shader->Bind();
	_TexNoise->Bind(1);
	_TexMask->Bind(2);
	t += 0.02f;
	_shader->SetUniform("t", t);
	_shader->SetUniform("lumi", L);
	_shader->SetUniform("coloramp", color);
}

void NightEffect::RenderImGui()
{
	ImGui::SliderFloat("Time", &t, 0.0f, 12.0f);
	ImGui::SliderFloat("Color", &color, 0.0f, 50.0f);
	ImGui::SliderFloat("Lumi", &L, 0.0f, 50.0f);

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
