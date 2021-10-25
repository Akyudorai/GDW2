#pragma once

#include <list>
#include <map>
#include <string>
#include <memory>

#include "NOU/Mesh.h"
#include "NOU/Texture.h"
#include "NOU/Material.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"


using namespace std;
using namespace nou;

namespace OMG
{
	class Resources {
	public:


		static Resources& GetInstance() {
			static Resources instance;
			return instance;
		}

		unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
		unique_ptr<Mesh> mesh_ducky, mesh_box;
		unique_ptr<Texture2D> tex2D_ducky;
		unique_ptr<Material> mat_ducky, mat_unselected, mat_selected, mat_line;
				
		void LoadResources();

		template <class T>
		string GetKey(const T* target);
		
		template <class T>
		map<string, unique_ptr<T>> GetMap();
		

		/* // For later, to compress all items into lists of item types
		map<string, unique_ptr<Mesh>> Meshes;
		map<string, unique_ptr<Texture2D>> Textures;
		map<string, unique_ptr<Material>> Materials;
		map<string, unique_ptr<ShaderProgram>> Shaders;
		*/

		map<string, unique_ptr<Mesh>> Meshes;

		/* // For when we transition to maps, we can grab from a specific list using an identifier
		template<typename T>
		T& GetResource()
		{
			return ???;
		}
		*/

	protected:


	private:
		

		
	};
}