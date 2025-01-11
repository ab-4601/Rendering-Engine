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

    ImGuiStyle* style = &ImGui::GetStyle();
    style->WindowBorderSize = 0.f;
    style->WindowRounding = 5.f;

    style->Colors[ImGuiCol_Text] = ImVec4(1.f, 0.9f, 0.9f, 1.f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_HeaderHovered] = style->Colors[ImGuiCol_FrameBgHovered];
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_Tab] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TabActive] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
    style->Colors[ImGuiCol_TabHovered] = ImVec4(0.4f, 0.4f, 0.4f, 1.f);
    style->Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.9f, 0.41f, 0.f, 1.f);
}

void Overlay::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    ImGui::GetMainViewport();

    CreateDockingSpace();
}

void Overlay::CreateDockingSpace() {
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowBgAlpha(0.f);

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

    ImGui::Begin("Dockspace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspaceID = ImGui::GetID("My Dockspace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.f, 0.f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void Overlay::CreateSceneViewport(uint32_t textureID, const Camera& camera, Mesh* mesh) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2.f, 2.f });
    ImGui::Begin("Scene Viewport");

    viewportPos = ImGui::GetWindowPos();
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();

    float viewportAspect = availableSpace.x / availableSpace.y;

    if (viewportAspect > aspectRatio) {
        viewportSize.x = availableSpace.x;
        viewportSize.y = availableSpace.x / aspectRatio;
    }
    else {
        viewportSize.y = availableSpace.y;
        viewportSize.x = availableSpace.y * aspectRatio;
    }

    ImVec2 offset{ 0.f, 0.f };
    offset.x = (availableSpace.x - viewportSize.x) / 2.f;
    offset.y = (availableSpace.y - viewportSize.y) / 2.f;

    ImVec2 cursorPos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(ImVec2(cursorPos.x + offset.x, cursorPos.y + offset.y));

    ImGui::Image((intptr_t)textureID, viewportSize, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

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

void Overlay::DrawVec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label.c_str());

    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 1.f));

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

void Overlay::render(const Window& window, float& exposure, float& shadowRadius, float& filterRadius, float& bloomThreshold, float& ssaoRadius,
    float& ssaoBias, float& ssaoOcclusionPower, bool& drawSkybox, bool& displayGrid, bool& displayCoordinateSystem, bool& enableBloom,
    bool& enableWireframe, bool& enableShadows, bool& enableHDR, bool& enableSSAO, glm::vec3& dirLightLocation, Mesh* currMesh)
{
    ImGui::Begin("Scene Information and modifiers");

    if (ImGui::TreeNode("Scene")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::Checkbox("Render Skybox", &drawSkybox);
        ImGui::Checkbox("Display Coordinate System", &displayCoordinateSystem);
        ImGui::Checkbox("Display Grid", &displayGrid);
        ImGui::Checkbox("Wireframe", &enableWireframe);
        ImGui::Checkbox("Enable HDR", &enableHDR);
        ImGui::Checkbox("Enable bloom", &enableBloom);
        ImGui::Checkbox("Enable shadows", &enableShadows);
        ImGui::Checkbox("SSAO", &enableSSAO);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        ImGui::TreePop();
    }

    ImGui::NewLine();

    if (ImGui::TreeNode("HDR")) {
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
        DrawDragFloat("Exposure", &exposure, 0.01f, 150.f);
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
        DrawDragFloat("PCF filter radius", &shadowRadius, 0.001f, 150.f);
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
    ImGui::NewLine();

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
    ImGui::End();

    if (currMesh != nullptr) {
        glm::mat4 model = glm::mat4(1.f);

        ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, glm::value_ptr(model));
        currMesh->setModelMatrix(model);

        glm::vec3 color{ colorBuffer[0], colorBuffer[1], colorBuffer[2] };

        currMesh->setColor(color);
        currMesh->setMeshMaterial(roughness, metallic, ao);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        glfwMakeContextCurrent(window.getGlfwWindow());
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void Overlay::manipulate(int windowWidth, int windowHeight, const Camera& camera, Mesh* mesh) {
    ImGuizmo::AllowAxisFlip(true);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::Enable(true);
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, (float)windowWidth, (float)windowHeight);

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

Overlay::~Overlay() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &viewportColorBuffer);
}