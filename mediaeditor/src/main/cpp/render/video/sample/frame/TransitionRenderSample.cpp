//
// Created by 江守亭 on 2021/10/27.
//

#include <locale>
#include "TransitionRenderSample.h"
TransitionRenderSample::TransitionRenderSample() {
	m_NextTextureIds = GL_NONE;
}

TransitionRenderSample::~TransitionRenderSample() noexcept {
}

const char vShaderStr[] =
	"#version 300 es\n"
	"layout(location = 0) in vec4 a_position;\n"
	"layout(location = 1) in vec2 a_texCoord;\n"
	"uniform mat4 u_MVPMatrix;\n"
	"out vec2 v_texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = u_MVPMatrix * a_position;\n"
	"    v_texCoord = a_texCoord;\n"
	"}";

int TransitionRenderSample::Init() {
	int ret = InitTextureId(vShaderStr, m_TransitionFShader);
	if (ret != 0) {
		LOGCATE("TransitionRenderSample::Init...InitTextureId Failed!");
		return ret;
	}

	glGenTextures(1, &m_NextTextureIds);
	glBindTexture(GL_TEXTURE_2D, m_NextTextureIds);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return 0;
}

void TransitionRenderSample::UnInit() {
	UnInitTextureId();
	if (m_NextTextureIds != GL_NONE) {
		glDeleteTextures(1, &m_NextTextureIds);
		m_NextTextureIds = GL_NONE;
	}
}

void TransitionRenderSample::Draw() {
	if (m_ProgramId == GL_NONE || m_NextTextureIds == GL_NONE) return;

	DrawTextureInner();

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_NextTextureIds);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_NextImage.width, m_NextImage.height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, m_NextImage.ppPlane[0]);
	GLUtil::setInt(m_ProgramId, "s_NextTexture", 4);

	float progress = (m_ImageIndex % m_OneFrameImageCount) * 1.0f / m_OneFrameImageCount;

	GLUtil::setFloat(m_ProgramId, "progress", progress);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void TransitionRenderSample::SetFShaderStr(const char *fShader) {
	m_TransitionFShader = fShader;
}

void TransitionRenderSample::SetOneFrameImageCount(int count) {
	if (count > 1) {
		m_OneFrameImageCount = count + 1;
	}
}

void TransitionRenderSample::SetNextRenderImage(FrameImage *image, int imageIndex) {
	m_ImageIndex = imageIndex;
	if (image->ppPlane[0]) {
		m_NextImage.width = image->width;
		m_NextImage.height = image->height;
		m_NextImage.format = image->format;
		FrameImageUtil::AllocFrameImage(&m_NextImage);
		FrameImageUtil::CopyFrameImage(&m_NextImage, image);
	}
}
