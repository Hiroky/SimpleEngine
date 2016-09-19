#pragma once

#include "se/Common.h"
#include "se/Graphics/GPUBuffer.h"

namespace se
{

	/**
	 *	スタティックメッシュ
	 */
	class StaticMesh
	{
	private:
		struct Shape
		{
			uint32_t indexStart;
			uint32_t indexCount;
			uint32_t materialIndex;
		};

		struct Material
		{
			Texture* albedo;
		};

	private:
		std::vector<Shape> shapes_;
		VertexBuffer vertexBuffer_;
		IndexBuffer indexBuffer_;
		std::vector<Texture*> textures_;
		std::vector<Material> materials_;

	public:
		StaticMesh();
		~StaticMesh();

		void Create(const char* fileName);

		const VertexBuffer& GetVertexBuffer() const { return vertexBuffer_; }
		const IndexBuffer& GetIndexBuffer() const { return indexBuffer_; }
		uint32_t GetShapeNum() const { return static_cast<uint32_t>(shapes_.size()); }
		const Shape& GetShape(uint32_t index) const { return shapes_[index]; }
		uint32_t GetMaterialNum() const { static_cast<uint32_t>(materials_.size()); }
		const Material& GetMaterial(uint32_t index) const { return materials_[index]; }
	};

}