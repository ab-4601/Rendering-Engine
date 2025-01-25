#include "../../../headers/Engine Core/Editor/Overlay.h"

void Overlay::_init(const Window& window) {
    aspectRatio = viewportSize.x / viewportSize.y;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    //io->Fonts->AddFontDefault();
    mainfont = io->Fonts->AddFontFromFileTTF("src/Content/Fonts/Montserrat-Regular.ttf", 17.5f, NULL);
    IM_ASSERT(mainfont != NULL);

    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_RendererHasViewports;
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    style = &ImGui::GetStyle();

    style->TabBarOverlineSize = 0.f;
    style->WindowBorderSize = 0.f;
    style->WindowRounding = 5.f;
    style->TabRounding = 5.f;
    style->GrabRounding = 1.f;
    style->FrameRounding = 1.f;

    style->Colors[ImGuiCol_Border] = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
    style->Colors[ImGuiCol_Text] = ImVec4(1.f, 0.9f, 0.9f, 1.f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_HeaderHovered] = style->Colors[ImGuiCol_FrameBgHovered];
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Tab] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_TabSelected] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
    style->Colors[ImGuiCol_Separator] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
}

void Overlay::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    CreateDockingSpace();
}

void Overlay::Render(GLFWwindow* window) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(window);
    }
}

void Overlay::CreateDockingSpace() {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    //ImGui::SetNextWindowBgAlpha(0.f);

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

    ImGui::Begin("Dockspace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspaceID = ImGui::GetID("My Dockspace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void Overlay::DrawVec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);

    float dragFloatHeight = ImGui::GetFrameHeight();
    float textHeight = ImGui::CalcTextSize(label.c_str()).y;
    float verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

    ImGui::Text(label.c_str());

    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 2.f));

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
    ImVec2 buttonSize = { lineHeight + 3.f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.15f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.15f, 1.f));
    if (ImGui::Button("X", buttonSize))
        values[0] = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##x", &values[0], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.2f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.7f, 0.2f, 1.f));
    if (ImGui::Button("Y", buttonSize))
        values[1] = resetValue;
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##y", &values[1], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.25f, 0.9f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.35f, 1.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.25f, 0.8f, 1.f));
    if (ImGui::Button("Z", buttonSize))
        values[2] = resetValue;

    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##z", &values[2], 1.f, 0.f, 0.f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::DrawDragFloat(const std::string& label, float* values, float speed, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);

    float dragFloatHeight = ImGui::GetFrameHeight();
    float textHeight = ImGui::CalcTextSize(label.c_str()).y;
    float verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    std::string labelID = "##" + label;
    ImGui::DragFloat(labelID.c_str(), values, speed, 0.f, 0.f, "%.4f");
    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth)
{
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);

    float dragFloatHeight = ImGui::GetFrameHeight();
    float textHeight = ImGui::CalcTextSize(label.c_str()).y;
    float verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    std::string labelID = "##" + label;
    ImGui::SliderFloat(labelID.c_str(), values, min, max, "%.4f");
    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::manipulate(float x, float y, float width, float height, const Camera& camera) {
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::Enable(true);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(x, y, width, height);

    if (entityID.y == false) {
        ImGuizmo::Manipulate(
            glm::value_ptr(camera.generateViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()),
            transformOperation, ImGuizmo::WORLD, glm::value_ptr(Mesh::meshList[entityID.x]->getModelMatrix())
        );

        colorBuffer[0] = Mesh::meshList[entityID.x]->getColor().x;
        colorBuffer[1] = Mesh::meshList[entityID.x]->getColor().y;
        colorBuffer[2] = Mesh::meshList[entityID.x]->getColor().z;

        metallic = Mesh::meshList[entityID.x]->getMetallic();
        roughness = Mesh::meshList[entityID.x]->getRoughness();
        ao = Mesh::meshList[entityID.x]->getAO();

        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(Mesh::meshList[entityID.x]->getModelMatrix()), translation, rotation, scale);
    }
    else {
        ImGuizmo::Manipulate(
            glm::value_ptr(camera.generateViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()),
            transformOperation, ImGuizmo::WORLD, glm::value_ptr(Model::modelList[entityID.x]->getModelMatrix())
        );

        colorBuffer[0] = Model::modelList[entityID.x]->getColor().x;
        colorBuffer[1] = Model::modelList[entityID.x]->getColor().y;
        colorBuffer[2] = Model::modelList[entityID.x]->getColor().z;

        metallic = Model::modelList[entityID.x]->getMetallic();
        roughness = Model::modelList[entityID.x]->getRoughness();
        ao = Model::modelList[entityID.x]->getAO();

        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(Model::modelList[entityID.x]->getModelMatrix()), translation, rotation, scale);
    }
}

void Overlay::DrawRenderingModifiers(const Window& window) {
    ImGui::Begin("Rendering");

    if (ImGui::CollapsingHeader("HDR")) {
        static int msaaSelection = 0;
        static int frameRateSelection = 0;
        exposure = (exposure < 0.f) ? 0.f : exposure;
        DrawDragFloat("Exposure", &exposure, 0.01f, 150.f);
        DrawDragFloat("Emissive Strength", &emissiveMaterialStrength, 0.01f, 150.f);

        ImGui::PushID("MSAA Samples");
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 150.f);
        float dragFloatHeight = ImGui::GetFrameHeight();
        float textHeight = ImGui::CalcTextSize("MSAA Samples").y;
        float verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
        ImGui::Text("MSAA Samples");
        ImGui::NextColumn();

        if (ImGui::BeginCombo("##MSAA Samples", msaaSampleDisplayOptions[msaaSelection].c_str())) {
            int previousIndex = msaaSelection;

            for (size_t i = 0; i < msaaSampleDisplayOptions.size(); i++) {
                if (ImGui::Selectable(msaaSampleDisplayOptions[i].c_str(), &msaaSelected)) {
                    msaaSelection = (int)i;
                    msaaSelected[i] = !msaaSelected[i];
                }
            }

            if (previousIndex != msaaSelection) {
                HDR::Framebuffer().updateSamplesMSAA(viewportDimensions.z, viewportDimensions.w, numSamples[msaaSelection]);
            }

            msaaSelected[previousIndex] = false;

            ImGui::EndCombo();
        }

        ImGui::Columns(1);
        ImGui::PopID();

        ImGui::PushID("Frame Rate Cap");
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 150.f);
        dragFloatHeight = ImGui::GetFrameHeight();
        textHeight = ImGui::CalcTextSize("Frame Rate Cap").y;
        verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
        ImGui::Text("Frame Rate Cap");
        ImGui::NextColumn();

        if (ImGui::BeginCombo("##Frame Rate Cap", frameTimeDisplayOptions[frameRateSelection].c_str())) {
            int previousIndex = frameRateSelection;

            for (size_t i = 0; i < frameTimeDisplayOptions.size(); i++) {
                if (ImGui::Selectable(frameTimeDisplayOptions[i].c_str(), &selectedFramerate)) {
                    frameRateSelection = (int)i;
                    selectedFramerate[i] = !selectedFramerate[i];
                }
            }

            if (previousIndex != msaaSelection) {
                frameRateCap = frameTimes[frameRateSelection];
            }

            selectedFramerate[previousIndex] = false;

            ImGui::EndCombo();
        }

        ImGui::Columns(1);
        ImGui::PopID();

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Bloom")) {
        filterRadius = filterRadius < 0.f ? 0.f : filterRadius;
        DrawDragFloat("Filter Radius", &filterRadius, 0.0001f, 150.f);

        bloomThreshold = bloomThreshold < 0.f ? 0.f : bloomThreshold;
        DrawDragFloat("Threshold", &bloomThreshold, 0.01f, 150.f);

        bloomKnee = bloomKnee < 0.f ? 0.f : bloomKnee;
        DrawDragFloat("Knee", &bloomKnee, 0.0001f, 150.f);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Shadows")) {
        shadowRadius = (shadowRadius < 0.f) ? 0.f : shadowRadius;
        DrawDragFloat("PCF Filter Radius", &shadowRadius, 0.001f, 150.f);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("SSAO")) {
        DrawDragFloat("Radius", &ssaoRadius, 0.1f, 150.f);
        DrawDragFloat("Bias", &ssaoBias, 0.001f, 150.f);
        DrawDragFloat("Occlusion Power", &ssaoOcclusionPower, 1.f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    ImGui::Text("Vendor : %s", window.vendor);
    ImGui::Text("Renderer : %s", window.renderer);
    ImGui::Text("OpenGL version : %s", window.version);
    ImGui::Text("GLSL version : %s", window.glslVersion);

    ImGui::End();
}

void Overlay::DrawSceneViewportForward(glm::mat4& viewportMatrix, const Window& window, Camera& camera) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
    ImGui::Begin("Scene Viewport");

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    float framePadding = ImGui::GetFontSize() + style->FramePadding.y * 2.f;

    viewportPos = ImGui::GetWindowPos();
    viewportSize = ImGui::GetContentRegionAvail();

    viewportPos.y += ImGui::GetWindowSize().y - viewportSize.y;

    if (viewportDimensions.z != (int)viewportSize.x || viewportDimensions.w != (int)viewportSize.y) {
        viewportDimensions.z = (int)viewportSize.x;
        viewportDimensions.w = (int)viewportSize.y;

        glViewport(viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w);

        HDR::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w, true);
        PostProcessMaster::Framebuffer().resize(viewportDimensions, true);

        camera.setProjectionAspect(viewportSize.x / viewportSize.y);

        viewportMatrix = glm::mat4{
            glm::vec4(viewportDimensions.z / 2.f, 0.f, 0.f, viewportDimensions.z / 2.f),
            glm::vec4(0.f, viewportDimensions.w / 2.f, 0.f, viewportDimensions.w / 2.f),
            glm::vec4(0.f, 0.f, 1.f, 0.f),
            glm::vec4(0.f, 0.f, 0.f, 1.f)
        };
    }

    ImGui::Image((intptr_t)PostProcessMaster::Framebuffer().ColorbufferID(), viewportSize, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowPos(ImVec2(viewportPos.x + 5.f, viewportPos.y + 5.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(viewportSize.x, 20));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;
    flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    if (ImGui::Begin("Toolbar", nullptr, flags)) {
        if (ImGui::Button("Wireframe"))
            drawWireframe = !drawWireframe;

        ImGui::SameLine();
        if (ImGui::Button("Coordinate System"))
            displayCoordinateSystem = !displayCoordinateSystem;

        ImGui::SameLine();
        if (ImGui::Button("Grid"))
            displayGrid = !displayGrid;
    }
    ImGui::End();

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(2);

    snprintf(FPS, sizeof(FPS), "%.2f FPS", io->Framerate);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 textPosition = ImVec2(viewportPos.x + 10.f, viewportPos.y + viewportSize.y - 20.f);
    drawList->AddText(textPosition, IM_COL32(10, 255, 50, 255), FPS);   

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && isMouseOverWindow(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {
        mousePos = ImGui::GetMousePos();
        mousePos.x -= viewportPos.x;
        mousePos.y -= viewportPos.y;

        if (mousePos.x >= 0 || mousePos.x <= viewportSize.x || mousePos.y >= 0 || mousePos.y <= viewportSize.y) {
            float scaleX = (float)window.getBufferWidth() / viewportSize.x;
            float scaleY = (float)window.getBufferHeight() / viewportSize.y;

            float mouseX = mousePos.x * scaleX;
            float mouseY = mousePos.y * scaleY;

            entityID = MouseSelector::EntitySelector().mouseSelectionResult(
                window.getBufferHeight(), (int)mouseX, (int)mouseY
            );
        }
    }

    if (entityID.x != -1) {
        _updateTransformOperation();
        manipulate(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y, camera);
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void Overlay::DrawSceneViewportDeferred(glm::mat4& viewportMatrix, const Window& window, Camera& camera) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
    ImGui::Begin("Scene Viewport");

    float framePadding = ImGui::GetFontSize() + style->FramePadding.y * 2.f;

    viewportPos = ImGui::GetWindowPos();
    viewportSize = ImGui::GetContentRegionAvail();

    viewportPos.y += ImGui::GetWindowSize().y - viewportSize.y;

    if (viewportDimensions.z != (int)viewportSize.x || viewportDimensions.w != (int)viewportSize.y) {
        viewportDimensions.z = (int)viewportSize.x;
        viewportDimensions.w = (int)viewportSize.y;

        glViewport(viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w);

        HDR::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w, false);
        PostProcessMaster::Framebuffer().resize(viewportDimensions, false);
        GBuffer::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w);
        SSAO::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w);

        camera.setProjectionAspect(viewportSize.x / viewportSize.y);

        viewportMatrix = glm::mat4{
            glm::vec4(viewportDimensions.z / 2.f, 0.f, 0.f, viewportDimensions.z / 2.f),
            glm::vec4(0.f, viewportDimensions.w / 2.f, 0.f, viewportDimensions.w / 2.f),
            glm::vec4(0.f, 0.f, 1.f, 0.f),
            glm::vec4(0.f, 0.f, 0.f, 1.f)
        };
    }

    intptr_t displayBuffer = PostProcessMaster::Framebuffer().ColorbufferID();

    if (window.getKeyPress(GLFW_KEY_N))
        displayBuffer = GBuffer::Framebuffer().normalBuffer();
    else if (window.getKeyPress(GLFW_KEY_P))
        displayBuffer = GBuffer::Framebuffer().positionBuffer();
    else if (window.getKeyPress(GLFW_KEY_M))
        displayBuffer = GBuffer::Framebuffer().metallicBuffer();
    else if (window.getKeyPress(GLFW_KEY_C))
        displayBuffer = GBuffer::Framebuffer().albedoBuffer();
    else if (window.getKeyPress(GLFW_KEY_O))
        displayBuffer = SSAO::Framebuffer().occlusionBuffer();

    ImGui::Image(displayBuffer, viewportSize, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

    snprintf(FPS, sizeof(FPS), "%.2f FPS", io->Framerate);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddText(ImVec2(viewportPos.x + 4.f, viewportPos.y + framePadding), IM_COL32(10, 255, 50, 255), FPS);

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && isMouseOverWindow(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {
        mousePos = ImGui::GetMousePos();
        mousePos.x -= viewportPos.x;
        mousePos.y -= viewportPos.y;

        if (mousePos.x >= 0 || mousePos.x <= viewportSize.x || mousePos.y >= 0 || mousePos.y <= viewportSize.y) {
            float scaleX = (float)window.getBufferWidth() / viewportSize.x;
            float scaleY = (float)window.getBufferHeight() / viewportSize.y;

            mousePos.x = mousePos.x * scaleX;
            mousePos.y = mousePos.y * scaleY;

            entityID = MouseSelector::EntitySelector().mouseSelectionResult(
                window.getBufferHeight(), (int)mousePos.x, (int)mousePos.y
            );
        }
    }

    if (entityID.x != -1) {
        _updateTransformOperation();
        manipulate(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y, camera);
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void Overlay::_updateTransformOperation() {
    if (ImGui::IsKeyPressed(ImGuiKey_1))
        transformOperation = ImGuizmo::TRANSLATE;

    else if (ImGui::IsKeyPressed(ImGuiKey_2))
        transformOperation = ImGuizmo::ROTATE;

    else if (ImGui::IsKeyPressed(ImGuiKey_3))
        transformOperation = ImGuizmo::SCALE;
}

void Overlay::DrawDetailsPanel(const Window& window) {
    if (runtimeInformation) DrawRuntimeInformation();

    static bool swapInterval = enableVsync;

    ImGui::Begin("Details Panel");

    if (ImGui::CollapsingHeader("Scene")) {
        ImGui::Checkbox("Render Skybox", &drawSkybox);
        ImGui::Checkbox("Enable HDR", &enableHDR);
        ImGui::Checkbox("Enable Bloom", &enableBloom);
        ImGui::Checkbox("Enable Shadows", &enableShadows);
        ImGui::Checkbox("Enable SSAO", &enableSSAO);
        ImGui::Checkbox("Enable V-Sync", &enableVsync);
        ImGui::Checkbox("Runtime Information", &runtimeInformation);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    if (swapInterval != enableVsync) {
        swapInterval = enableVsync;
        glfwSwapInterval(enableVsync);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Properties")) {
        if (ImGui::TreeNode("Material Properties")) {
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            ImGui::Columns(2);
            ImGui::SetColumnWidth(0, 120.f);

            float dragFloatHeight = ImGui::GetFrameHeight();
            float textHeight = ImGui::CalcTextSize("Albedo").y;
            float verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);

            ImGui::Text("Albedo");
            ImGui::NextColumn();
            ImGui::ColorEdit4("##color", colorBuffer);
            ImGui::Columns(1);

            DrawSliderFloat("Metallic", &metallic, 0.f, 1.f);
            DrawSliderFloat("Roughness", &roughness, 0.f, 1.f);
            DrawSliderFloat("AO", &ao, 0.f, 1.f);

            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            ImGui::TreePop();
        }

        ImGui::NewLine();

        if (ImGui::TreeNode("Transforms")) {
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            DrawVec3Control("Translation", translation);
            DrawVec3Control("Rotation", rotation);
            DrawVec3Control("Scale", scale, 1.f);

            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
            ImGui::TreePop();
        }
    }

    ImGui::NewLine();
    DrawVec3Control("Directional Light", glm::value_ptr(directionalLightPosition), 0.f, 150.f);

    ImGui::End();

    if (entityID.x != -1) {
        glm::mat4 model = glm::mat4(1.f);

        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(model));

        if (entityID.y == false) {
            Mesh::meshList[entityID.x]->setModelMatrix(model);

            glm::vec3 color{ colorBuffer[0], colorBuffer[1], colorBuffer[2] };

            Mesh::meshList[entityID.x]->setColor(color);
            Mesh::meshList[entityID.x]->setMeshMaterial(roughness, metallic, ao);
        }
        else {
            Model::modelList[entityID.x]->setModelMatrix(model);

            glm::vec3 color{ colorBuffer[0], colorBuffer[1], colorBuffer[2] };

            Model::modelList[entityID.x]->setColor(color);
            Model::modelList[entityID.x]->setMeshMaterial(roughness, metallic, ao);
        }
    }
}

void Overlay::DrawRuntimeInformation() const {
    ImGui::Begin("Runtime");

    ImGui::Text(
        "Viewport Dimensions : (%i, %i, %i, %i)",
        viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w
    );
    ImGui::Text("Mouse Cursor Position : (%.2f, %.2f)", mousePos.x, mousePos.y);
    ImGui::Text("Entity Index : (%i, %i)", entityID.x, entityID.y);
    ImGui::Text("Viewport Size : (%.1f, %.1f)", viewportSize.x, viewportSize.y);

    ImGui::NewLine();
    ImGui::Text("Application average: %.4f ms/frame", 1000.f / io->Framerate);

    ImGui::End();
}

Overlay::~Overlay() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &viewportColorBuffer);
}