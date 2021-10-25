#include "Resources.h"

namespace OMG
{
	void Resources::LoadResources()
	{
		// Textured lit shader
		unique_ptr vs_texLitShader = make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
		unique_ptr fs_texLitShader = make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);
		vector<Shader*> texLit = { vs_texLitShader.get(), fs_texLitShader.get() };
		prog_texLit = make_unique<ShaderProgram>(texLit);

		// Untextured lit shader
		unique_ptr vs_litShader = make_unique<Shader>("shaders/lit.vert", GL_VERTEX_SHADER);
		unique_ptr fs_litShader = make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);
		vector<Shader*> lit = { vs_litShader.get(), fs_litShader.get() };
		prog_lit = make_unique<ShaderProgram>(lit);

		// Untextured unlit shader
		unique_ptr vs_unlitShader = make_unique<Shader>("shaders/unlit.vert", GL_VERTEX_SHADER);
		unique_ptr fs_unlitShader = make_unique<Shader>("shaders/unlit.frag", GL_FRAGMENT_SHADER);
		vector<Shader*> unlit = { vs_unlitShader.get(), vs_unlitShader.get() };
		prog_unlit = make_unique<ShaderProgram>(unlit);

		// Set up duck
		mesh_ducky = std::make_unique<Mesh>();
		GLTF::LoadMesh("duck/Duck.gltf", *mesh_ducky);
		//Meshes.insert(make_pair("Duck", mesh_ducky));
		pair<string, unique_ptr<Mesh>> p_duck_mesh = make_pair("Duck", move(mesh_ducky));
		Meshes.insert(move(p_duck_mesh));

		tex2D_ducky = std::make_unique<Texture2D>("duck/DuckCM.png");

		// Set up box
		mesh_box = std::make_unique<Mesh>();
		GLTF::LoadMesh("box/Box.gltf", *mesh_box);
		pair<string, unique_ptr<Mesh>> p_box_mesh = make_pair("Box", move(mesh_box));
		Meshes.insert(move(p_box_mesh));

		// Set up duck material
		mat_ducky = std::make_unique<Material>(*prog_texLit);
		mat_ducky->AddTexture("albedo", *tex2D_ducky);

		// Set up point and line materials
		mat_unselected = std::make_unique<Material>(*prog_lit);
		mat_unselected->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

		mat_selected = std::make_unique<Material>(*prog_lit);
		mat_selected->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

		mat_line = std::make_unique<Material>(*prog_unlit);
		mat_line->m_color = glm::vec3(1.0f, 1.0f, 1.0f);
	}
	
	template <class T>
	string Resources::GetKey(const T* target)
	{
		for (std::map<string, unique_ptr<T>>::iterator it = GetMap<T>().begin(); it != GetMap<T>().end(); ++it)
		{
			bool is_selected = (current_mesh == it->first);
			if (ImGui::Selectable(const_cast<char*>(it->first.c_str()), is_selected)) {
				current_mesh = it->first.c_str();
				// Set the mesh
				object->Get<CMeshRenderer>().SetMesh(*Resources::GetInstance().Meshes[current_mesh]);
			}
		}
	}

	template <class T> 
	map<string, unique_ptr<T>> GetMap() {
		
		switch (T) {
		case Mesh:
			return Meshes;		
		default:
			break;
		}
	}
}