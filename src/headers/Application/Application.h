#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "../Engine Core/Window.h"
#include "../Engine Core/Skybox.h"
#include "../Engine Core/Editor/EngineEditor.h"
#include "../Engine Core/Lights/SpotLight.h"
#include "../Engine Core/Mesh/Geometry/Cube.h"
#include "../Engine Core/Mesh/Terrain/Terrain.h"
#include "../Engine Core/Mesh/Geometry/UVSphere.h"
#include "../Engine Core/Mesh/Geometry/Icosphere.h"
#include "../Engine Core/Shadows/CascadedShadows.h"
#include "../Engine Core/Lights/DirectionalLight.h"
#include "../Particle System TF/ParticleSystemTF.h"
#include "../Engine Core/Particle System/ParticleSystem.h"
#include "../Engine Core/Fluid Simulation/FluidSimulation.h"

#include "../Scene Core/Grid.h"
#include "../Scene Core/LightSources.h"
#include "../Scene Core/CoordinateSystem.h"

enum LIGHTING_SAMPLERS : int{ IRRADIANCE = 1, PREFILTER = 2, BRDF = 3, POINT_SHADOW = 4, CASCADED_SHADOW = 5, OFFSETS = 6 };
enum DEFERRED_SHADER : int { POSITION_BUFFER = 7, ALBEDO_BUFFER = 8, NORMAL_BUFFER = 9, MATERIAL_BUFFER = 10, OCCLUSION_BUFFER = 11 };

class Application {
private:
    int bufferWidth{ 0 }, bufferHeight{ 0 };

    Window window{};
    Skybox skybox{ window.getBufferWidth(), window.getBufferHeight() };
    DirectionalLight skylight{ 0.1f, 0.5f, { 4000.f, 4000.f, 0.f }, { 1.f, 1.f, 1.f } };
    LightSources lightSources;
    CascadedShadows csm{ window.getBufferWidth(), window.getBufferHeight(), 0.5f, 20, 8, 16 };

    std::vector<PointLight*> pointLights{ ::MAX_POINT_LIGHTS, nullptr };
    std::vector<SpotLight*> spotLights{ ::MAX_SPOT_LIGHTS, nullptr };

    uint32_t pointLightCount = 0, spotLightCount = 0;
    uint32_t currentFramebuffer = 0;

    Shader forwardShader{ "src/shaders/PBR_forward.vert", "src/shaders/PBR_forward.frag", "src/shaders/PBR_forward.geom" };
    Shader deferredShader{ "src/shaders/PBR_deferred.vert", "src/shaders/PBR_deferred.frag" };
    Shader outlineShader{ "src/shaders/outline.vert", "src/shaders/outline.frag" };
    Shader raymarchingShader{ "src/shaders/raymarching.vert", "src/shaders/raymarching.frag" };

    void setLightingUniforms(Shader& shader);
    void setGlobalPBRUniforms(Shader& shader);

    void rayMarchingLoop();
    void fluidSimulation();
    void mainLoopForward(Psystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition);
    void mainLoopDeferred(ParticleSystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition);

public:
    Application();

    void start();

    ~Application() {
        glDeleteBuffers(1, &Mesh::RenderDataBufferObject);
        Mesh::RenderDataBufferObject = 0;
    }
};

#endif