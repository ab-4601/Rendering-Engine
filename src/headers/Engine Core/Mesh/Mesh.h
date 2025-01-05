#ifndef _MESH_H_
#define _MESH_H_

#include "../Shader.h"
#include "../Camera.h"
#include "../Texture.h"

struct Vertex {
	glm::vec3 position;
	glm::vec2 texel;
	glm::vec3 normal;
	glm::vec3 tangent;

	Vertex(
		glm::vec3 position = glm::vec3(0.f), glm::vec2 texel = glm::vec2(0.f),
		glm::vec3 normal = glm::vec3(0.f), glm::vec3 tangent = glm::vec3(0.f)
	) : position{ position }, texel{ texel }, normal{ normal }, tangent{ tangent } {
	}
};

struct MeshMetaData {
	uint32_t baseVertex = 0;
	uint32_t baseIndex = 0;
	uint32_t numIndices = 0;
	uint32_t materialIndex = 0;
};

struct DrawCommand {
	uint32_t indexCount = 0;
	uint32_t instancedCount = 0;
	uint32_t baseIndex = 0;
	uint32_t baseVertex = 0;
	uint32_t baseInstance = 0;
};

struct RenderData3D {
	int meshIndex = 0;
	uint64_t diffuseMap = 0;
	uint64_t normalMap = 0;
	uint64_t metallicMap = 0;
	uint64_t emissiveMap = 0;
};

class Mesh {
protected:
	static uint32_t meshCount;
	uint32_t objectID{ 0 };

	glm::vec3 color{ 1.f };
	glm::mat4 model{ 1.f };
	glm::mat4 outlineModel{ 1.f };

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<MeshMetaData> meshData;
	std::vector<DrawCommand> drawCommands;
	std::vector<RenderData3D> renderData;

	GLenum windingOrder;
	uint32_t indexOffset{ 0 }, vertexOffset{ 0 };
	float metallic{ 0.f }, roughness{ 0.f }, ao{ 0.f };

	uint32_t VAO{ 0 };
	uint32_t VBO{ 0 };
	uint32_t EBO{ 0 };
	uint32_t IBO{ 0 };

	Texture* diffuseMap = nullptr;
	Texture* normalMap = nullptr;
	Texture* depthMap = nullptr;
	Texture* metallicMap = nullptr;
	Texture* roughnessMap = nullptr;

	bool useDiffuseMap{ false };
	bool useNormalMap{ false };
	bool strippedNormalMap{ false };
	bool useMaterialMap{ false };
	bool useEmissiveMap{ false };
	bool drawIndexed{ true };
	bool calcShadows{ false };
	bool enableSSAO{ false };

public:
	static std::vector<Mesh*> meshList;

	Mesh(GLenum windingOrder = GL_CCW);

	void setVertices(const std::vector<Vertex>& vertices) { this->vertices = vertices; }
	void setIndices(const std::vector<uint32_t>& indices) { this->indices = indices; }

	const std::vector<Vertex>& getVertices() const { return vertices; }
	const std::vector<uint32_t>& getIndices() const { return indices; }
	const std::vector<MeshMetaData>& getMeshData() const { return meshData; }
	const std::vector<RenderData3D>& getRenderData() const { return renderData; }
	const std::vector<DrawCommand>& getDrawCommands() const { return drawCommands; }

	glm::mat4& getModelMatrix() { return model; }
	glm::vec3 getColor() const { return color; }
	uint32_t getObjectID() const { return objectID; }
	float getMetallic() const { return metallic; }
	float getRoughness() const { return roughness; }
	float getAO() const { return ao; }
	bool isDrawIndexed() const { return drawIndexed; }

	bool getDiffuseMapBool() const { return useDiffuseMap; }
	bool getNormalMapBool() const { return useNormalMap; }
	bool getMaterialMapBool() const { return useMaterialMap; }
	bool getStrippedNormalBool() const { return strippedNormalMap; }
	GLenum getWindingOrder() const { return windingOrder; }

	void setDiffuseMap(Texture* materialMap) { diffuseMap = materialMap; }
	void setNormalMap(Texture* materialMap) { normalMap = materialMap; }
	void setMetallicMap(Texture* materialMap) { metallicMap = materialMap; }
	void setRoughnessMap(Texture* materialMap) { roughnessMap = materialMap; }

	void bindDiffuseMap() const { diffuseMap->useTexture(GL_TEXTURE0); }
	void bindNormalMap() const { normalMap->useTexture(GL_TEXTURE1); }
	void bindRoughnessMap() const { roughnessMap->useTexture(GL_TEXTURE2); }
	void bindMetallicMap() const { metallicMap->useTexture(GL_TEXTURE3); }

	void setModelMatrix(const glm::mat4& matrix) { this->model = matrix; }
	void setColor(glm::vec3 color) { this->color = color; }
	void setObjectID(int objectID) { this->objectID = objectID; }
	void setShadowBoolUniform(bool calcShadows) { this->calcShadows = calcShadows; }
	void setSSAOboolUniform(bool enableSSAO) { this->enableSSAO = enableSSAO; }

	void loadMesh(
		bool useDiffuseMap = false, bool drawIndexed = true,
		bool useNormalMap = false, bool useMaterialMap = false,
		bool useEmissiveMap = false, bool isStrippedNormal = false
	);

	void drawMesh(GLenum renderMode);
	void renderMesh(Shader& shader, GLenum renderMode = GL_TRIANGLES);

	void renderMeshWithOutline(Shader& shader, Shader& outlineShader, GLenum renderMode = GL_TRIANGLES);

	void setMeshMaterial(float roughness, float metallic, float ao);

	void clearMesh();

	~Mesh() {
		clearMesh();
	}
};

#endif