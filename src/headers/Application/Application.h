#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#pragma once

#include "../Engine Core/Skybox.h"
#include "../Engine Core/Editor/Overlay.h"
#include "../Engine Core/HDR.h"
#include "../Engine Core/Post Processing/BloomRenderer.h"
#include "../Engine Core/MouseSelector.h"
#include "../Scene Core/CoordinateSystem.h"
#include "../Engine Core/Mesh/Geometry/Cube.h"
#include "../Engine Core/Mesh/Geometry/UVSphere.h"
#include "../Engine Core/Mesh/Geometry/Icosphere.h"
#include "../Engine Core/Mesh/Terrain/Terrain.h"
#include "../Engine Core/GBuffer.h"
#include "../Engine Core/Post Processing/SSAO.h"
#include "../Scene Core/LightSources.h"
#include "../Engine Core/Shadows/CascadedShadows.h"
#include "../Engine Core/Lights/DirectionalLight.h"
#include "../Engine Core/Lights/SpotLight.h"
#include "../Engine Core/Particle System/ParticleSystem.h"
#include "../Engine Core/Post Processing/PostProcessMaster.h"
#include "../Scene Core/Grid.h"

class Application {
private:
    bool drawSkybox = true;
    bool enableBloom = true;
    bool drawWireframe = false;
    bool enableShadows = false;
    bool enableSSAO = false;
    bool enableHDR = true;
    bool displayCoordinateSystem = false;
    bool displayGrid = false;

    float exposure = 1.f;
    float filterRadius = 0.005f;
    float shadowRadius = 2.f;
    float bloomThreshold = 1.f;
    float ssaoRadius = 50.f;
    float ssaoBias = 2.f;
    float ssaoOcclusionPower = 10.f;

    float deltaTime = 0.f;
    float lastTime = 0.f;
    float currTime = 0.f;
    float prevTime = 0.f;
    float elapsedTime = 0.f;

    glm::mat4 viewportMatrix{ 1.f };

    Quad quad;

    glm::vec2 mouseClickCoords{ 0.f };

    int windowWidth{ 0 }, windowHeight{ 0 };

    Window window{};
    Camera camera{ {-300, 500, 0}, window.getBufferWidth(), window.getBufferHeight() };
    Overlay overlay{ window };
    HDR hdrBuffer;
    Grid grid{};
    MouseSelector selection{ window.getBufferWidth(), window.getBufferHeight() };
    CoordinateSystem coordSystem;
    Skybox skybox{ window.getBufferWidth(), window.getBufferHeight() };
    DirectionalLight skylight{ 0.1f, 0.5f, { 4000.f, 4000.f, 0.f }, { 1.f, 1.f, 1.f } };
    LightSources lightSources{};
    CascadedShadows csm{ window.getBufferWidth(), window.getBufferHeight(), 0.5f, 20, 6, 6 };
    GBuffer gbuffer{ window.getBufferWidth(), window.getBufferHeight() };
    SSAO ssao{ window.getBufferWidth(), window.getBufferHeight() };
    PostProcessMaster postProcess{ window.getBufferWidth(), window.getBufferHeight() };

    std::vector<PointLight*> pointLights{ ::MAX_POINT_LIGHTS, nullptr };
    std::vector<SpotLight*> spotLights{ ::MAX_SPOT_LIGHTS, nullptr };

    uint32_t pointLightCount = 0;
    uint32_t spotLightCount = 0;

    uint32_t currentFramebuffer = 0;

    int index{ -1 };

    Shader forwardShader{ "src/shaders/PBR_forward.vert", "src/shaders/PBR_forward.frag", "src/shaders/PBR_forward.geom" };
    Shader deferredShader{ "src/shaders/PBR_deferred.vert", "src/shaders/PBR_deferred.frag" };
    Shader outlineShader{ "src/shaders/highlight.vert", "src/shaders/highlight.frag" };

    void setLightingUniforms(Shader& shader);
    void setGlobalPBRUniforms(Shader& shader);

    void mainLoopForward(ParticleSystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition, glm::vec3& lightDirection);
    void mainLoopDeferred(ParticleSystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition, glm::vec3& lightDirection);

public:
    Application();

    void start();

    ~Application() = default;
};

#endif