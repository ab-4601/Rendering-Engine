#ifndef _MOUSE_SELECTOR_H_
#define _MOUSE_SELECTOR_H_

#include "../Engine Core/Window.h"
#include "../Engine Core/Mesh/Model.h"
#include "SelectionTexture.h"

class MouseSelector {
private:
	Shader shader{ "src/shaders/selection.vert", "src/shaders/selection.frag" };
	SelectionTexture pixelInfo;

public:
	MouseSelector(int windowWidth, int windowHeight);

	void pickingPhase(const std::vector<Mesh*>& meshes, uint32_t framebuffer = 0);
	int mouseSelectionResult(int windowHeight, int x, int y);

	~MouseSelector() = default;
};

#endif