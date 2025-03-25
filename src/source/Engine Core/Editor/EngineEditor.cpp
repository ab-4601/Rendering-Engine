#include "../../../headers/Engine Core/Editor/EngineEditor.h"
#include <imgui_internal.h>

void EngineEditor::_init(const Window& window) {
    #if _DEBUG
        RegisterDebugCallback();
    #endif

    EditorCamera.init(window, glm::vec3{ -300.f, 0.f, 0.f });
    aspectRatio = viewportSize.x / viewportSize.y;

    vendor = (const char*)glGetString(GL_VENDOR);
    renderer = (const char*)glGetString(GL_RENDERER);
    version = (const char*)glGetString(GL_VERSION);
    glslVersion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    viewportMatrix = glm::mat4{
        glm::vec4((float)window.getBufferWidth() / 2.f, 0.f, 0.f, (float)window.getBufferWidth() / 2.f),
        glm::vec4(0.f, (float)window.getBufferHeight() / 2.f, 0.f, (float)window.getBufferHeight() / 2.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    //io->Fonts->AddFontDefault();
    mainfont = io->Fonts->AddFontFromFileTTF("src/Content/Fonts/Montserrat-Regular.ttf", 17.5f, NULL);
    IM_ASSERT(mainfont != NULL);

    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io->BackendFlags |= ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_RendererHasViewports;

    style = &ImGui::GetStyle();

    style->DockingSeparatorSize = 3.f;
    style->TabBarOverlineSize = 0.f;
    style->WindowBorderSize = 0.f;
    style->WindowRounding = 5.f;
    style->TabRounding = 2.f;
    style->GrabRounding = 12.f;
    //style->FrameRounding = 12.f;
    style->SeparatorTextPadding = ImVec2(25.f, 15.f);

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
    style->Colors[ImGuiCol_DockingPreview] = ImVec4(0.f, 0.6f, 0.6f, 1.f);
    style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.f, 0.6f, 0.6f, 1.f);
}

void EngineEditor::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    CreateDockingSpace();
}

void EngineEditor::Render(GLFWwindow* window) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(window);
    }
}

void EngineEditor::CreateDockingSpace() {
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

void EngineEditor::DrawVec3Control(const std::string& label, float* values, float resetValue, float columnWidth) {
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

void EngineEditor::DrawDragFloat(const std::string& label, float* values, float speed, float columnWidth, float min, float max) {
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
    ImGui::DragFloat(labelID.c_str(), values, speed, min, max, "%.4f");
    ImGui::Columns(1);
    ImGui::PopID();
}

void EngineEditor::DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth)
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

template<typename T>
int EngineEditor::DrawCombo(ComboItem<T>& item, int& selection) 
{
    int previousSelection{ 0 };

    ImGui::PushID(item.label.c_str());
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, 150.f);
    float dragFloatHeight = ImGui::GetFrameHeight();
    float textHeight = ImGui::CalcTextSize(item.label.c_str()).y;
    float verticalOffset = (dragFloatHeight - textHeight) * 0.5f;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + verticalOffset);
    ImGui::Text(item.label.c_str());
    ImGui::NextColumn();

    if (ImGui::BeginCombo(("##" + item.label).c_str(), item.displayVals[selection].c_str())) {
        previousSelection = selection;

        for (size_t i = 0; i < item.displayVals.size(); i++) {
            if (ImGui::Selectable(item.displayVals[i].c_str(), &item.flags)) {
                selection = (int)i;
                item.flags[i] = !item.flags[i];
            }
        }

        ImGui::EndCombo();
    }

    ImGui::Columns(1);
    ImGui::PopID();

    return previousSelection;
}

void EngineEditor::manipulate(float x, float y, float width, float height) {
    ImGuizmo::AllowAxisFlip(false);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::Enable(true);
    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(x, y, width, height);

    if (entityID.y == false) {
        ImGuizmo::Manipulate(
            glm::value_ptr(EditorCamera.generateViewMatrix()), glm::value_ptr(EditorCamera.getProjectionMatrix()),
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
            glm::value_ptr(EditorCamera.generateViewMatrix()), glm::value_ptr(EditorCamera.getProjectionMatrix()),
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

void EngineEditor::DrawToolbar(const ImGuiViewport* const viewport) {
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowPos(ImVec2(viewportPos.x + 5.f, viewportPos.y + 5.f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(viewportSize.x, 20));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.f);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;
    flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;

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
}

void EngineEditor::DrawRenderingModifiers() {
    ImGui::Begin("Rendering");

    if (ImGui::CollapsingHeader("HDR", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int msaaSelection = 0;
        static int frameRateSelection = 0;

        DrawDragFloat("Exposure", &exposure, 0.01f, 150.f, 0.f, 10.f);
        DrawDragFloat("Emissive Strength", &emissiveMaterialStrength, 0.01f, 150.f, 0.f, 10000.f);

        int previousSelection = DrawCombo<int>(MSAA, msaaSelection);

        if (previousSelection != msaaSelection) {
            HDR::Framebuffer().updateSamplesMSAA(viewportDimensions.z, viewportDimensions.w, MSAA.values[msaaSelection]);
            MSAA.flags[previousSelection] = false;
        }

        previousSelection = DrawCombo<float>(frameRate, frameRateSelection);

        if (previousSelection != frameRateSelection) {
            frameRateCap = frameRate.values[frameRateSelection];
            frameRate.flags[previousSelection] = false;
        }

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Bloom", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawDragFloat("Filter Radius", &filterRadius, 0.0001f, 150.f, 0.f, 1.f);
        DrawDragFloat("Threshold", &bloomThreshold, 0.01f, 150.f, 0.f, FLT_MAX);
        DrawDragFloat("Knee", &bloomKnee, 0.0001f, 150.f, 0.f, 1.f);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Shadows", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawDragFloat("PCF Filter Radius", &shadowRadius, 0.001f, 150.f, 0.f, 10.f);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("SSAO", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawDragFloat("Radius", &ssaoRadius, 0.1f, 150.f, 0.f, FLT_MAX);
        DrawDragFloat("Bias", &ssaoBias, 0.001f, 150.f, 0.f, FLT_MAX);
        DrawDragFloat("Occlusion Power", &ssaoOcclusionPower, 1.f, 150.f, 0.f, FLT_MAX);

        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    ImGui::Text("Vendor : %s", vendor);
    ImGui::Text("Renderer : %s", renderer);
    ImGui::Text("OpenGL version : %s", version);
    ImGui::Text("GLSL version : %s", glslVersion);

    ImGui::End();
}

void EngineEditor::DrawDebugLog() {
    ImGui::Begin("Debug Log");

    if (ImGui::Button("Clear"))
        g_DebugLogEntries.clear();

    ImGui::SameLine();

    if (ImGui::Button("Copy"))
        ImGui::LogToClipboard();

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);

    ImGui::BeginChild("##log", ImVec2(0.f, 0.f), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar);

    ImVec2 pos = ImGui::GetCursorScreenPos();
    const float lineHeight = ImGui::GetTextLineHeight();
    bool firstFragment = true, endsWithNewLine = true;

    ImGuiListClipper clipper;
    clipper.Begin((int)g_DebugLogEntries.size(), lineHeight);

    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
            firstFragment = true;
            const DebugLogEntry& entry = g_DebugLogEntries[i];
            ImVec2 linePos = pos;

            for (const auto& frag : entry.fragments) {
                bool endsWithNewline = (!frag.text.empty() && frag.text.back() == '\n');
                ImGui::PushStyleColor(ImGuiCol_Text, frag.color);
                if (firstFragment) {
                    ImGui::TextUnformatted(frag.text.c_str());
                    firstFragment = false;
                }
                else {
                    ImGui::SameLine(0.0f, 0.0f);
                    ImGui::TextUnformatted(frag.text.c_str());
                }
                ImGui::PopStyleColor();
                if (endsWithNewline) {
                    firstFragment = true;
                }
            }
            pos.y += lineHeight;
            pos.x = ImGui::GetCursorScreenPos().x;
        }
    }
    clipper.End();

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

void EngineEditor::ResizeViewportForward() {
    viewportDimensions.z = (int)viewportSize.x;
    viewportDimensions.w = (int)viewportSize.y;

    glViewport(viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w);

    HDR::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w);
    PostProcessMaster::Framebuffer().resize(viewportDimensions);

    EditorCamera.setProjectionAspect(viewportSize.x / viewportSize.y);

    viewportMatrix = glm::mat4{
        glm::vec4(viewportDimensions.z / 2.f, 0.f, 0.f, viewportDimensions.z / 2.f),
        glm::vec4(0.f, viewportDimensions.w / 2.f, 0.f, viewportDimensions.w / 2.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };
}

void EngineEditor::ResizeViewportDeferred() {
    viewportDimensions.z = (int)viewportSize.x;
    viewportDimensions.w = (int)viewportSize.y;

    glViewport(viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w);

    HDR::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w);
    PostProcessMaster::Framebuffer().resize(viewportDimensions);
    GBuffer::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w);
    SSAO::Framebuffer().resize(viewportDimensions.z, viewportDimensions.w);

    EditorCamera.setProjectionAspect(viewportSize.x / viewportSize.y);

    viewportMatrix = glm::mat4{
        glm::vec4(viewportDimensions.z / 2.f, 0.f, 0.f, viewportDimensions.z / 2.f),
        glm::vec4(0.f, viewportDimensions.w / 2.f, 0.f, viewportDimensions.w / 2.f),
        glm::vec4(0.f, 0.f, 1.f, 0.f),
        glm::vec4(0.f, 0.f, 0.f, 1.f)
    };
}

void EngineEditor::entityPicking(const Window& window) {
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
}

void EngineEditor::DrawSceneViewportForward(const Window& window) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
    ImGui::Begin("Scene Viewport");

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    float framePadding = ImGui::GetFontSize() + style->FramePadding.y * 2.f;

    viewportPos = ImGui::GetWindowPos();  
    viewportSize = ImGui::GetContentRegionAvail();

    viewportPos.y += (ImGui::GetWindowSize().y - viewportSize.y);

    if (viewportDimensions.z != (int)viewportSize.x || viewportDimensions.w != (int)viewportSize.y)
        ResizeViewportForward();

    ImGui::Image((ImTextureID)PostProcessMaster::Framebuffer().ColorbufferID(), viewportSize, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

    DrawToolbar(viewport);

    snprintf(FPS, sizeof(FPS), "%.2f FPS", io->Framerate);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 textPosition = ImVec2(viewportPos.x + 10.f, viewportPos.y + viewportSize.y - 20.f);
    drawList->AddText(textPosition, IM_COL32(10, 255, 50, 255), FPS);

    entityPicking(window);

    if (entityID.x != -1) {
        updateTransformOperation();
        manipulate(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void EngineEditor::DrawSceneViewportDeferred(const Window& window) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.f, 0.f });
    ImGui::Begin("Scene Viewport");

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    float framePadding = ImGui::GetFontSize() + style->FramePadding.y * 2.f;

    viewportPos = ImGui::GetWindowPos();
    viewportSize = ImGui::GetContentRegionAvail();

    viewportPos.y += ImGui::GetWindowSize().y - viewportSize.y;

    if (viewportDimensions.z != (int)viewportSize.x || viewportDimensions.w != (int)viewportSize.y)
        ResizeViewportDeferred();

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

    DrawToolbar(viewport);

    snprintf(FPS, sizeof(FPS), "%.2f FPS", io->Framerate);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 textPosition = ImVec2(viewportPos.x + 10.f, viewportPos.y + viewportSize.y - 20.f);
    drawList->AddText(textPosition, IM_COL32(10, 255, 50, 255), FPS);

    entityPicking(window);

    if (entityID.x != -1) {
        updateTransformOperation();
        manipulate(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
    }

    ImGui::PopStyleVar(1);
    ImGui::End();
}

void EngineEditor::updateTransformOperation() {
    if (ImGui::IsKeyPressed(ImGuiKey_T))
        transformOperation = ImGuizmo::TRANSLATE;

    else if (ImGui::IsKeyPressed(ImGuiKey_R))
        transformOperation = ImGuizmo::ROTATE;

    else if (ImGui::IsKeyPressed(ImGuiKey_E))
        transformOperation = ImGuizmo::SCALE;
}

void EngineEditor::DrawDetailsPanel(const Window& window) {
    static bool showStyleEditor = false;
    static bool swapInterval = enableVsync;

    if (showStyleEditor) ImGui::ShowStyleEditor(this->style);

    ImGui::Begin("Details Panel");

    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Render Skybox", &drawSkybox);
        ImGui::Checkbox("Enable HDR", &enableHDR);
        ImGui::Checkbox("Enable Bloom", &enableBloom);
        ImGui::Checkbox("Enable Shadows", &enableShadows);
        ImGui::Checkbox("Enable SSAO", &enableSSAO);
        ImGui::Checkbox("Enable V-Sync", &enableVsync);
        ImGui::Checkbox("Style Editor", &showStyleEditor);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    if (swapInterval != enableVsync) {
        swapInterval = enableVsync;
        glfwSwapInterval(enableVsync);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::SeparatorTextEx(ImGui::GetID("Material Properties"), "Material Properties", NULL, 3.f);
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

        DrawDragFloat("Metallic", &metallic, 0.001f, 120.f, 0.f, 1.f);
        DrawDragFloat("Roughness", &roughness, 0.001f, 120.f, 0.f, 1.f);
        DrawDragFloat("AO", &ao, 0.001f, 120.f, 0.f, 1.f);

        ImGui::SeparatorTextEx(ImGui::GetID("Transforms"), "Transforms", NULL, 3.f);
        DrawVec3Control("Translation", translation);
        DrawVec3Control("Rotation", rotation);
        DrawVec3Control("Scale", scale, 1.f);

        ImGui::NewLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen)) {
        glm::vec3 pointLightDirection{ 0.f };
        glm::vec3 spotLightDirection{ 0.f };
        DrawVec3Control("Directional Light", glm::value_ptr(directionalLightPosition), 0.f, 150.f);
        DrawVec3Control("Point Light", glm::value_ptr(pointLightDirection), 0.f, 150.f);
        DrawVec3Control("Spot Light", glm::value_ptr(spotLightDirection), 0.f, 150.f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 3.f);
    }

    ImGui::NewLine();

    ImGui::Text(
        "Viewport Dimensions : (%i, %i, %i, %i)",
        viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w
    );
    ImGui::Text("Mouse Cursor Position : (%.2f, %.2f)", mousePos.x, mousePos.y);
    ImGui::Text("Entity Index : (%i, %i)", entityID.x, entityID.y);

    ImGui::NewLine();
    ImGui::Text("Application average: %.4f ms/frame", 1000.f / io->Framerate);

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

EngineEditor::~EngineEditor() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}