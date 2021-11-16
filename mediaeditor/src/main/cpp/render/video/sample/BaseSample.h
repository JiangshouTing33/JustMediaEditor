//
// Created by 江守亭 on 2021/10/9.
//

#ifndef JUSTMEDIAEDITOR_BASESAMPLE_H
#define JUSTMEDIAEDITOR_BASESAMPLE_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <mutex>
#include <gtc/matrix_transform.hpp>
#include <ImageDef.h>
#include <GLUtil.h>

using namespace glm;
class BaseSample {
public:
    virtual int Init() = 0;
    virtual void UnInit() = 0;
    virtual void Draw() = 0;

	virtual void SetDstSize(int width, int height) {
		m_DstSize.x = width;
		m_DstSize.y = height;
	}
protected:
	virtual void UpdateMVPMatrix(float translateX, float translateY, float scaleX, float scaleY);
	virtual vec2 GetTrulyDisplayImageSize(int width, int height);

protected:
    GLuint m_ProgramId = GL_NONE;
	glm::mat4 m_MVPMatrix;
	glm::vec2 m_DstSize;
};

#endif //JUSTMEDIAEDITOR_BASESAMPLE_H
