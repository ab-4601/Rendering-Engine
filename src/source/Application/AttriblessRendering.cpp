#include "../../headers/Application/AttriblessRendering.h"

using uint32_t = unsigned int;

void bindAttriblessVAO() {
    static uint32_t vao = 0;
    if (vao == 0)
        glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
}

void drawAttriblessQuad() {
    glFrontFace(GL_CCW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void drawAttriblessCube() {
    glFrontFace(GL_CCW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 14);
    glBindVertexArray(0);
}