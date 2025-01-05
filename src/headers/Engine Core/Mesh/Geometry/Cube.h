#ifndef _CUBE_H_
#define _CUBE_H_

#include "../Mesh.h"

class Cube : public Mesh {
public:
	Cube(GLenum windingOrder = GL_CCW);

	~Cube() = default;
};

#endif