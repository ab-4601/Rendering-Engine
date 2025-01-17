#include "../../../headers/Engine Core/Editor/MouseSelector.h"

MouseSelector::MouseSelector(int windowWidth, int windowHeight) {
	pixelInfo.init(windowWidth, windowHeight);
}

void MouseSelector::pickingPhase(glm::ivec4 viewportDimensions, const Window& window, uint32_t framebuffer) {
	pixelInfo.enableWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, window.getBufferWidth(), window.getBufferHeight());

	shader.useShader();

	for (size_t i = 0; i < Mesh::meshList.size(); i++) {
		shader.setUint("objectIndex", Mesh::meshList[i]->getObjectID() + 1);
		shader.setUint("drawIndex", static_cast<uint32_t>(i));
		shader.setMat4("model", Mesh::meshList[i]->getModelMatrix());

		Mesh::meshList[i]->drawMesh(GL_TRIANGLES);
	}

	shader.endShader();

	pixelInfo.disableWriting(framebuffer);

	glViewport(viewportDimensions.x, viewportDimensions.y, viewportDimensions.z, viewportDimensions.w);
}

int MouseSelector::mouseSelectionResult(int windowHeight, int x, int y) {
	int clickedObjectID = -1;

	PixelInfo pixel = pixelInfo.readPixel(x, windowHeight - y - 1);

	if (pixel.objectID != 0) {
		clickedObjectID = (int)pixel.objectID - 1;

		return clickedObjectID;
	}

	return -1;
}