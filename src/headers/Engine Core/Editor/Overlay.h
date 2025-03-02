#ifndef _OVERLAY_H_
#define _OVERLAY_H_

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include "../Mesh/Mesh.h"
#include "../Camera.h"
#include "../Window.h"
#include "../HDR.h"
#include "../GBuffer.h"
#include "../Post Processing/SSAO.h"
#include "../Post Processing/PostProcessMaster.h"
#include "MouseSelector.h"

template <typename T>
struct ComboItem {
	std::string label = "";
	std::vector<T> values = {};
	std::vector<bool> flags = {};
	std::vector<std::string> displayVals = {};
};

class Overlay {
private:
	ImGuizmo::OPERATION transformOperation = ImGuizmo::TRANSLATE;
	ImFont* mainfont = nullptr;
	ImGuiIO* io = nullptr;
	ImGuiStyle* style = nullptr;

	glm::ivec4 viewportDimensions{ 0 };

	ImVec2 viewportPos{ 0.f, 0.f }, mousePos{ 0.f, 0.f }, viewportSize{ 0.f, 0.f };
	
	char FPS[24] = { '\0' };

	float colorBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	float translation[3] = { 0.f, 0.f, 0.f }, rotation[3] = { 0.f, 0.f, 0.f }, scale[3] = { 0.f, 0.f, 0.f };
	float metallic = 0.f, ao = 0.f, roughness = 0.f, aspectRatio = 0.f;

	bool isMouseOverViewport = false;

	ComboItem<int> MSAA;
	ComboItem<float> frameRate;

	void DrawVec3Control(const std::string& label, float* values, float resetValue = 0.f, float columnWidth = 120.f);
	void DrawDragFloat(const std::string& label, float* values, float speed = 1.f, float columnWidth = 120.f, float min = 0.f, float max = 0.f);
	void DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth = 120.f);

	template <typename T>
	int DrawCombo(ComboItem<T>& item, int& selection);

	void CreateDockingSpace();

	void manipulate(float x, float y, float width, float height);

	void DrawRenderingModifiers(const Window& window);
	void DrawSceneViewportForward(const Window& window);
	void DrawSceneViewportDeferred(const Window& window);
	void DrawDetailsPanel(const Window& window);

	void ResizeViewportForward();
	void ResizeViewportDeferred();

public:
	Overlay() {
		MSAA.label = "MSAA Samples";
		MSAA.flags = { true, false, false };
		MSAA.displayVals = { "MSAA4x", "MSAA8x", "MSAA16x" };
		MSAA.values = { 4, 8, 16 };

		frameRate.label = "Framerate Cap";
		frameRate.flags = { true, false, false, false };
		frameRate.displayVals = { "60 FPS", "80 FPS", "120 FPS", "Disable Framerate Cap" };
		frameRate.values = { 0.016f, 0.012f, 0.008f, 0.f };
	}

	Camera EditorCamera;

	Overlay& operator=(const Overlay&) = delete;
	static Overlay& Editor() {
		static Overlay editor;
		return editor;
	}

	bool drawSkybox = true, enableBloom = true, drawWireframe = false, enableShadows = false, enableVsync = false;
	bool enableSSAO = false, enableHDR = true, displayCoordinateSystem = false, displayGrid = true;

	glm::ivec2 entityID{ -1 };
	glm::vec3 directionalLightPosition{ 3000.f, 3000.f, 0.f };
	glm::mat4 viewportMatrix{ 1.f };

	float exposure = 1.f, filterRadius = 0.005f, shadowRadius = 2.f, bloomKnee = 0.04f, emissiveMaterialStrength = 1.f;
	float bloomThreshold = 1.f, ssaoRadius = 50.f, ssaoBias = 2.f, ssaoOcclusionPower = 10.f, frameRateCap = 0.016f;

	float pcfSamples = 8.f;

	void _init(const Window& window);

	bool isMouseOverWindow(ImGuiMouseButton mouseButton) const {
		return io->WantCaptureMouse && ImGui::IsMouseClicked(mouseButton);
	}

	int viewportWidth() const { return viewportDimensions.z; }
	int viewportHeight() const { return viewportDimensions.w; }
	const glm::ivec4& getViewportDimensions() const { return viewportDimensions; }

	void DrawEditorForward(const Window& window) {
		DrawSceneViewportForward(window);
		DrawRenderingModifiers(window);
		DrawDetailsPanel(window);
	}

	void DrawEditorDeferred(const Window& window) {
		DrawSceneViewportDeferred(window);
		DrawRenderingModifiers(window);
		DrawDetailsPanel(window);
	}

	void newFrame();
	void _updateTransformOperation();
	void Render(GLFWwindow* window);

	~Overlay();
};

#endif