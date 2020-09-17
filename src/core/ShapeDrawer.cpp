#include "core/ShapeDrawer.h"

#if defined(_WIN32)
#include <glew.h>
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#else
#include <OpenGL/gl3.h>
#endif
#elif defined(__ANDROID__)
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#endif

std::string line_VShader = R"(
#version 300 es
precision mediump float;
layout(location = 0) in vec3 position;
uniform mat4 viewProjection;
void main()
{
    gl_Position = viewProjection * vec4(position, 1.0);
}
)";

std::string line_FShader = R"(
#version 300 es
precision mediump float;
uniform vec3 color;
out vec4 frag_color;
void main()
{
	frag_color = vec4(color, 1.0);
}
)";

GLfloat line_position[] = {
    0.f, 0.f, 0.f, // Position 0
    0.f, 0.f, 0.f  // Position 1
};

extern GLuint ProcessShaders(const char *vertexShaderCode, const char *fragmentShaderCode);
GLuint line_programID;
GLint viewProjection_location;
GLint color_location;

GLuint m_vertexArrayID;
GLuint m_positionBufferID;
Mat4 m_viewProjection;

namespace ige::creator
{
    void ShapeDrawer::initialize()
    {
        line_programID = ProcessShaders(line_VShader.c_str(), line_FShader.c_str());
        color_location = glGetUniformLocation(line_programID, "color");
        viewProjection_location = glGetUniformLocation(line_programID, "viewProjection");
        pyxie_ogl_assert;

        glGenVertexArrays(1, &m_vertexArrayID);
        glBindVertexArray(m_vertexArrayID);
        pyxie_ogl_assert;

        glGenBuffers(1, &m_positionBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
        pyxie_ogl_assert;
    }

    void ShapeDrawer::setViewProjectionMatrix(const Mat4 &viewProjection)
    {
        m_viewProjection = viewProjection;
    }

    void ShapeDrawer::drawLine(const Vec3 &start, const Vec3 &end, const Vec3 &color, float width)
    {
        line_position[0] = start[0];
        line_position[1] = start[1];
        line_position[2] = start[2];
        line_position[3] = end[0];
        line_position[4] = end[1];
        line_position[5] = end[2];

        bool depthBackup = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(line_programID);
        glUniform3f(color_location, color[0], color[1], color[2]);
        glUniformMatrix4fv(viewProjection_location, 1, GL_FALSE, m_viewProjection.P());
        pyxie_ogl_assert;

        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * 3 * sizeof(GLfloat), &line_position);
        pyxie_ogl_assert;

        glBindVertexArray(m_vertexArrayID);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_positionBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        pyxie_ogl_assert;

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(width);
        pyxie_ogl_assert;

        glDrawArrays(GL_LINES, 0, 2);
        pyxie_ogl_assert;

        glDisableVertexAttribArray(0);
        glBindVertexArray(0);
        pyxie_ogl_assert;
        glLineWidth(1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glUseProgram(0);
        if (depthBackup)
            glEnable(GL_DEPTH_TEST);
    }
} // namespace ige::creator