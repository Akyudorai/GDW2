#include "UpdatedBoxCollider.h"

#include "Utils/ImGuiHelper.h"

void UpdatedBoxCollider::Awake()
{
	UpdatePosition();
}

void UpdatedBoxCollider::Update(float deltaTime)
{
	UpdatePosition();
}

void UpdatedBoxCollider::RenderImGui()
{
	LABEL_LEFT(ImGui::InputFloat3, "Extents", (float*)&m_extents, 2);
	LABEL_LEFT(ImGui::Checkbox, "Is Trigger", &isTrigger);
}

nlohmann::json UpdatedBoxCollider::ToJson() const
{
	return {
		{ "x_extent", m_extents.x },
		{ "y_extent", m_extents.y },
		{ "z_extent", m_extents.z },
		{ "isTrigger", isTrigger }
	};
}

UpdatedBoxCollider::UpdatedBoxCollider() : m_extents(glm::vec3(1.0f))
{ }

UpdatedBoxCollider::UpdatedBoxCollider(glm::vec3 extents) : m_extents(extents)
{ 
	for (int i = 0; i < 8; i++) {
		vertices[i] = glm::vec3(0);
	}
}

UpdatedBoxCollider::~UpdatedBoxCollider() = default;

UpdatedBoxCollider::Sptr UpdatedBoxCollider::FromJson(const nlohmann::json & blob)
{
	UpdatedBoxCollider::Sptr result = std::make_shared<UpdatedBoxCollider>();
	result->m_extents = glm::vec3(blob["x_extent"], blob["y_extent"], blob["z_extent"]);
	result->isTrigger = blob["isTrigger"];
	return result;
}

