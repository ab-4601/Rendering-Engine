#include "../../headers/Engine Core/Framebuffer.h"

bool Framebuffer::checkFramebufferStatus() {
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			return true;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cerr << "Framebuffer format not supported" << std::endl;
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cerr << "Framebuffer has incomplete attachment" << std::endl;
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cerr << "Framebuffer is missing an attachment" << std::endl;
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cerr << "Framebuffer draw buffer is incomplete" << std::endl;
			return false;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cerr << "Framebuffer read buffer is incomplete" << std::endl;
			return false;
		default:
			std::cerr << "Unknown framebuffer error" << std::endl;
			return false;
	}
}