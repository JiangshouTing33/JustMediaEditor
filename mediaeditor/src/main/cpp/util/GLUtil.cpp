//
// Created by 江守亭 on 2021/9/25.
//

#include "GLUtil.h"
GLuint GLUtil::LoadShader(GLenum shaderType, const char *pSource)
{
    GLuint shader = 0;
        shader = glCreateShader(shaderType);
        if (shader)
        {
            glShaderSource(shader, 1, &pSource, NULL);
            glCompileShader(shader);
            GLint compiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
            if (!compiled)
            {
                GLint infoLen = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
                if (infoLen)
                {
                    char* buf = (char*) malloc((size_t)infoLen);
                    if (buf)
                    {
                        glGetShaderInfoLog(shader, infoLen, NULL, buf);
                        LOGCATE("GLUtils::LoadShader Could not compile shader %d:\n%s\n", shaderType, buf);
                        free(buf);
                    }
                    glDeleteShader(shader);
                    shader = 0;
                }
            }
        }
    return shader;
}

GLuint GLUtil::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle)
{
    GLuint program = 0;
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle) return program;
        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle) return program;

        program = glCreateProgram();
        if (program)
        {
            glAttachShader(program, vertexShaderHandle);
            CheckGLError("glAttachShader");
            glAttachShader(program, fragShaderHandle);
            CheckGLError("glAttachShader");
            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;
            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGCATE("GLUtils::CreateProgram Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
    return program;
}

GLuint GLUtil::CreateProgramWithFeedback(const char *pVertexShaderSource, const char *pFragShaderSource, GLuint &vertexShaderHandle, GLuint &fragShaderHandle, GLchar const **varying, int varyingCount)
{
    GLuint program = 0;
        vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
        if (!vertexShaderHandle) return program;

        fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
        if (!fragShaderHandle) return program;

        program = glCreateProgram();
        if (program)
        {
            glAttachShader(program, vertexShaderHandle);
            CheckGLError("glAttachShader");
            glAttachShader(program, fragShaderHandle);
            CheckGLError("glAttachShader");

            //transform feedback
            glTransformFeedbackVaryings(program, varyingCount, varying, GL_INTERLEAVED_ATTRIBS);
            GO_CHECK_GL_ERROR();

            glLinkProgram(program);
            GLint linkStatus = GL_FALSE;
            glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

            glDetachShader(program, vertexShaderHandle);
            glDeleteShader(vertexShaderHandle);
            vertexShaderHandle = 0;
            glDetachShader(program, fragShaderHandle);
            glDeleteShader(fragShaderHandle);
            fragShaderHandle = 0;
            if (linkStatus != GL_TRUE)
            {
                GLint bufLength = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
                if (bufLength)
                {
                    char* buf = (char*) malloc((size_t)bufLength);
                    if (buf)
                    {
                        glGetProgramInfoLog(program, bufLength, NULL, buf);
                        LOGCATE("GLUtils::CreateProgramWithFeedback Could not link program:\n%s\n", buf);
                        free(buf);
                    }
                }
                glDeleteProgram(program);
                program = 0;
            }
        }
    return program;
}

void GLUtil::DeleteProgram(GLuint &program)
{
    if (program)
    {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void GLUtil::CheckGLError(const char *pGLOperation)
{
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGCATE("GLUtils::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }

}

GLuint GLUtil::CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource) {
    GLuint vertexShaderHandle, fragShaderHandle;
    return CreateProgram(pVertexShaderSource, pFragShaderSource, vertexShaderHandle, fragShaderHandle);
}