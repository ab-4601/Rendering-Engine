#ifndef _MOUSE_SELECTOR_H_
#define _MOUSE_SELECTOR_H_

#include "../../Engine Core/Window.h"
#include "../../Engine Core/Mesh/Model.h"
#include "SelectionTexture.h"

class MouseSelector {
private:
	Shader shader{ "src/shaders/selection.vert", "src/shaders/selection.frag" };
	SelectionTexture pixelInfo;

public:
	MouseSelector(int windowWidth, int windowHeight);

	MouseSelector& operator=(const MouseSelector&) = delete;
	static MouseSelector& EntitySelector() {
		static MouseSelector selector{ 800, 600 };
		return selector;
	}
	
	uint32_t selectionTexture() const { return pixelInfo.getSelectionColorBuffer(); }

	void resize(int width, int height) {
		if (width <= 0 || height <= 0) {
			std::cerr << "Invalid framebuffer dimensions" << std::endl;
			return;
		}

		pixelInfo.resize(width, height);
	}
	void pickingPhase(glm::ivec4 viewportDimensions, const Window& window, uint32_t framebuffer = 0);
	glm::ivec2 mouseSelectionResult(int windowHeight, int x, int y);

	~MouseSelector() = default;
};

#endif