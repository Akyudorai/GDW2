#pragma once

#include "NOU/CMeshRenderer.h"
#include <memory>

namespace nou
{
	class MorphRendererComponent : CMeshRenderer
	{
	public:

		enum class Attrib
		{
			POSITION_0 = 0,
			POSITION_1 = 1,
			NORMAL_0 = 2,
			NORMAL_1 = 3,
			UV = 4
		};

		MorphRendererComponent(Entity& owner,
			const Mesh& baseMesh,
			Material& mat);
		virtual ~MorphRendererComponent() = default;

		MorphRendererComponent(MorphRendererComponent&&) = default;
		MorphRendererComponent& operator=(MorphRendererComponent&&) = default;

		void UpdateData(const Mesh& frame0, const Mesh& frame1, float t);
		virtual void Draw();

	protected:

		//Pointers to the frames we're currently in between.
		Mesh* m_frame0;
		Mesh* m_frame1;
		//The t-value for interpolating between our frames.
		float m_t;
	};
}