#include "../../headers/Scene Core/LightSources.h"

LightSources::LightSources() {
	sourceMesh.loadMesh();
}

void LightSources::renderLightSources(DirectionalLight& directionalLight, const std::vector<PointLight*>& pointLights,
	const std::vector<SpotLight*>& spotLights, uint32_t pointLightCount, uint32_t spotLightCount)
{
	shader.useShader();

	glm::mat4 model = glm::mat4(1.f);

	shader.setFloat("intensity", 50.f);
	shader.setVec3("lightColor", directionalLight.getLightColor());

	glm::vec3 dirLightDirection = directionalLight.getLightDirection();

	model = glm::translate(model, dirLightDirection);
	model = glm::scale(model, glm::vec3(100.f));
	shader.setMat4("model", model);

	glFrontFace(GL_CCW);
	sourceMesh.renderMesh();

	for (uint32_t i = 0; i < pointLightCount; i++) {
		model = glm::mat4(1.f);
		shader.setVec3("lightColor", pointLights[i]->getLightColor());

		model = glm::translate(model, pointLights[i]->getPosition());
		model = glm::scale(model, glm::vec3(10.f));

		shader.setMat4("model", model);
		sourceMesh.renderMesh();
	}

	for (uint32_t i = 0; i < spotLightCount; i++) {
		model = glm::mat4(1.f);
		shader.setVec3("lightColor", spotLights[i]->getLightColor());

		model = glm::translate(model, spotLights[i]->getPosition());
		model = glm::scale(model, glm::vec3(10.f));

		shader.setMat4("model", model);
		sourceMesh.renderMesh();
	}

	shader.endShader();
}