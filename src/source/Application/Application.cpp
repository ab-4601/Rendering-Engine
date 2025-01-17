#include "../../headers/Application/Application.h"

Application::Application() {
    Mesh::meshList.clear();
    Model::modelList.clear();

    bufferWidth = window.getBufferWidth();
    bufferHeight = window.getBufferHeight();

    MouseSelector::EntitySelection().resize(bufferWidth, bufferHeight);

    viewportMatrix = glm::mat4{
        glm::vec4((float)window.getBufferWidth() / 2.f, 0.f, 0.f, (float)window.getBufferWidth() / 2.f),
        glm::vec4(0.f, (float)window.getBufferHeight() / 2.f, 0.f, (float)window.getBufferHeight() / 2.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };

    quad.createQuad();
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
    shader.useShader();

    shader.setFloat("nearPlane", ::near_plane);
    shader.setFloat("farPlane", ::far_plane);
    shader.setInt("cascadeCount", csm.getNumCascades());
    shader.setVec3("offsetTexSize", csm.getNoiseTextureSize());
    shader.setMat4("viewportMatrix", viewportMatrix);

    const float* cascadePlanes = csm.cascadePlanes();

    char buffer[64]{ '\0' };

    for (int i = 0; i < ::MAX_CASCADES; i++) {
        snprintf(buffer, sizeof(buffer), "cascadePlanes[%i]", i);
        shader.setFloat(buffer, cascadePlanes[i]);
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getIrradianceMap());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getPrefilterTexture());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, skybox.getBRDFTexture());

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D_ARRAY, csm.getShadowMaps());

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_3D, csm.noiseBuffer());

    shader.endShader();
}

void Application::start() {
    glm::vec3 lightDirection(3000.f, 3000.f, 0.f);
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

    Overlay::Editor()._init(window);

    glm::mat4 modelMatrix{ 1.f };

    Icosphere sphere{ GL_CW, 5 };
    sphere.setColor({ 0.07f, 1.f, 1.f });
    sphere.setMeshMaterial(0.f, 0.f, 1.f);
    sphere.loadMesh();

    /*UVSphere sphere{ 100, 100, 1.f };
    sphere.setColor({ 0.07f, 1.f, 1.f });
    sphere.setMeshMaterial(0.f, 0.f, 1.f);
    sphere.loadMesh();*/

    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(400.f, 100.f, 0.f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.f));

    sphere.setModelMatrix(modelMatrix);

    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, 100.f, 0.f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.f));

    Cube cube{};
    cube.setColor({ 1.f, 0.07f, 0.07f });
    cube.setModelMatrix(modelMatrix);
    cube.setMeshMaterial(0.f, 0.f, 1.f);;
    cube.loadMesh(false, false);

    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(5.f, 5.f, 5.f));

    /*Terrain terrain(500, 500);
    terrain.generateHeightMaps(3);
    terrain.setMeshMaterial(0.f, 1.f, 1.f);
    terrain.generateTerrain();
    terrain.loadMesh();
    terrain.setModelMatrix(model);
    terrain.setColor(glm::vec3(0.2f, 0.2f, 0.2f));*/

    /*Model suntemple(
        "src/Content/Models/SunTemple/SunTemple.fbx",
        "src/Content/Models/SunTemple/Textures/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_SPECULAR,
        aiTextureType_EMISSIVE,
        true, true
    );*/

    modelMatrix = glm::mat4(1.f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(100.f));

    /*Model isometricRoom(
        "src/Content/Models/IsometricRoom/Isometric Room v5058 p4866.fbx",
        "src/Content/Models/IsometricRoom/",
        aiTextureType_DIFFUSE,
        aiTextureType_NORMALS,
        aiTextureType_METALNESS,
        aiTextureType_EMISSIVE,
        false, true
    );*/

    //isometricRoom->setModelMatrix(model);

    /*Model sponza(
        "src/Content/Models/Sponza/Sponza.gltf",
        "src/Content/Models/Sponza/"
    );*/

    CoordinateSystem::Widget().createCoordinateSystem();

    ParticleTexture partTex("src/Content/Textures/Particle System/particleAtlas.png", 4.f);
    glm::vec3 particlePosition{ 20.f, 20.f, 20.f }, velocity{ 10.f, 50.f, 10.f }, particleColor{ 1.f, 0.5f, 0.05f };
    ParticleSystem pSystem(particleColor, 10, 15.f, 1.f, 6.f, partTex);

    /*ParticleTexture fire("src/Content/Textures/fire.png", 8.f);
    glm::vec3 fireParticlePosition{ 1125.f, 120.f, 400.f };
    ParticleSystem fireSystem(particleColor, 30.f, -30.f, 1.f, 30.f, fire);*/

    HDR::Framebuffer()._initMSAA(window.getBufferWidth(), window.getBufferHeight());
    //hdrBuffer._init(bufferWidth, bufferHeight);

    setGlobalPBRUniforms(forwardShader);
    //setGlobalPBRUniforms(deferredShader);

    glClearColor(0.f, 0.01f, 0.01f, 1.f);

    while (!glfwWindowShouldClose(window.getGlfwWindow())) {
        mainLoopForward(pSystem, modelMatrix, particlePosition, lightDirection);
    }
}

void Application::mainLoopForward(ParticleSystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition, glm::vec3& lightDirection)
{
    if (window.getBufferWidth() != bufferWidth || window.getBufferHeight() != bufferHeight) {
        bufferWidth = window.getBufferWidth();
        bufferHeight = window.getBufferHeight();

        MouseSelector::EntitySelection().resize(bufferWidth, bufferHeight);
    }

    currTime = (float)glfwGetTime();
    deltaTime = currTime - lastTime;
    elapsedTime += deltaTime;
    lastTime = currTime;

    camera.keyFunctionality(window.getCurrWindow(), deltaTime);
    camera.mouseFunctionality(window.getXChange(), window.getYChange(), window.getScrollChange());

    //fireSystem.updateParticles(deltaTime, this->camera.getCameraPosition());
    pSystem.updateParticles(deltaTime, camera.getCameraPosition());

    if (elapsedTime >= 0.016f)
    {
        Overlay::Editor().newFrame();

        currentFramebuffer = 0;
        elapsedTime = 0.f;

        skylight.updateLightLocation(lightDirection);

        if (Overlay::Editor().enableHDR) {
            HDR::Framebuffer().enableHDRWriting();
            currentFramebuffer = HDR::Framebuffer().FramebufferID();
        }
        else {
            PostProcessMaster::Framebuffer().enableWriting();
            currentFramebuffer = PostProcessMaster::Framebuffer().FramebufferID();
        }

        if (Overlay::Editor().drawSkybox)
            skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

        lightSources.renderLightSources(skylight, pointLights, spotLights, pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().enableShadows) {
            csm.calculateShadows(Overlay::Editor().getViewportDimensions(), lightDirection, currentFramebuffer);
        }

        MouseSelector::EntitySelection().pickingPhase(Overlay::Editor().getViewportDimensions(), window, currentFramebuffer);

        mouseCoords = window.MouseCoordinates();

        forwardShader.useShader();

        forwardShader.setMat4("viewportMatrix", viewportMatrix);

        setLightingUniforms(forwardShader);

        forwardShader.setUint("calcShadows", Overlay::Editor().enableShadows);
        forwardShader.setUint("enableSSAO", Overlay::Editor().enableSSAO);
        forwardShader.setUint("drawWireframe", Overlay::Editor().drawWireframe);
        forwardShader.setFloat("radius", Overlay::Editor().shadowRadius);

        for (size_t i = 0; i < Model::modelList.size(); i++) {
            Model::modelList[i]->renderModel(forwardShader);
        }

        if (Overlay::Editor().entityID < (int)Mesh::meshList.size() && Overlay::Editor().entityID != -1) {
            Overlay::Editor()._updateTransformOperation(window);
            Mesh::meshList[Overlay::Editor().entityID]->renderMeshWithOutline(forwardShader, outlineShader);
        }

        for (size_t i = 0; i < Mesh::meshList.size(); i++) {
            if ((int)i != Overlay::Editor().entityID)
                Mesh::meshList[i]->renderMesh(forwardShader);
        }

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().displayGrid)
            Grid::EditorGrid().renderGrid();

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().displayCoordinateSystem)
            CoordinateSystem::Widget().drawCoordinateSystem(Overlay::Editor().getViewportDimensions(), camera);

// ----------------------------------------------------------------------------------------------------------------

        particlePosition = camera.getCameraPosition() + camera.getFrontVector() * 200.f;
        //fireSystem.generateParticles(fireParticlePosition, 0.f);

        if (window.getKeyPress(GLFW_KEY_R)) {
            pSystem.generateParticles(particlePosition, 0.f);
        }

        pSystem.renderParticles(&window, camera, modelMatrix);
        //fireSystem.renderParticles(&this->window, this->camera, modelMatrix);

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().enableHDR) {
            PostProcessMaster::Framebuffer().postProcessingPassMSAA(
                Overlay::Editor().enableBloom, Overlay::Editor().bloomThreshold,
                Overlay::Editor().filterRadius, Overlay::Editor().exposure
            );
        }
        else
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

        Overlay::Editor().CreateSceneViewport(viewportMatrix, window, camera);
        Overlay::Editor().render(window, lightDirection);

        glfwSwapBuffers(window.getMainWindow());
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glfwPollEvents();
}

void Application::mainLoopDeferred(ParticleSystem& pSystem, glm::mat4& modelMatrix, glm::vec3& particlePosition, glm::vec3& lightDirection)
{
    currTime = (float)glfwGetTime();
    deltaTime = currTime - lastTime;
    elapsedTime += deltaTime;
    lastTime = currTime;

    camera.keyFunctionality(window.getCurrWindow(), deltaTime);
    camera.mouseFunctionality(window.getXChange(), window.getYChange(), window.getScrollChange());

    pSystem.updateParticles(deltaTime, camera.getCameraPosition());
    //fireSystem.updateParticles(deltaTime, this->camera.getCameraPosition());

    if (elapsedTime >= 0.016f)
    {
        currentFramebuffer = 0;
        elapsedTime = 0.f;

        Overlay::Editor().newFrame();

        skylight.updateLightLocation(lightDirection);

        if (Overlay::Editor().enableHDR) {
            HDR::Framebuffer().enableHDRWriting();
            currentFramebuffer = HDR::Framebuffer().FramebufferID();
        }
        else {
            PostProcessMaster::Framebuffer().enableWriting();
            currentFramebuffer = PostProcessMaster::Framebuffer().FramebufferID();
        }

        
        if (Overlay::Editor().enableShadows) {
            csm.calculateShadows(Overlay::Editor().getViewportDimensions(), lightDirection, currentFramebuffer);
        }

        MouseSelector::EntitySelection().pickingPhase(Overlay::Editor().getViewportDimensions(), window, currentFramebuffer);

        mouseCoords = window.MouseCoordinates();

        SSAO::Framebuffer().setRadius(Overlay::Editor().ssaoRadius);
        SSAO::Framebuffer().setBias(Overlay::Editor().ssaoBias);
        SSAO::Framebuffer().setOcclusionPower(Overlay::Editor().ssaoOcclusionPower);

        GBuffer::Framebuffer().updateWireframeBool(Overlay::Editor().drawWireframe);
        GBuffer::Framebuffer().updateBuffer(outlineShader, Overlay::Editor().entityID, currentFramebuffer);
        SSAO::Framebuffer().calcSSAO(currentFramebuffer);

        deferredShader.useShader();

        setLightingUniforms(deferredShader);

        deferredShader.setUint("calcShadows", Overlay::Editor().enableShadows);
        deferredShader.setUint("enableSSAO", Overlay::Editor().enableSSAO);
        deferredShader.setFloat("radius", Overlay::Editor().shadowRadius);

        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, GBuffer::Framebuffer().positionBuffer());

        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, GBuffer::Framebuffer().albedoBuffer());

        glActiveTexture(GL_TEXTURE9);
        glBindTexture(GL_TEXTURE_2D, GBuffer::Framebuffer().normalBuffer());

        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, GBuffer::Framebuffer().metallicBuffer());

        glActiveTexture(GL_TEXTURE11);
        glBindTexture(GL_TEXTURE_2D, SSAO::Framebuffer().occlusionBuffer());

        if (Overlay::Editor().entityID < (int)Mesh::meshList.size() && Overlay::Editor().entityID != -1) {
            Overlay::Editor()._updateTransformOperation(window);
        }

        quad.renderQuad();

//----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().drawSkybox)
            skybox.renderSkybox();

// ----------------------------------------------------------------------------------------------------------------

        lightSources.renderLightSources(skylight, pointLights, spotLights, pointLightCount, spotLightCount);

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().displayGrid)
            Grid::EditorGrid().renderGrid();

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().displayCoordinateSystem)
            CoordinateSystem::Widget().drawCoordinateSystem(Overlay::Editor().getViewportDimensions(), camera);

//----------------------------------------------------------------------------------------------------------------

        particlePosition = camera.getCameraPosition() + camera.getFrontVector() * 200.f;
        //fireSystem.generateParticles(fireParticlePosition, 0.f);

        if (window.getKeyPress(GLFW_KEY_R)) {
            pSystem.generateParticles(particlePosition, 0.f);
        }

        pSystem.renderParticles(&window, camera, modelMatrix);
        //fireSystem.renderParticles(&window, camera, modelMatrix);

// ----------------------------------------------------------------------------------------------------------------

        if (Overlay::Editor().enableHDR) {
            PostProcessMaster::Framebuffer().postProcessingPass(
                Overlay::Editor().enableBloom, Overlay::Editor().bloomThreshold,
                Overlay::Editor().filterRadius, Overlay::Editor().exposure
            );
        }

        Overlay::Editor().CreateSceneViewport(viewportMatrix, window, camera);
        Overlay::Editor().render(window, lightDirection);

        glfwSwapBuffers(window.getMainWindow());
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glfwPollEvents();
}