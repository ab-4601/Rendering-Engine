#include "../../headers/Engine Core/MouseSelector.h"

MouseSelector::MouseSelector(int windowWidth, int windowHeight) {
	pixelInfo.init(windowWidth, windowHeight);
}

void MouseSelector::pickingPhase(const std::vector<Mesh*>& meshes, uint32_t framebuffer)
{
	pixelInfo.enableWriting();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.useShader();

	for (size_t i = 0; i < meshes.size(); i++) {
		shader.setUint("objectIndex", meshes[i]->getObjectID() + 1);
		shader.setUint("drawIndex", static_cast<uint32_t>(i));
		shader.setMat4("model", meshes[i]->getModelMatrix());

		meshes[i]->drawMesh(GL_TRIANGLES);
	}

	shader.endShader();

	pixelInfo.disableWriting(framebuffer);
}

int MouseSelector::mouseSelectionResult(int windowHeight, int x, int y) {
	int clickedObjectID = -1;

	SelectionTexture::PixelInfo pixel = pixelInfo.readPixel(x, windowHeight - y - 1);

	if (pixel.objectID != 0) {
		clickedObjectID = pixel.objectID - 1;

		return clickedObjectID;
	}

	return -1;
}