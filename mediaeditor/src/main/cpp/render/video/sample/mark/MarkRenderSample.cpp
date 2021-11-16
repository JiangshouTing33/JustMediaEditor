//
// Created by 江守亭 on 2021/10/9.
//

#include "MarkRenderSample.h"
MarkRenderSample::MarkRenderSample() {
	m_MarkProgramId = GL_NONE;
	m_MarkTextureId = GL_NONE;
}

MarkRenderSample::~MarkRenderSample() {
	FrameImageUtil::FreeFrameImage(&m_MarkImage);
}

void MarkRenderSample::SetMarkImage(FrameImage *markImage, float positionX, float positionY) {
	if (markImage->ppPlane[0]) {
		m_MarkImage.width = markImage->width;
		m_MarkImage.height = markImage->height;
		m_MarkImage.format = markImage->format;

		FrameImageUtil::AllocFrameImage(&m_MarkImage);
		FrameImageUtil::CopyFrameImage(&m_MarkImage, markImage);

		m_MarkPosition.x = positionX;
		m_MarkPosition.y = positionY;

		m_MarkScale.x = m_DstSize.x == 0 ? 1 : ((float)m_MarkImage.width / m_DstSize.x);
		m_MarkScale.y = m_DstSize.y == 0 ? 1 : ((float)m_MarkImage.height / m_DstSize.y);

		m_MarkTrulySize.x = (m_DstSize.x == 0 ? m_MarkImage.width : m_DstSize.x) * m_MarkScale.x;
		m_MarkTrulySize.y = (m_DstSize.y == 0 ? m_MarkImage.height : m_DstSize.y) * m_MarkScale.y;
	}
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

const char markShaderStr[] = "#version 300 es\n"
							 "precision highp float;\n"
							 "in vec2 v_texCoord;\n"
							 "layout(location = 0) out vec4 outColor;\n"
							 "uniform sampler2D s_TextureMap;\n"
							 "void main() {\n"
							 "outColor = texture(s_TextureMap, v_texCoord);\n"
							 "}";

int MarkRenderSample::Init() {
	if (m_MarkProgramId != GL_NONE) return 0;

	m_MarkProgramId = GLUtil::CreateProgram(vShaderStr, markShaderStr);

	if (m_MarkProgramId == GL_NONE) return -1;

	glGenTextures(1, &m_MarkTextureId);
	glBindTexture(GL_TEXTURE_2D, m_MarkTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return 0;
}

void MarkRenderSample::UnInit() {
	if (m_MarkProgramId != GL_NONE) {
		glDeleteProgram(m_MarkProgramId);
		m_MarkProgramId = GL_NONE;

		glDeleteTextures(1, &m_MarkTextureId);
		m_MarkTextureId = GL_NONE;
	}
}

void MarkRenderSample::Draw() {
	if (m_MarkProgramId != GL_NONE && m_MarkImage.ppPlane[0] && m_MarkPosition.x >= 0 && m_MarkPosition.y >= 0) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(m_MarkProgramId);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_MarkTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_MarkImage.width, m_MarkImage.height,
					 0, GL_RGBA, GL_UNSIGNED_BYTE, m_MarkImage.ppPlane[0]);
		GLUtil::setInt(m_MarkProgramId, "s_TextureMap", 4);

		vec2 renderImgSize = GetTrulyDisplayImageSize(m_RenderImageSize.x, m_RenderImageSize.y);
		vec2 markTranslate = GetTrulyMarkTranslate(renderImgSize);
		UpdateMVPMatrix(markTranslate.x, markTranslate.y, m_MarkScale.x, m_MarkScale.y);

		GLUtil::setMat4(m_MarkProgramId, "u_MVPMatrix", m_MVPMatrix);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		glDisable(GL_BLEND);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}
}

vec2 MarkRenderSample::GetTrulyMarkTranslate(const vec2& renderImgSize) {
	float leftPadding = 0;
	float rightPadding = 0;
	float topPadding = 0;
	float bottomPadding = 0;

	if (m_MarkPosition.x < 0.1f) {
		m_MarkPosition.x = 0;
		leftPadding = 0.6f;
	} else if (m_MarkPosition.x >= 0.9f) {
		m_MarkPosition.x = 1;
		rightPadding = -0.75f;
	}

	if (m_MarkPosition.y < 0.1f) {
		m_MarkPosition.y = 0;
		topPadding = 0.7f;
	} else if (m_MarkPosition.y >= 0.9f) {
		m_MarkPosition.y = 1;
		bottomPadding = -0.7f;
	}

	float markTranslateX = -renderImgSize.x / m_MarkTrulySize.x + renderImgSize.x * 2 * m_MarkPosition.x / m_MarkTrulySize.x + leftPadding + rightPadding;
	float markTranslateY = -renderImgSize.y / m_MarkTrulySize.y + renderImgSize.y * 2 * m_MarkPosition.y / m_MarkTrulySize.y + topPadding + bottomPadding;

	return vec2 (markTranslateX, markTranslateY);
}
