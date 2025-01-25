#include "../../../headers/Engine Core/Mesh/Model.h"

uint32_t Model::modelCount = 0;
std::vector<Model*> Model::modelList{};

Model::Model(
	std::string fileName, std::string texFolderPath, aiTextureType diffuseMap,
	aiTextureType normalMap, aiTextureType metallicMap, aiTextureType roughnessMap,
	aiTextureType occlusionMap, aiTextureType emissiveMap,
	bool isStrippedNormal, bool hasEmissiveMaterial, bool isPackedMap
)
	: Mesh(GL_CCW), texFolderPath{ texFolderPath }
{
	meshList.pop_back();
	meshCount--;

	modelList.push_back(this);
	modelID = modelCount++;

	diffuseTextures.clear();
	normalTextures.clear();
	roughnessTextures.clear();
	occlusionTextures.clear();
	metalnessTextures.clear();
	emissiveTextures.clear();

	useEmissiveMap = hasEmissiveMaterial;
	packedMaterialMap = isPackedMap;

	loadModel(fileName, diffuseMap, normalMap, metallicMap, roughnessMap, occlusionMap, emissiveMap);
	loadMesh(true, true, true, true, hasEmissiveMaterial, isStrippedNormal);
}

void Model::_updateMeshData(const aiScene* scene) {
	meshData.resize(scene->mNumMeshes);

	for (size_t i = 0; i < meshData.size(); i++) {
		meshData[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		meshData[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshData[i].baseVertex = vertexOffset;
		meshData[i].baseIndex = indexOffset;

		vertexOffset += scene->mMeshes[i]->mNumVertices;
		indexOffset += meshData[i].numIndices;
	}
}

void Model::_updateIndirectBuffer() {
	drawCommands.resize(meshData.size());

	for (size_t i = 0; i < meshData.size(); i++) {
		drawCommands[i].instancedCount = 1;
		drawCommands[i].baseInstance = 0;
		drawCommands[i].indexCount = meshData[i].numIndices;
		drawCommands[i].baseIndex = meshData[i].baseIndex;
		drawCommands[i].baseVertex = meshData[i].baseVertex;
	}

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawCommand) * drawCommands.size(), &drawCommands[0], GL_STATIC_DRAW);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void Model::_updateRenderData() {
	RenderData3D data{};
	int index = 0;

	for (const MeshMetaData& mesh : meshData) {
		data.diffuseMap = diffuseTextures[mesh.materialIndex]->getTextureHandle();
		data.normalMap = normalTextures[mesh.materialIndex]->getTextureHandle();
		data.metallicMap = metalnessTextures[mesh.materialIndex]->getTextureHandle();
		data.roughnessMap = (!packedMaterialMap) ? roughnessTextures[mesh.materialIndex]->getTextureHandle() : 0;
		data.occlusionMap = (!packedMaterialMap) ? occlusionTextures[mesh.materialIndex]->getTextureHandle() : 0;
		data.emissiveMap = (useEmissiveMap) ? emissiveTextures[mesh.materialIndex]->getTextureHandle() : 0;

		renderData.push_back(data);
	}

	glGenBuffers(1, &RSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, RSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RenderData3D) * renderData.size(), &renderData[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Model::_loadNode(aiNode* node, const aiScene* const scene) {
	for (size_t i = 0; i < node->mNumMeshes; i++)
		_loadMesh(scene->mMeshes[node->mMeshes[i]], scene);

	for (size_t i = 0; i < node->mNumChildren; i++)
		_loadNode(node->mChildren[i], scene);
}

void Model::_loadMesh(aiMesh* mesh, const aiScene* const scene) {
	Vertex vertex{};

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.texel = (mesh->mTextureCoords[0]) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
			: glm::vec2(0.f);
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

		vertices.push_back(vertex);
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
}

void Model::_loadMaterialMap(const aiScene* const scene, std::vector<Texture*>& maps, aiTextureType textureType) const {
	maps.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];
		maps[i] = nullptr;

		if (material->GetTextureCount(textureType)) {
			aiString path;

			if (material->GetTexture(textureType, 0, &path) == AI_SUCCESS) {
				size_t idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				size_t extIdx = filename.rfind(".");
				std::string extension = filename.substr(extIdx + 1);
				std::string texPath = texFolderPath + filename;

				maps[i] = new Texture(texPath);

				if (extension != "dds") {
					if (!maps[i]->loadTexture()) {
						std::cerr << "Failed to load material at: " << texPath << std::endl;

						delete maps[i];
						maps[i] = nullptr;
					}

					if (!maps[i]->makeBindless()) {
						std::cerr << "Failed to load texture to GPU memory: " << texPath << std::endl;
					}
				}
				else {
					if (!maps[i]->loadDDSTexture()) {
						std::cerr << "Failed to load material at: " << texPath << std::endl;

						delete maps[i];
						maps[i] = nullptr;
					}

					if (!maps[i]->makeBindless()) {
						std::cerr << "Failed to load texture to GPU memory: " << texPath << std::endl;
					}
				}
			}
		}

		if (maps[i] == nullptr) {
			maps[i] = new Texture("src/Content/Textures/General Textures/black.dds");
			maps[i]->loadDDSTexture();
			maps[i]->makeBindless();
		}
	}
}

void Model::loadModel(std::string fileName, aiTextureType diffuseMap, aiTextureType normalMap,
	aiTextureType metallicMap, aiTextureType roughnessMap, aiTextureType occlusionMap, aiTextureType emissiveMap)
{
	if (fileName == "") {
		std::cout << "No file path specified" << std::endl;
		return;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, ASSIMP_LOAD_FLAGS);

	if (!scene) {
		std::cerr << fileName + " failed to load" << std::endl;
		std::cerr << importer.GetErrorString() << std::endl;
		return;
	}

	_updateMeshData(scene);
	_updateIndirectBuffer();
	_loadNode(scene->mRootNode, scene);
	_loadMaterialMap(scene, diffuseTextures, diffuseMap);
	_loadMaterialMap(scene, normalTextures, normalMap);
	_loadMaterialMap(scene, metalnessTextures, metallicMap);

	if(!packedMaterialMap) {
		_loadMaterialMap(scene, roughnessTextures, roughnessMap);
		_loadMaterialMap(scene, occlusionTextures, occlusionMap);
	}

	if (useEmissiveMap)
		_loadMaterialMap(scene, emissiveTextures, emissiveMap);

	_updateRenderData();
}

void Model::drawModel(GLenum renderMode) {
	glFrontFace(windingOrder);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, RSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, RSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(RenderData3D) * renderData.size(), &renderData[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO);

	glMultiDrawElementsIndirect(renderMode, GL_UNSIGNED_INT, NULL, (GLsizei)drawCommands.size(), 0);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Model::renderModel(Shader& shader, GLenum renderMode) {
	shader.setMat4("model", model);

	shader.setUint("useDiffuseMap", useDiffuseMap);
	shader.setUint("useNormalMap", useNormalMap);
	shader.setUint("strippedNormalMap", strippedNormalMap);
	shader.setUint("useMaterialMap", useMaterialMap);
	shader.setUint("useEmissiveMap", useEmissiveMap);
	shader.setUint("isPackedMaterialMap", packedMaterialMap);
	shader.setUint("isModel", true);

	drawModel();
}

void Model::renderModelWithOutline(Shader& shader, Shader& outlineShader, GLenum renderMode) {
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);

	renderModel(shader, renderMode);

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//glDisable(GL_DEPTH_TEST);

	outlineShader.useShader();

	outlineShader.setMat4("model", glm::scale(model, glm::vec3(1.01f)));

	drawModel(GL_TRIANGLES);

	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glDisable(GL_STENCIL_TEST);
	//glEnable(GL_DEPTH_TEST);

	outlineShader.endShader();

	shader.useShader();
}

void Model::clearModel() {
	for (size_t i = 0; i < diffuseTextures.size(); i++) {
		if (diffuseTextures[i]) {
			delete diffuseTextures[i];
			diffuseTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < normalTextures.size(); i++) {
		if (normalTextures[i]) {
			delete normalTextures[i];
			normalTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < roughnessTextures.size(); i++) {
		if (roughnessTextures[i]) {
			delete roughnessTextures[i];
			roughnessTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < metalnessTextures.size(); i++) {
		if (metalnessTextures[i]) {
			delete metalnessTextures[i];
			metalnessTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < occlusionTextures.size(); i++) {
		if (occlusionTextures[i]) {
			delete occlusionTextures[i];
			occlusionTextures[i] = nullptr;
		}
	}

	for (size_t i = 0; i < emissiveTextures.size(); i++) {
		if (emissiveTextures[i]) {
			delete emissiveTextures[i];
			emissiveTextures[i] = nullptr;
		}
	}

	if (RSBO != 0) {
		glDeleteBuffers(1, &RSBO);
		RSBO = 0;
	}
}