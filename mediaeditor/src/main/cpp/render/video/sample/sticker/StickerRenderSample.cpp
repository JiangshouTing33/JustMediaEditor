//
// Created by 江守亭 on 2021/10/21.
//

#include "StickerRenderSample.h"
StickerRenderSample::StickerRenderSample() {
	m_StickerProgramId = GL_NONE;
	m_StickerTextureId = GL_NONE;
}

StickerRenderSample::~StickerRenderSample() = default;

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

const char stickerFShaderStr[] = "#version 300 es\n"
							 "precision highp float;\n"
							 "in vec2 v_texCoord;\n"
							 "layout(location = 0) out vec4 outColor;\n"
							 "uniform sampler2D s_TextureMap;\n"
							 "void main() {\n"
							 "outColor = texture(s_TextureMap, v_texCoord);\n"
							 "}";

int StickerRenderSample::Init() {
	if (m_StickerProgramId != GL_NONE) return 0;

	m_StickerProgramId = GLUtil::CreateProgram(vShaderStr, stickerFShaderStr);

	if (m_StickerProgramId == GL_NONE) return -1;

	glGenTextures(1, &m_StickerTextureId);
	glBindTexture(GL_TEXTURE_2D, m_StickerTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return 0;
}

void StickerRenderSample::UnInit() {
	if (m_StickerProgramId != GL_NONE) {
		glDeleteProgram(m_StickerProgramId);
		m_StickerProgramId = GL_NONE;
	}

	if (m_StickerTextureId != GL_NONE) {
		glDeleteTextures(1, &m_StickerTextureId);
		m_StickerTextureId = GL_NONE;
	}
}

void StickerRenderSample::Draw() {
	if (m_StickerProgramId == GL_NONE || m_StickerTextureId == GL_NONE || m_StickerDisplayOptions.empty()) return;

	for (int i = 0; i < m_StickerDisplayOptions.size(); ++i) {
		RenderSticker(m_StickerDisplayOptions.at(i));
	}
	m_StickerDisplayOptions.clear();
}

void StickerRenderSample::SetStickerDisplayOptions(std::deque<StickerDisplayOption > stickerDisplayOptions) {
	m_StickerDisplayOptions = std::move(stickerDisplayOptions);
}

void StickerRenderSample::RenderSticker(const StickerDisplayOption& stickerDisplayOption) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(m_StickerProgramId);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_StickerTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stickerDisplayOption.m_StickerImage->width, stickerDisplayOption.m_StickerImage->height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, stickerDisplayOption.m_StickerImage->ppPlane[0]);
	GLUtil::setInt(m_StickerProgramId, "s_TextureMap", 6);

	vec2 m_StickerScale;
	m_StickerScale.x = m_DstSize.x == 0 ? 1 : ((float)stickerDisplayOption.m_StickerImage->width / m_DstSize.x);
	m_StickerScale.y = m_DstSize.y == 0 ? 1 : ((float)stickerDisplayOption.m_StickerImage->height / m_DstSize.y);

	vec2 stickerTrans = GetStickerImageTranslate(
		vec2 (stickerDisplayOption.m_StickerImage->width, stickerDisplayOption.m_StickerImage->height),
		stickerDisplayOption.m_Position);

	UpdateMVPMatrix(stickerTrans.x, stickerTrans.y, m_StickerScale.x, m_StickerScale.y);

	GLUtil::setMat4(m_StickerProgramId, "u_MVPMatrix", m_MVPMatrix);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

vec2 StickerRenderSample::GetStickerImageTranslate(const vec2& stickerImageSize, vec2 position) {
	float paddingXMin = -m_DstSize.x * 0.5 / stickerImageSize.x + 0.5;
	float paddingXMax = m_DstSize.x * 0.5 / stickerImageSize.x - 0.5;

	float paddingYMin = -m_DstSize.y * 0.5 / stickerImageSize.y + 0.5;
	float paddingYMax = m_DstSize.y * 0.5 / stickerImageSize.y - 0.5;

	position.x = max(-1.f, position.x);
	position.x = min(1.f, position.x);
	position.y = max(-1.f, position.y);
	position.y = min(1.f, position.y);

	float translateX = (m_DstSize.x * 0.5 * position.x) / stickerImageSize.x;
	float translateY = (m_DstSize.y * 0.5 * position.y) / stickerImageSize.y;

	translateX = max(paddingXMin, translateX);
	translateX = min(paddingXMax, translateX);

	translateY = max(paddingYMin, translateY);
	translateY = min(paddingYMax, translateY);

	return vec2 (translateX, translateY);
}
