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

class Overlay {
private:
	ImGuizmo::OPERATION transformOperation = ImGuizmo::TRANSLATE;
	ImFont* mainfont = nullptr;
	ImGuiIO* io = nullptr;

	uint32_t viewportColorBuffer{ 0 };
	ImVec2 viewportSize{ 0.f, 0.f }, viewportPos{ 0.f, 0.f };

	float colorBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	float translation[3] = { 0.f, 0.f, 0.f };
	float rotation[3] = { 0.f, 0.f, 0.f };
	float scale[3] = { 0.f, 0.f, 0.f };
	float metallic = 0.f;
	float ao = 0.f;
	float roughness = 0.f;
	float aspectRatio = 0.f;

	bool isMouseOverViewport = false;

	void DrawVec3Control(const std::string& label, float* values, float resetValue = 0.f, float columnWidth = 120.f);
	void DrawDragFloat(const std::string& label, float* values, float speed = 1.f, float columnWidth = 120.f);
	void DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth = 120.f);

public:
	Overlay(const Window& window) {
		_init(window);
	};

	void _init(const Window& window);

	bool isMouseOverWindow(ImGuiMouseButton mouseButton) const {
		return io->WantCaptureMouse && ImGui::IsMouseClicked(mouseButton);
	}

	ImVec2 getSceneViewportDimensions() const {
		return viewportSize;
	}

	void CreateDockingSpace();
	void CreateSceneViewport(uint32_t textureID, const Camera& camera, Mesh* mesh = nullptr);

	void newFrame();
	void _updateTransformOperation(const Window& window);
	void render(
		const Window& window, float& exposure, float& shadowRadius, float& filterRadius, float& bloomThreshold,
		float& ssaoRadius, float& ssaoBias, float& ssaoOcclusionPower,
		bool& drawSkybox, bool& displayGrid, bool& displayCoordinateSystem, bool& enableBloom, bool& enableWireframe,
		bool& enableShadows, bool& enableHDR, bool& enableSSAO, glm::vec3& dirLightLocation, Mesh* currMesh = nullptr
	);
	void manipulate(int bufferWidth, int bufferHeight, const Camera& camera, Mesh* mesh);

	~Overlay();
};

#endif