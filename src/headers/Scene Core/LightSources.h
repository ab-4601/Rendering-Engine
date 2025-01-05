#ifndef _LIGHT_SOURCES_H_
#define _LIGHT_SOURCES_H_

#include <vector>
#include "../Core Includes/GLM_Core.h"
#include "../Core Includes/GL_Core.h"
#include "../Engine Core/Lights/DirectionalLight.h"
#include "../Engine Core/Lights/PointLight.h"
#include "../Engine Core/Lights/SpotLight.h"
#include "../Engine Core/Shader.h"

struct LightMesh {
    uint32_t VAO{ 0 }, VBO{ 0 };

    std::vector<float> vertices = {
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // front face
        -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        // left face
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        // right face
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        // bottom face
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        // top face
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f , 1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
    };

    void loadMesh() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void renderMesh() const {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

class LightSources {
private:
    Shader shader{ "src/shaders/light.vert", "src/shaders/light.frag" };
    LightMesh sourceMesh;

public:
    LightSources();

    void renderLightSources(DirectionalLight& directionalLight, const std::vector<PointLight*>& pointLights,
        const std::vector<SpotLight*>& spotLights, uint32_t pointLightCount, uint32_t spotLightCount);

    ~LightSources() = default;
};

#endif