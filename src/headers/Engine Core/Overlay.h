#ifndef _OVERLAY_H_
#define _OVERLAY_H_

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>
#include "Mesh/Mesh.h"
#include "Camera.h"
#include "Window.h"

class Overlay {
private:
	ImGuizmo::OPERATION transformOperation = ImGuizmo::TRANSLATE;
	ImFont* mainfont = nullptr;
	ImGuiIO* io = nullptr;

	float colorBuffer[4] = { 0.f, 0.f, 0.f, 1.f };
	float translation[3] = { 0.f, 0.f, 0.f };
	float rotation[3] = { 0.f, 0.f, 0.f };
	float scale[3] = { 0.f, 0.f, 0.f };
	float metallic = 0.f;
	float ao = 0.f;
	float roughness = 0.f;

	void DrawVec3Control(const std::string& label, float* values, float resetValue = 0.f, float columnWidth = 120.f);
	void DrawDragFloat(const std::string& label, float* values, float speed = 1.f, float columnWidth = 120.f);
	void DrawSliderFloat(const std::string& label, float* values, float min, float max, float columnWidth = 120.f);

public:
	Overlay(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = &ImGui::GetIO(); (void)io;
		_init(window);
	};

	void _init(GLFWwindow* window);

	bool isMouseOverWindow(ImGuiMouseButton mouseButton) {
		return io->WantCaptureMouse && ImGui::IsMouseClicked(mouseButton);
	}

	void _newFrame();
	void _updateTransformOperation(const Window& window);
	void render(
		float& exposure, float& shadowRadius, float& filterRadius, float& bloomThreshold,
		float& ssaoRadius, float& ssaoBias, float& ssaoOcclusionPower,
		bool& drawSkybox, bool& displayGrid, bool& displayCoordinateSystem, bool& enableBloom, bool& enableWireframe,
		bool& enableShadows, bool& enableHDR, bool& enableSSAO, glm::vec3& dirLightLocation, Mesh* currMesh = nullptr
	);
	void manipulate(int bufferWidth, int bufferHeight, const Camera& camera, Mesh* mesh);

	~Overlay();
};

#endif