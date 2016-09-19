#include "se/Graphics/StaticMesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdparty/tinyobjloader/tiny_obj_loader.h"

namespace se 
{
	StaticMesh::StaticMesh()
	{
	}

	StaticMesh::~StaticMesh()
	{
		for (auto& m : materials_) {
			delete m.albedo;
		}
	}

	void StaticMesh::Create(const char* fileName)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		// ロード
		Printf("Loading %s\n", fileName);
		char baseDir[255];
		_splitpath_s(fileName, nullptr, 0, baseDir, sizeof(baseDir), nullptr, 0, nullptr, 0);
		std::string err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fileName, baseDir, true)) {
			if (!err.empty()) {
				Printf(err.c_str());
			}
			Printf("Failed to load/parse .obj.\n");
			return;
		}
		//Printf("# of vertices  : %d\n", (attrib.vertices.size() / 3));
		//Printf("# of normals   : %d\n", (attrib.normals.size() / 3));
		//Printf("# of texcoords : %d\n", (attrib.texcoords.size() / 2));
		//Printf("# of shapes    : %d\n", shapes.size());
		//Printf("# of materials : %d\n", materials.size());

		// 現状はすべてのシェイプで頂点構造が一致している前提で処理しているため1つの頂点バッファにすべて入っている
		// TODO:tangent, bitangent計算
		uint32_t vtxAttrs = VERTEX_ATTR_FLAG_POSITION;
		vtxAttrs |= (attrib.normals.size() > 0) ? VERTEX_ATTR_FLAG_NORMAL : 0;
		vtxAttrs |= (attrib.texcoords.size() > 0) ? VERTEX_ATTR_FLAG_TEXCOORD0 : 0;
		uint32_t vtxStride = ComputeVertexStride(vtxAttrs);

		// 全インデックス数を計算
		uint32_t totalIndexNum = 0;
		for (auto& shape : shapes) {
			totalIndexNum += static_cast<uint32_t>(shape.mesh.indices.size());
		}

		// 頂点をインデックス展開するためトータルインデックス数分の頂点バッファを確保
		std::unique_ptr<uint8_t> vertexData(new uint8_t[vtxStride * totalIndexNum]);
		std::unique_ptr<uint8_t> indexData(new uint8_t[sizeof(uint32_t) * totalIndexNum]);
		uint32_t currentIndex = 0;
		uint32_t* currentIndexBufferPtr = reinterpret_cast<uint32_t*>(indexData.get());
		for (uint32_t i = 0; i < shapes.size(); i++) {
			auto& shape = shapes[i];
			Assert(shape.mesh.num_face_vertices.size() == shape.mesh.material_ids.size());

			// For each face
			uint32_t currentMaterial = 0xffffffff;
			Shape currentShape;
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
				Assert(shape.mesh.num_face_vertices[f] == 3);	// 三角形化済みのはず

				// マテリアルの切り替わり判定
				if (currentMaterial != shape.mesh.material_ids[f]) {
					if (currentMaterial != 0xffffffff) {
						currentShape.indexCount = currentIndex - currentShape.indexStart;
						shapes_.push_back(currentShape);
					}
					currentMaterial = shape.mesh.material_ids[f];
					currentShape.materialIndex = currentMaterial;
					currentShape.indexStart = currentIndex;
					currentShape.indexCount = 0;
				}

				// For each vertex in the face
				uint32_t ptrOffset = 0;
				uint8_t* currentVertexBufferPtr = vertexData.get() + (vtxStride * currentIndex);
				for (size_t v = 0; v < 3; v++) {
					tinyobj::index_t idx = shape.mesh.indices[(f * 3) + v];

					// position
					memcpy(currentVertexBufferPtr + ptrOffset, &attrib.vertices[idx.vertex_index * 3], 12);
					ptrOffset += 12;

					// normal
					if (vtxAttrs & VERTEX_ATTR_FLAG_NORMAL) {
						memcpy(currentVertexBufferPtr + ptrOffset, &attrib.normals[idx.normal_index * 3], 12);
						ptrOffset += 12;
					}

					// texcoord
					if (vtxAttrs & VERTEX_ATTR_FLAG_TEXCOORD0) {
						float2* uv = reinterpret_cast<float2*>(currentVertexBufferPtr + ptrOffset);
						uv->x = attrib.texcoords[idx.texcoord_index * 2 + 0];
						uv->y = 1.0f - attrib.texcoords[idx.texcoord_index * 2 + 1]; // OpenGL -> DirectX
						ptrOffset += 8;
					}

					currentIndexBufferPtr[currentIndex] = currentIndex;
					currentIndex++;
				}
			}

			// 最後のマテリアル分追加
			currentShape.indexCount = currentIndex - currentShape.indexStart;
			shapes_.push_back(currentShape);
		}

		vertexBuffer_.Create(vertexData.get(), vtxStride * totalIndexNum, vtxAttrs);
		indexBuffer_.Create(indexData.get(), sizeof(uint32_t) * totalIndexNum, INDEX_BUFFER_STRIDE_U32);

		// マテリアル
		materials_.resize(materials.size());
		std::unordered_map<std::string, uint32_t> textureMap;
		for (size_t i = 0; i < (uint32_t)materials_.size(); i++) {
			auto& m = materials[i];
			ZeroMemory(&materials_[i], sizeof(materials_[i]));

			// テクスチャ検索
			if (m.diffuse_texname.size() > 0) {
				Texture* texture = nullptr;
				auto iter = textureMap.find(m.diffuse_texname);
				if (iter != textureMap.end()) {
					texture = textures_[iter->second];
				} else {
					texture = new Texture();
					char path[255];
					snprintf(path, sizeof(path), "%s%s", baseDir, m.diffuse_texname.c_str());
					texture->LoadFromFile(path);
					textures_.push_back(texture);
					uint32_t index = (uint32_t)textures_.size() - 1;
					textureMap[m.diffuse_texname] = index;

				}
				materials_[i].albedo = texture;
			}
		}
	}
}