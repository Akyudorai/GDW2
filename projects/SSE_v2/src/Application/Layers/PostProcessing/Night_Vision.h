#pragma once
#include "Application/Layers/PostProcessingLayer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/FrameBuffer.h"

class NightEffect : public PostProcessingLayer::Effect {
public:
	MAKE_PTRS(NightEffect);
	Texture2D::Sptr _TexNoise;
	Texture2D::Sptr _TexMask;

	NightEffect();
	NightEffect(bool active);
	virtual ~NightEffect();

	virtual void Apply(const Framebuffer::Sptr& gBuffer, VertexArrayObject::Sptr _quads);
	virtual void RenderImGui() override;

	NightEffect::Sptr FromJson(const nlohmann::json& data);
	virtual nlohmann::json ToJson() const override;

protected:
	ShaderProgram::Sptr _shader;

	float color = 4.0f;
	float t = 1.0f;
	float L = 0.2f;
};


