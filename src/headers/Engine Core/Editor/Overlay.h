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
#include "../Post Processing/PostProcessMaster.h"
#include "../MouseSelector.h"

class Overlay {
private:
	ImGuizmo::OPERATION transformOperation = ImGuizmo::TRANSLATE;
	ImFont* mainfont = nullptr;
	ImGuiIO* io = nullptr;
	ImGuiStyle* style = nullptr;

	uint32_t viewportColorBuffer{ 0 };
	glm::ivec4 viewportDimensions{ 0 };

	ImVec2 viewportPos{ 0.f, 0.f }, mousePos{ 0.f, 0.f }, viewportSize{ 0.f, 0.f };

	float colorBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	float translation[3] = { 0.f, 0.f, 0.f }, rotation[3] = { 0.f, 0.f, 0.f }, scale[3] = { 0.f, 0.f, 0.f };
	float metallic = 0.f, ao = 0.f, roughness = 0.f, aspectRatio = 0.f;

	bool isMouseOverViewport = false, runtimeInformation = false;

	std::vector<bool> selected{ true, false, false };
	std::vector<int> numSamples{ 4, 8, 16 };
	std::vector<std::string> samples{ "MSAA4x", "MSAA8x", "MSAA16x" };

	void DrawVec3Control(const std::string& label, float* values, float resetValue = 0.f, float columnWidth = 120.f);
	void DrawDragFloat(const std::string& label, float* values, float speed = 1.f, float columnWidth = 120.f);
	void DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth = 120.f);
	void DrawCombo(const std::string& label, std::vector<std::string>& options, std::vector<bool>& values, int& index);

	void DrawDriverAndVendorInformation(const Window& window) const;
	void DrawRuntimeInformation() const;
	void CreateDockingSpace();

public:
	Overlay() = default;

	Overlay& operator=(const Overlay&) = delete;
	static Overlay& Editor() {
		static Overlay editor;
		return editor;
	}

	bool drawSkybox = true, enableBloom = true, drawWireframe = false, enableShadows = false;
	bool enableSSAO = false, enableHDR = true, displayCoordinateSystem = false, displayGrid = true;

	int entityID = -1;

	float exposure = 1.f, filterRadius = 0.005f, shadowRadius = 2.f;
	float bloomThreshold = 1.f, ssaoRadius = 50.f, ssaoBias = 2.f, ssaoOcclusionPower = 10.f;

	void _init(const Window& window);

	bool isMouseOverWindow(ImGuiMouseButton mouseButton) const {
		return io->WantCaptureMouse && ImGui::IsMouseClicked(mouseButton);
	}

	int viewportWidth() const { return viewportDimensions.z; }
	int viewportHeight() const { return viewportDimensions.w; }
	const glm::ivec4& getViewportDimensions() const { return viewportDimensions; }

	void CreateSceneViewport(glm::mat4& viewportMatrix, const Window& window, Camera& camera);

	void newFrame();
	void _updateTransformOperation(const Window& window);
	void render(const Window& window, glm::vec3& dirLightLocation);
	void manipulate(float x, float y, float width, float height, const Camera& camera, Mesh* mesh);

	~Overlay();
};

#endif