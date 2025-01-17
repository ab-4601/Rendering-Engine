#include "../../../headers/Engine Core/Editor/Overlay.h"

void Overlay::_init(const Window& window) {
    aspectRatio = (float)window.getBufferWidth() / window.getBufferHeight();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    //io->Fonts->AddFontDefault();
    mainfont = io->Fonts->AddFontFromFileTTF("src/Content/Fonts/JetBrainsMono-SemiBold.ttf", 18.5f, NULL);
    IM_ASSERT(mainfont != NULL);

    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    style = &ImGui::GetStyle();
    style->WindowBorderSize = 0.f;
    style->WindowRounding = 5.f;

    style->Colors[ImGuiCol_Text] = ImVec4(1.f, 0.9f, 0.9f, 1.f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_HeaderHovered] = style->Colors[ImGuiCol_FrameBgHovered];
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Tab] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_TabSelected] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
}

void Overlay::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    CreateDockingSpace();
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

void Overlay::CreateSceneViewport(glm::mat4& viewportMatrix, const Window& window, Camera& camera) {
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

    std::string text1 = "Application average: " + std::to_string(1000.f / io->Framerate) + " ms/frame";
    std::string text2 = std::to_string(io->Framerate) + "FPS";
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddText(viewportPos, IM_COL32(10, 255, 255, 255), text1.c_str());
    drawList->AddText(ImVec2(viewportPos.x, viewportPos.y + framePadding), IM_COL32(10, 255, 255, 255), text2.c_str());

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None) && isMouseOverWindow(ImGuiMouseButton_Left) && !ImGuizmo::IsOver()) {
        mousePos = ImGui::GetMousePos();
        mousePos.x -= viewportPos.x;
        mousePos.y -= viewportPos.y;

        if (mousePos.x > 0 || mousePos.x < viewportSize.x || mousePos.y > 0 || mousePos.y < viewportSize.y)
        {
            float scaleX = (float)window.getBufferWidth() / viewportSize.x;
            float scaleY = (float)window.getBufferHeight() / viewportSize.y;

            mousePos.x = mousePos.x * scaleX;
            mousePos.y = mousePos.y * scaleY;

            entityID = MouseSelector::EntitySelection().mouseSelectionResult(
                window.getBufferHeight(), (int)mousePos.x, (int)mousePos.y
            );
        }
    }

    if (entityID != -1) {
        Mesh* mesh = Mesh::meshList[entityID];
        manipulate(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y, camera, mesh);
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void Overlay::_updateTransformOperation(const Window& window) {
    if (window.getKeyPress(GLFW_KEY_1))
        transformOperation = ImGuizmo::TRANSLATE;

    else if (window.getKeyPress(GLFW_KEY_2))
        transformOperation = ImGuizmo::ROTATE;

    else if (window.getKeyPress(GLFW_KEY_3))
        transformOperation = ImGuizmo::SCALE;
}

void Overlay::manipulate(float x, float y, float width, float height, const Camera& camera, Mesh* mesh) {
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::Enable(true);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(x, y, width, height);

    ImGuizmo::Manipulate(
        glm::value_ptr(camera.generateViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()),
        transformOperation, ImGuizmo::WORLD, glm::value_ptr(mesh->getModelMatrix())
    );

    colorBuffer[0] = mesh->getColor().x;
    colorBuffer[1] = mesh->getColor().y;
    colorBuffer[2] = mesh->getColor().z;

    metallic = mesh->getMetallic();
    roughness = mesh->getRoughness();
    ao = mesh->getAO();

    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(mesh->getModelMatrix()), translation, rotation, scale);
}

void Overlay::DrawVec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());

    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.5f, 0.5f));

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
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    std::string labelID = "##" + label;
    ImGui::SliderFloat(labelID.c_str(), values, min, max, "%.4f");
    ImGui::Columns(1);
    ImGui::PopID();
}

void Overlay::DrawCombo(const std::string& label, std::vector<std::string>& options, std::vector<bool>& values, int& index) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 150.f);
    ImGui::Text(label.c_str());
    ImGui::NextColumn();

    if (ImGui::BeginCombo(("##" + label).c_str(), options[index].c_str())) {
        int previousIndex = index;

        for (size_t i = 0; i < options.size(); i++) {
            if (ImGui::Selectable(options[i].c_str(), &values)) {
                index = (int)i;
                values[i] = !values[i];
            }
        }

        if (previousIndex != index) {
            HDR::Framebuffer().updateSamplesMSAA(viewportDimensions.z, viewportDimensions.w, numSamples[index]);
        }

        values[previousIndex] = false;

        ImGui::EndCombo();
    }

    ImGui::Columns(1);
}

void Overlay::DrawDriverAndVendorInformation(const Window& window) const {
    ImGui::Begin("Driver and Vendor Information");

    ImGui::Text("Vendor         : %s", window.vendor);
    ImGui::Text("Renderer       : %s", window.renderer);
    ImGui::Text("OpenGL version : %s", window.version);
    ImGui::Text("GLSL version   : %s", window.glslVersion);

    ImGui::End();
}

void Overlay::DrawRuntimeInformation() const {
    ImGui::Begin("Runtime");

    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    ImGui::Text("Viewport dimensions   : (%i, %i, %i, %i)", viewport[0], viewport[1], viewport[2], viewport[3]);
    ImGui::Text("Mouse cursor position : (%.2f, %.2f)", mousePos.x, mousePos.y);
    ImGui::Text("Entity Index          : %i", entityID);
    ImGui::Text("Viewport Avail Size   : (%.1f, %.1f)", viewportSize.x, viewportSize.y);
    //ImGui::NewLine();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);

    ImGui::End();
}

void Overlay::render(const Window& window, glm::vec3& dirLightLocation) {
    DrawDriverAndVendorInformation(window);

    if (runtimeInformation) DrawRuntimeInformation();

    ImGui::Begin("Details Panel");

    if (ImGui::TreeNode("Scene")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::Checkbox("Render Skybox", &drawSkybox);
        ImGui::Checkbox("Display Coordinate System", &displayCoordinateSystem);
        ImGui::Checkbox("Display Grid", &displayGrid);
        ImGui::Checkbox("Wireframe", &drawWireframe);
        ImGui::Checkbox("Enable HDR", &enableHDR);
        ImGui::Checkbox("Enable Bloom", &enableBloom);
        ImGui::Checkbox("Enable Shadows", &enableShadows);
        ImGui::Checkbox("SSAO", &enableSSAO);
        ImGui::Checkbox("Runtime Information", &runtimeInformation);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("HDR")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        static int msaaSelection = 0;
        DrawDragFloat("Exposure", &exposure, 0.01f, 150.f);
        DrawCombo("MSAA Samples", samples, selected, msaaSelection);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Bloom")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        DrawDragFloat("Filter Radius", &filterRadius, 0.0001f, 150.f);
        bloomThreshold = bloomThreshold < 0.f ? 0.f : bloomThreshold;
        DrawDragFloat("Knee", &bloomThreshold, 0.01f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Shadows")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        DrawDragFloat("PCF Filter Radius", &shadowRadius, 0.001f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("SSAO")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        DrawDragFloat("Radius", &ssaoRadius, 0.1f, 150.f);
        DrawDragFloat("Bias", &ssaoBias, 0.001f, 150.f);
        DrawDragFloat("Occlusion Power", &ssaoOcclusionPower, 1.f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("Material Properties")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 120.f);
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

    if (ImGui::TreeNode("Transforms"))
    {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        DrawVec3Control("Translation", translation);
        DrawVec3Control("Rotation", rotation);
        DrawVec3Control("Scale", scale, 1.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);

        ImGui::TreePop();
    }

    ImGui::NewLine();
    DrawVec3Control("Directional Light", glm::value_ptr(dirLightLocation), 0.f, 150.f);

    ImGui::End();

    if (entityID != -1) {
        glm::mat4 model = glm::mat4(1.f);

        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(model));
        Mesh::meshList[entityID]->setModelMatrix(model);

        glm::vec3 color{ colorBuffer[0], colorBuffer[1], colorBuffer[2] };

        Mesh::meshList[entityID]->setColor(color);
        Mesh::meshList[entityID]->setMeshMaterial(roughness, metallic, ao);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(window.getGlfwWindow());
    }
}

Overlay::~Overlay() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &viewportColorBuffer);
}