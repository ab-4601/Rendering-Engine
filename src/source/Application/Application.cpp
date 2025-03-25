#include "../../headers/Application/Application.h"
#include "../../headers/Application/AttriblessRendering.h"

float currTime = 0.f;
float deltaTime = 0.f;
float lastTime = 0.f;
float prevTime = 0.f;
float elapsedTime = 0.f;

Application::Application() {
    Mesh::meshList.clear();
    Model::modelList.clear();

    bufferWidth = window.getBufferWidth();
    bufferHeight = window.getBufferHeight();

    MouseSelector::EntitySelector().resize(bufferWidth, bufferHeight);

    glGenBuffers(1, &Mesh::RenderDataBufferObject);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, Mesh::RenderDataBufferObject);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, Mesh::RenderDataBufferObject);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NULL, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Application::setLightingUniforms(Shader& shader) {
    shader.setVec3("directionalLight.base.color", skylight.getColor());
    shader.setVec3("directionalLight.direction", skylight.getLightDirection());

    if (pointLightCount > ::MAX_POINT_LIGHTS)
        pointLightCount = ::MAX_POINT_LIGHTS;

    if (spotLightCount > ::MAX_SPOT_LIGHTS)
        spotLightCount = ::MAX_SPOT_LIGHTS;

    shader.setUint("pointLightCount", pointLightCount);
    shader.setUint("spotLightCount", spotLightCount);

    char buffer[64]{ '\0' };

    for (uint i = 0; i < pointLightCount; i++) {
        snprintf(buffer, sizeof(buffer), "pointLights[%i].base.color", i);
        shader.setVec3(buffer, pointLights[i]->getColor());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].position", i);
        shader.setVec3(buffer, pointLights[i]->getPosition());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].constant", i);
        shader.setFloat(buffer, pointLights[i]->getAttenuationConstant());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].linear", i);
        shader.setFloat(buffer, pointLights[i]->getAttenuationLinear());

        snprintf(buffer, sizeof(buffer), "pointLights[%i].exponent", i);
        shader.setFloat(buffer, pointLights[i]->getAttenuationExponent());
    }

    for (uint i = 0; i < spotLightCount; i++) {
        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.base.color", i);
        shader.setVec3(buffer, spotLights[i]->getColor());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.position", i);
        shader.setVec3(&buffer[0], spotLights[i]->getPosition());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.constant", i);
        shader.setFloat(&buffer[0], spotLights[i]->getAttenuationConstant());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.linear", i);
        shader.setFloat(&buffer[0], spotLights[i]->getAttenuationLinear());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].base.exponent", i);
        shader.setFloat(&buffer[0], spotLights[i]->getAttenuationExponent());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].direction", i);
        shader.setVec3(&buffer[0], spotLights[i]->getDirection());

        snprintf(buffer, sizeof(buffer), "spotLights[%i].edge", i);
        shader.setFloat(&buffer[0], spotLights[i]->getProcEdge());
    }
}

void Application::setGlobalPBRUniforms(Shader& shader) {
    shader.setFloat("nearPlane", ::near_plane);
    shader.setFloat("farPlane", ::far_plane);
    shader.setInt("cascadeCount", csm.getNumCascades());
    shader.setVec3("offsetTexSize", csm.getNoiseTextureSize());
    shader.setMat4("viewportMatrix", EngineEditor::Editor().viewportMatrix);

    const float* cascadePlanes = csm.cascadePlanes();

    char buffer[64]{ '\0' };

    for (int i = 0; i < ::MAX_CASCADES; i++) {
        snprintf(buffer, sizeof(buffer), "cascadePlanes[%i]", i);
        shader.setFloat(buffer, cascadePlanes[i]);
    }

    glBindTextureUnit(LIGHTING_SAMPLERS::IRRADIANCE, skybox.getIrradianceMap());
    glBindTextureUnit(LIGHTING_SAMPLERS::PREFILTER, skybox.getPrefilterTexture());
    glBindTextureUnit(LIGHTING_SAMPLERS::BRDF, skybox.getBRDFTexture());
    glBindTextureUnit(LIGHTING_SAMPLERS::POINT_SHADOW, 0);
    glBindTextureUnit(LIGHTING_SAMPLERS::CASCADED_SHADOW, csm.getShadowMaps());
    glBindTextureUnit(LIGHTING_SAMPLERS::OFFSETS, csm.noiseBuffer());
}

void Application::start() {
    /*glm::vec3 pointLightPosition1(20.f, 20.f, 20.f);
    glm::vec3 pointLightPosition2(100.f, 30.f, 100.f);
    glm::vec3 spotLightPosition(300.f, 80.f, 300.f);

    PointLight pointLight1{ 1.f, 1.f, pointLightPosition1, 0.01f, 0.01f, 0.01f, glm::vec3(300.f) };
    pointLights[0] = &pointLight1;
    pointLightCount++;

    SpotLight spotLight1{
        1.f, 1.f, spotLightPosition, 0.001f, 0.001f, 0.001f, glm::vec3(100.f, 500.f, 500.f),
        glm::vec3(0.f, -1.f, 0.f), 45.f
    };
    spotLights[0] = &spotLight1;
    spotLightCount++;*/

    EngineEditor::Editor()._init(window);

    glm::mat4 modelMatrix{ 1.f };
    modelMatrix = glm::translate(modelMatrix, glm::vec3(400.f, 100.f, 0.f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.f));

    Icosphere sphere{ GL_CW, 5 };
    sphere.setColor({ 1.f, 0.07f, 0.07f });
    sphere.setMeshMaterial(0.f, 0.f, 1.f);
    sphere.setModelMatrix(modelMatrix);
    sphere.loadMesh();

    /*UVSphere sphere{ 100, 100, 1.f };
    sphere.setColor({ 0.07f, 1.f, 1.f });
    sphere.setMeshMaterial(0.f, 0.f, 1.f);
    sphere.setModelMatrix(modelMatrix);
    sphere.loadMesh();*/

    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 100.f, 0.f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.f));

    Cube cube{};
    cube.setColor({ 0.07f, 1.f, 1.f });
    cube.setModelMatrix(modelMatrix);
    cube.setMeshMaterial(0.f, 0.f, 1.f);;
    cube.loadMesh(false, false);

    /*modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(5.f, 5.f, 5.f));

    Terrain terrain(500, 500, 90);
    terrain.generateHeightMaps(5);
    terrain.setMeshMaterial(0.f, 0.f, 1.f);
    terrain.generateTerrain();
    terrain.loadMesh();
    terrain.setModelMatrix(modelMatrix);
    terrain.setColor(glm::vec3(0.2f, 0.2f, 0.2f));*/

    /*Model suntemple(
        "src/Content/Models/SunTemple/SunTemple.fbx",
        "src/Content/Models/SunTemple/Textures/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_SPECULAR,
        aiTextureType_DIFFUSE_ROUGHNESS,
        aiTextureType_AMBIENT_OCCLUSION,
        aiTextureType_EMISSIVE,
        true, true
    );*/

    /*modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(200.f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(45.f), glm::vec3(0.f, 0.f, 1.f));

    Model isometricRoom(
        "src/Content/Models/IsometricRoom/Isometric Room v5058 p4866.fbx",
        "src/Content/Models/IsometricRoom/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_METALNESS,
        aiTextureType_SHININESS,
        aiTextureType_AMBIENT,
        aiTextureType_EMISSIVE,
        false, true, false
    );

    isometricRoom.setModelMatrix(modelMatrix);*/

    Model sponza(
        "src/Content/Models/Sponza/Sponza.gltf",
        "src/Content/Models/Sponza/"
    );

    CoordinateSystem::Widget().createCoordinateSystem();

    ParticleTexture partTex("src/Content/Textures/Particle System/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10, 15.f, 1.f, 6.f, partTex);

    ParticleTexture fire("src/Content/Textures/Particle System/fire.png", 8.f);
    glm::vec3 fireParticlePosition{ 1125.f, 120.f, 400.f };
    ParticleSystem fireSystem(particleColor, 30, -30.f, 1.f, 30.f, fire);

    Psystem test{ glm::vec3(0.f) };

    HDR::Framebuffer().enableMultisampling();
    HDR::Framebuffer()._init(window.getBufferWidth(), window.getBufferHeight());
    PostProcessMaster::Framebuffer()._init(window.getBufferWidth(), window.getBufferHeight());

    initFluidSimulation();

    glfwSwapInterval(EngineEditor::Editor().enableVsync);
    glClearColor(0.4f, 0.4f, 0.4f, 1.f);

    while (!glfwWindowShouldClose(window.getGlfwWindow())) {
        glfwPollEvents();

        currTime = (float)glfwGetTime();
        deltaTime = currTime - lastTime;
        elapsedTime += deltaTime;
        lastTime = currTime;

        if (window.getBufferWidth() == 0 || window.getBufferHeight() == 0)
            continue;

        if (window.getBufferWidth() != bufferWidth || window.getBufferHeight() != bufferHeight) {
            bufferWidth = window.getBufferWidth();
            bufferHeight = window.getBufferHeight();

            MouseSelector::EntitySelector().resize(bufferWidth, bufferHeight);
        }

        EngineEditor::Editor().EditorCamera.update(window);

        pSystem.updateParticles(EngineEditor::Editor().EditorCamera.getCameraPosition());
        //fireSystem.updateParticles(Overlay::Editor().EditorCamera.getCameraPosition());

        if(elapsedTime >= EngineEditor::Editor().frameRateCap) {
            if (EngineEditor::Editor().enableHDR) {
                HDR::Framebuffer().enableHDRWriting();
                currentFramebuffer = HDR::Framebuffer().FramebufferID();
            }
            else {
                PostProcessMaster::Framebuffer().enableWriting();
                currentFramebuffer = PostProcessMaster::Framebuffer().FramebufferID();
            }

            EngineEditor::Editor().newFrame();
            elapsedTime = 0.f;

            //fluidSimulation();
            //rayMarchingLoop();
            //mainLoopForward(test, modelMatrix, fireParticlePosition);
            mainLoopDeferred(pSystem, modelMatrix, fireParticlePosition);

            EngineEditor::Editor().Render(window.getGlfwWindow());

            glfwSwapBuffers(window.getMainWindow());
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
}

void Application::fluidSimulation() {
    renderFluidSimulation();

    if (EngineEditor::Editor().enableHDR) {
        PostProcessMaster::Framebuffer().postProcessingPass(
            EngineEditor::Editor().enableBloom, EngineEditor::Editor().bloomThreshold, EngineEditor::Editor().bloomKnee,
            EngineEditor::Editor().filterRadius, EngineEditor::Editor().exposure
        );
    }
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    EngineEditor::Editor().DrawEditorForward(window);
}

void Application::rayMarchingLoop() {
    raymarchingShader.useShader();
    glm::vec3 resolution{ 1.f };
    resolution.x = (float)EngineEditor::Editor().getViewportDimensions().z;
    resolution.y = (float)EngineEditor::Editor().getViewportDimensions().w;

    if (window.getKeyPress(GLFW_KEY_R))
        raymarchingShader.reloadShader("src/shaders/raymarching.vert", "src/shaders/raymarching.frag");

    raymarchingShader.setVec3("resolution", resolution);
    raymarchingShader.setVec2("cursor", window.MouseCoordinates());
    raymarchingShader.setFloat("time", currTime);
    raymarchingShader.setFloat("deltaTime", deltaTime);
    raymarchingShader.setUint("isMouseClicked", window.getLMBPressed());

    bindAttriblessVAO();
    drawAttriblessQuad();

    if (EngineEditor::Editor().enableHDR) {
        PostProcessMaster::Framebuffer().postProcessingPass(
            EngineEditor::Editor().enableBloom, EngineEditor::Editor().bloomThreshold, EngineEditor::Editor().bloomKnee,
            EngineEditor::Editor().filterRadius, EngineEditor::Editor().exposure
        );
    }
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    EngineEditor::Editor().DrawEditorForward(window);
}

void Application::mainLoopForward(Psystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition) {
    skylight.updateLightLocation(EngineEditor::Editor().directionalLightPosition);

    if (EngineEditor::Editor().drawSkybox)
        skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

    lightSources.renderLightSources(skylight, pointLights, spotLights, pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().enableShadows) {
        csm.calculateShadows(EngineEditor::Editor().getViewportDimensions(), skylight.getLightDirection(), currentFramebuffer);
    }

    MouseSelector::EntitySelector().pickingPhase(EngineEditor::Editor().getViewportDimensions(), window, currentFramebuffer);

    forwardShader.useShader();

    forwardShader.setMat4("viewportMatrix", EngineEditor::Editor().viewportMatrix);

    setGlobalPBRUniforms(forwardShader);
    setLightingUniforms(forwardShader);

    forwardShader.setUint("calcShadows", EngineEditor::Editor().enableShadows);
    forwardShader.setUint("enableSSAO", EngineEditor::Editor().enableSSAO);
    forwardShader.setUint("drawWireframe", EngineEditor::Editor().drawWireframe);
    forwardShader.setFloat("radius", EngineEditor::Editor().shadowRadius);
    forwardShader.setFloat("emissiveIntensity", EngineEditor::Editor().emissiveMaterialStrength);

    for (size_t i = 0; i < Model::modelList.size(); i++) {
        if ((int)i == EngineEditor::Editor().entityID.x && (bool)EngineEditor::Editor().entityID.y == true)
            continue;

        Model::modelList[i]->renderModel(forwardShader);
    }

    if (EngineEditor::Editor().entityID.x != -1) {
        if ((bool)EngineEditor::Editor().entityID.y == false && EngineEditor::Editor().entityID.x < Mesh::meshList.size())
            Mesh::meshList[EngineEditor::Editor().entityID.x]->renderMeshWithOutline(forwardShader, outlineShader);

        else if ((bool)EngineEditor::Editor().entityID.y == true && EngineEditor::Editor().entityID.x < Model::modelList.size())
            Model::modelList[EngineEditor::Editor().entityID.x]->renderModelWithOutline(forwardShader, outlineShader);
    }

    for (size_t i = 0; i < Mesh::meshList.size(); i++) {
        if ((int)i == EngineEditor::Editor().entityID.x && (bool)EngineEditor::Editor().entityID.y == false)
            continue;
            
        Mesh::meshList[i]->renderMesh(forwardShader);
    }

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().displayGrid)
        Grid::EditorGrid().renderGrid();

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().displayCoordinateSystem)
        CoordinateSystem::Widget().drawCoordinateSystem(EngineEditor::Editor().getViewportDimensions(), EngineEditor::Editor().EditorCamera);

// ----------------------------------------------------------------------------------------------------------------

    /*particlePosition = Overlay::Editor().EditorCamera.getCameraPosition() + 
                       Overlay::Editor().EditorCamera.getFrontVector() * 200.f;*/
    //pSystem.generateParticles(particlePosition, 0.f);

    //pSystem.render(currTime, 1.f);

    /*if (window.getKeyPress(GLFW_KEY_R)) {
        pSystem.generateParticles(particlePosition, 0.f);
    }*/

    //pSystem.renderParticles(&window, Overlay::Editor().EditorCamera, modelMatrix);

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().enableHDR) {
        PostProcessMaster::Framebuffer().postProcessingPass(
            EngineEditor::Editor().enableBloom, EngineEditor::Editor().bloomThreshold, EngineEditor::Editor().bloomKnee,
            EngineEditor::Editor().filterRadius, EngineEditor::Editor().exposure
        );
    }
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    EngineEditor::Editor().DrawEditorForward(window);
}

void Application::mainLoopDeferred(ParticleSystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition) {
    skylight.updateLightLocation(EngineEditor::Editor().directionalLightPosition);

    if (EngineEditor::Editor().enableShadows) {
        csm.calculateShadows(EngineEditor::Editor().getViewportDimensions(), skylight.getLightDirection(), currentFramebuffer);
    }

    MouseSelector::EntitySelector().pickingPhase(EngineEditor::Editor().getViewportDimensions(), window, currentFramebuffer);

    SSAO::Framebuffer().setRadius(EngineEditor::Editor().ssaoRadius);
    SSAO::Framebuffer().setBias(EngineEditor::Editor().ssaoBias);
    SSAO::Framebuffer().setOcclusionPower(EngineEditor::Editor().ssaoOcclusionPower);

    GBuffer::Framebuffer().updateWireframeBool(EngineEditor::Editor().drawWireframe);
    GBuffer::Framebuffer().updateBuffer(outlineShader, EngineEditor::Editor().entityID, currentFramebuffer);

    if(EngineEditor::Editor().enableSSAO)
        SSAO::Framebuffer().calcSSAO(currentFramebuffer);

    deferredShader.useShader();
    
    setGlobalPBRUniforms(deferredShader);
    setLightingUniforms(deferredShader);

    deferredShader.setUint("calcShadows", EngineEditor::Editor().enableShadows);
    deferredShader.setUint("enableSSAO", EngineEditor::Editor().enableSSAO);
    deferredShader.setFloat("radius", EngineEditor::Editor().shadowRadius);

    glBindTextureUnit(DEFERRED_SHADER::POSITION_BUFFER, GBuffer::Framebuffer().positionBuffer());
    glBindTextureUnit(DEFERRED_SHADER::ALBEDO_BUFFER, GBuffer::Framebuffer().albedoBuffer());
    glBindTextureUnit(DEFERRED_SHADER::NORMAL_BUFFER, GBuffer::Framebuffer().normalBuffer());
    glBindTextureUnit(DEFERRED_SHADER::MATERIAL_BUFFER, GBuffer::Framebuffer().metallicBuffer());
    glBindTextureUnit(DEFERRED_SHADER::OCCLUSION_BUFFER, SSAO::Framebuffer().occlusionBuffer());

    bindAttriblessVAO();
    drawAttriblessQuad();

    HDR::Framebuffer().copyDepthInfo(GBuffer::Framebuffer().framebufferID(), EngineEditor::Editor().viewportWidth(), EngineEditor::Editor().viewportHeight());

//----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().drawSkybox)
        skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

    lightSources.renderLightSources(skylight, pointLights, spotLights, pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().displayGrid)
        Grid::EditorGrid().renderGrid();

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().displayCoordinateSystem)
        CoordinateSystem::Widget().drawCoordinateSystem(EngineEditor::Editor().getViewportDimensions(), EngineEditor::Editor().EditorCamera);

//----------------------------------------------------------------------------------------------------------------

    particlePosition = EngineEditor::Editor().EditorCamera.getCameraPosition() + 
                       EngineEditor::Editor().EditorCamera.getFrontVector() * 200.f;

    if (window.getKeyPress(GLFW_KEY_R)) {
        pSystem.generateParticles(particlePosition, 0.f);
    }

    pSystem.renderParticles(&window, EngineEditor::Editor().EditorCamera, modelMatrix);

// ----------------------------------------------------------------------------------------------------------------

    if (EngineEditor::Editor().enableHDR) {
        PostProcessMaster::Framebuffer().postProcessingPass(
            EngineEditor::Editor().enableBloom, EngineEditor::Editor().bloomThreshold, EngineEditor::Editor().bloomKnee,
            EngineEditor::Editor().filterRadius, EngineEditor::Editor().exposure
        );
    }
    else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    EngineEditor::Editor().DrawEditorDeferred(window);
}