#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#pragma once

#include "Camera.h"
#include "Texture.h"
#include "Shader.h"
#include "../Application/AttriblessRendering.h"

class Skybox : public Texture {
private:
    static const int CUBEMAP_RESOLUTION = 1024;
    static const int CONVOLUTION_RESOLUTION = 32;
    static const int PREFILTER_RESOLUTION = 512;

    uint32_t VAO{ 0 }, VBO{ 0 };

    float vertices[108] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 viewMatrices[6] = {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    Shader skyboxShader{ "src/shaders/skybox.vert", "src/shaders/skybox.frag" };
    Shader hdrToCubeShader{ "src/shaders/HDRskybox.vert", "src/shaders/HDRskybox.frag" };
    Shader irradianceShader{ "src/shaders/HDRskybox.vert", "src/shaders/irradiance.frag" };
    Shader prefilterShader{ "src/shaders/HDRskybox.vert", "src/shaders/prefilter.frag" };
    Shader brdfShader{ "src/shaders/brdf.vert", "src/shaders/brdf.frag" };

    uint32_t FBO{ 0 }, RBO{ 0 }, environmentMap{ 0 };
    uint32_t irradianceMap{ 0 }, prefilterMap{ 0 }, brdfTexture{ 0 };

    void renderCube() const {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    void clearBuffers();

    void _initFBO();
    void _generateCubemap();
    void _generateIrradianceMap();
    void _generatePrefilterMipmap();
    void _generateBRDFMap();

public:
    Skybox(
        int windowWidth, int windowHeight,
        const char* fileName = "src/Content/Textures/skybox/village_cloudy_sky_dome_4k.hdr"
    );

    void loadEquirectangularMap(const char* file_name);
    void renderSkybox();

    uint32_t getIrradianceMap() const { return irradianceMap; }
    uint32_t getBRDFTexture() const { return brdfTexture; };
    uint32_t getPrefilterTexture() const { return prefilterMap; }

    ~Skybox() {
        clearBuffers();
    }
};

#endif