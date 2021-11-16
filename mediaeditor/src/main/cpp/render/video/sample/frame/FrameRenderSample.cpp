//
// Created by 江守亭 on 2021/10/9.
//

#include "FrameRenderSample.h"
FrameRenderSample::~FrameRenderSample() {
	FrameImageUtil::FreeFrameImage(m_RenderImage);
}

void FrameRenderSample::SetSampleFrameData(FrameImage *image) {
	m_RenderImage = image;
}

int FrameRenderSample::InitTextureId(const char *pVertexShaderSource, const char *pFragShaderSource) {
	if (m_ProgramId != GL_NONE) {
		return 0;
	}

	m_ProgramId = GLUtil::CreateProgram(pVertexShaderSource, pFragShaderSource);

	if (m_ProgramId == GL_NONE) {
		return -1;
	}

	for (int i = 0; i < TEXTURE_ID_LENGTH; ++i) {
		glGenTextures(1, &m_TextureIds[i]);
		glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}

	return 0;
}

void FrameRenderSample::UnInitTextureId() {
	if (m_ProgramId) {
		glDeleteProgram(m_ProgramId);
		m_ProgramId = GL_NONE;
	}

	glDeleteTextures(TEXTURE_ID_LENGTH, m_TextureIds);
}

void FrameRenderSample::DrawTextureInner() {
	std::unique_lock<std::mutex> lock(m_Mutex);
	glUseProgram(m_ProgramId);

	switch (m_RenderImage->format) {
		case IMAGE_FORMAT_RGBA:
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage->width, m_RenderImage->height,
						 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage->ppPlane[0]);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
			break;
		case IMAGE_FORMAT_NV21:
		case IMAGE_FORMAT_NV12:
			//upload Y plane data
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage->width, m_RenderImage->height,
						 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderImage->ppPlane[0]);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);

			//update UV plane data
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage->width>>1,
						 m_RenderImage->height>>1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderImage->ppPlane[1]);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
			break;
		case IMAGE_FORMAT_I420:
			//upload Y plane data
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage->width, m_RenderImage->height,
						 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderImage->ppPlane[0]);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);

			//update U plane data
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage->width>>1,
						 m_RenderImage->height>>1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderImage->ppPlane[1]);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);

			//update V plane data
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, m_RenderImage->width>>1,
						 m_RenderImage->height>>1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_RenderImage->ppPlane[2]);
			glBindTexture(GL_TEXTURE_2D, GL_NONE);
			break;
	}

	for (int i = 0; i < 3; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
		char samplerName[64] = {0};
		sprintf(samplerName, "s_texture%d", i);
		GLUtil::setInt(m_ProgramId, samplerName, i);
	}

	vec2 renderImgSize = GetTrulyDisplayImageSize(m_RenderImage->width, m_RenderImage->height);
	UpdateMVPMatrix(0, 0,
					m_DstSize.x == 0 ? 1 : ((float)renderImgSize.x / m_DstSize.x),
					m_DstSize.y == 0 ? 1 : ((float)renderImgSize.y / m_DstSize.y));

	GLUtil::setInt(m_ProgramId, "u_nImgType", m_RenderImage->format);
	GLUtil::setMat4(m_ProgramId, "u_MVPMatrix", m_MVPMatrix);
}