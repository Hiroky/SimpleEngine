#include "se/Graphics/StaticMesh.h"
#include <fstream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdparty/tinyobjloader/tiny_obj_loader.h"

namespace se 
{
	namespace {
		struct CacheHeader
		{
			uint16_t shapeNum;
			uint16_t materialNum;
			uint16_t vertexAttrs;
			uint32_t vertexNum;
			uint32_t offsetToShapes;
			uint32_t offsetToVertices;
			uint32_t offsetToIndeces;
			uint32_t offsetToMaterial;
		};
	}

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
		char baseDir[256];
		char fileNameWoExt[256];
		char tempPath[256];
		_splitpath_s(fileName, nullptr, 0, baseDir, sizeof(baseDir), fileNameWoExt, sizeof(baseDir), nullptr, 0);

		// キャッシュファイル名
		snprintf(tempPath, sizeof(tempPath), "%s%s.cache", baseDir, fileNameWoExt);
		std::ifstream cacheFile(tempPath, std::ios::in | std::ios::binary);
		if (cacheFile) {
			/** キャッシュから読み込み **/

			// ファイル全体をリード
			cacheFile.seekg(0, std::fstream::end);
			uint32_t eofPos = (uint32_t)cacheFile.tellg();
			cacheFile.clear();
			cacheFile.seekg(0, std::fstream::beg);
			uint32_t begPos = (uint32_t)cacheFile.tellg();
			uint32_t size = eofPos - begPos;
			std::unique_ptr<uint8_t> cacheData(new uint8_t[size]);
			cacheFile.read((char*)cacheData.get(), size);

			// パース
			const CacheHeader* header = (CacheHeader*)cacheData.get();
			const Shape* shapes = (Shape*)(cacheData.get() + header->offsetToShapes);
			shapes_.resize(header->shapeNum);
			memcpy(&shapes_[0], shapes, sizeof(Shape) * header->shapeNum);

			uint32_t vtxStride = ComputeVertexStride(header->vertexAttrs);
			vertexBuffer_.Create(cacheData.get() + header->offsetToVertices, vtxStride * header->vertexNum, header->vertexAttrs);
			indexBuffer_.Create(cacheData.get() + header->offsetToIndeces, sizeof(uint32_t) * header->vertexNum, INDEX_BUFFER_STRIDE_U32);

			const char* materials = (const char*)(cacheData.get() + header->offsetToMaterial);
			materials_.resize(header->materialNum);
			std::unordered_map<std::string, uint32_t> textureMap;
			for (size_t i = 0; i < (uint32_t)materials_.size(); i++) {
				const char* m = materials + (256 * i);
				ZeroMemory(&materials_[i], sizeof(materials_[i]));

				// テクスチャ検索
				if (strlen(m) > 0) {
					Texture* texture = nullptr;
					auto iter = textureMap.find(m);
					if (iter != textureMap.end()) {
						texture = textures_[iter->second];
					} else {
						texture = new Texture();
						snprintf(tempPath, sizeof(tempPath), "%s%s", baseDir, m);
						texture->LoadFromFile(tempPath);
						textures_.push_back(texture);
						uint32_t index = (uint32_t)textures_.size() - 1;
						textureMap[m] = index;

					}
					materials_[i].albedo = texture;
				}
			}
		} else {
			/** キャッシュがなかったらobjを読み込み **/
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;

			Printf("Loading %s\n", fileName);
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
						snprintf(tempPath, sizeof(tempPath), "%s%s", baseDir, m.diffuse_texname.c_str());
						texture->LoadFromFile(tempPath);
						textures_.push_back(texture);
						uint32_t index = (uint32_t)textures_.size() - 1;
						textureMap[m.diffuse_texname] = index;

					}
					materials_[i].albedo = texture;
				}
			}

			// 読み込み高速化のためのキャッシュデータを生成
			{
				snprintf(tempPath, sizeof(tempPath), "%s%s.cache", baseDir, fileNameWoExt);
				std::ofstream file(tempPath, std::ios::out | std::ios::binary);

				CacheHeader header;
				header.shapeNum = (uint16_t)shapes_.size();
				header.materialNum = (uint16_t)materials_.size();
				header.vertexAttrs = (uint16_t)vtxAttrs;
				header.vertexNum = totalIndexNum;
				header.offsetToShapes = sizeof(CacheHeader);
				header.offsetToVertices = header.offsetToShapes + sizeof(Shape) * header.shapeNum;
				header.offsetToIndeces = header.offsetToVertices + (vtxStride * totalIndexNum);
				header.offsetToMaterial = header.offsetToIndeces + (sizeof(uint32_t) * totalIndexNum);
				file.write((char*)&header, sizeof(header));

				// シェイプ
				for (auto& s : shapes_) {
					file.write((char*)&s, sizeof(s));
				}
				// 頂点
				file.write((char*)vertexData.get(), vtxStride * totalIndexNum);
				// インデックス
				file.write((char*)indexData.get(), sizeof(uint32_t) * totalIndexNum);
				// マテリアル
				for (auto& m : materials) {
					FillMemory(tempPath, sizeof(tempPath), 0);
					snprintf(tempPath, sizeof(tempPath), "%s", m.diffuse_texname.c_str());
					file.write(tempPath, sizeof(tempPath));
				}
			}
		}
	}
}