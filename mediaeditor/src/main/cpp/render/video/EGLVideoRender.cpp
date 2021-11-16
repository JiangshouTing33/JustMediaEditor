//
// Created by 江守亭 on 2021/10/9.
//

#include "EGLVideoRender.h"

#include <utility>
EGLVideoRender::EGLVideoRender() {
	m_eglSurface = EGL_NO_SURFACE;
	m_eglDisplay = EGL_NO_DISPLAY;
	m_eglContext = EGL_NO_CONTEXT;

	m_VaoId = GL_NONE;
	m_FboId = GL_NONE;
	m_FboTextureId = GL_NONE;
	m_PboId = GL_NONE;
}

EGLVideoRender::~EGLVideoRender() = default;

void EGLVideoRender::SetDstSize(int width, int height) {
	m_DstSize.x = width;
	m_DstSize.y = height;

	if (m_FrameRenderSample) {
		m_FrameRenderSample->SetDstSize(m_DstSize.x, m_DstSize.y);
	}

	if (m_MarkRenderSample) {
		m_MarkRenderSample->SetDstSize(m_DstSize.x, m_DstSize.y);
	}

	if (m_FontRenderSample) {
		m_FontRenderSample->SetDstSize(m_DstSize.x, m_DstSize.y);
	}

	if (m_StickerRenderSample) {
		m_StickerRenderSample->SetDstSize(m_DstSize.x, m_DstSize.y);
	}
}

void EGLVideoRender::SetFrameRenderSample(FrameRenderSample *frameRenderSample) {
	m_FrameRenderSample = frameRenderSample;
}

void EGLVideoRender::SetMarkRenderSample(MarkRenderSample *markRenderSample) {
	m_MarkRenderSample = markRenderSample;
}

void EGLVideoRender::SetFontRenderSample(FontRenderSample *fontRenderSample) {
	m_FontRenderSample = fontRenderSample;
}

void EGLVideoRender::SetStickerRenderSample(StickerRenderSample *stickerRenderSample) {
	m_StickerRenderSample = stickerRenderSample;
}

void EGLVideoRender::SetTextDisplayParams(std::deque<TextDisplayParam *> textDisplayParams) {
	if (m_FontRenderSample) {
		m_FontRenderSample->SetTextDisplayParams(std::move(textDisplayParams));
	}
}

void EGLVideoRender::SetStickerDisplayOptions(std::deque<StickerDisplayOption> options) {
	if (m_StickerRenderSample) {
		m_StickerRenderSample->SetStickerDisplayOptions(std::move(options));
	}
}

void EGLVideoRender::LoadFontFace(std::deque<TextDisplayParam *> displayTextQueue) {
	if (m_FontRenderSample) {
		m_FontRenderSample->LoadFontFace(std::move(displayTextQueue));
	}
}

GLfloat verticesCoords[] = {
	-1.0f,  1.0f, 0.0f,  // Position 0
	-1.0f, -1.0f, 0.0f,  // Position 1
	1.0f,  -1.0f, 0.0f,  // Position 2
	1.0f,   1.0f, 0.0f,  // Position 3
};

GLfloat mFboCoords[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
};

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

int EGLVideoRender::Init() {
	m_IsEglEnvReady = CreateEGLEnv() == 0;

	if (!m_IsEglEnvReady) {
		LOGCATE("EGLVideoRender::Init...CreateEGLEnv Failed!");
		return -1;
	}

	if (m_FrameRenderSample == nullptr) {
		return 0;
	}

	if (m_FrameRenderSample->Init() != 0) {
		LOGCATE("EGLVideoRender::Init...m_FrameRenderSample->Init() Failed!");
		return -1;
	}

	if (m_MarkRenderSample) {
		int result = m_MarkRenderSample->Init();
		if (result != 0) {
			LOGCATE("EGLVideoRender::Init...m_MarkRenderSample->Init() Failed!");
			m_MarkRenderSample->UnInit();
			delete m_MarkRenderSample;
			m_MarkRenderSample = nullptr;
		}
	}

	if (m_FontRenderSample) {
		int result = m_FontRenderSample->Init();
		if (result != 0) {
			LOGCATE("EGLVideoRender::Init...m_FontRenderSample->Init() Failed!");
			m_FontRenderSample->UnInit();
			delete m_FontRenderSample;
			m_FontRenderSample = nullptr;
		}
	}

	if (m_StickerRenderSample) {
		int result = m_StickerRenderSample->Init();
		if (result != 0) {
			LOGCATE("EGLVideoRender::Init...m_StickerRenderSample->Init() Failed!");
			m_StickerRenderSample->UnInit();
			delete m_StickerRenderSample;
			m_StickerRenderSample = nullptr;
		}
	}

	glGenBuffers(3, m_VboIds);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mFboCoords), mFboCoords, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_VaoId);
	glBindVertexArray(m_VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
	glBindVertexArray(GL_NONE);

	return 0;
}

void EGLVideoRender::UnInit() {
	if (m_FrameRenderSample) {
		m_FrameRenderSample->UnInit();
		delete m_FrameRenderSample;
		m_FrameRenderSample = nullptr;
	}

	if (m_MarkRenderSample) {
		m_MarkRenderSample->UnInit();
		delete m_MarkRenderSample;
		m_MarkRenderSample = nullptr;
	}

	if (m_FontRenderSample) {
		m_FontRenderSample->UnInit();
		delete m_FontRenderSample;
		m_FontRenderSample = nullptr;
	}

	if (m_StickerRenderSample) {
		m_StickerRenderSample->UnInit();
		delete m_StickerRenderSample;
		m_StickerRenderSample = nullptr;
	}

	if (m_VaoId != GL_NONE) {
		glDeleteVertexArrays(1, &m_VaoId);
		m_VaoId = GL_NONE;
	}

	glDeleteBuffers(3, m_VboIds);

	if (m_FboId != GL_NONE) {
		glDeleteFramebuffers(1, &m_FboId);
		m_FboId = GL_NONE;
	}

	if (m_FboTextureId != GL_NONE) {
		glDeleteTextures(1, &m_FboTextureId);
		m_FboTextureId = GL_NONE;
	}

	if (m_IsEglEnvReady) {
		DestroyEGLEnv();
		m_IsEglEnvReady = false;
	}

	if (m_PboId != GL_NONE) {
		glDeleteBuffers(1, &m_PboId);
		m_PboId = GL_NONE;
	}
}

void EGLVideoRender::Draw() {
	if (m_FrameRenderSample == nullptr) {
		if (m_RenderFrameCallback && m_RenderFrameCtx) {
			m_RenderFrameCallback(m_RenderFrameCtx, &m_RenderImage);
		}

		if (m_RenderDoneCallback && m_RenderDoneCtx) {
			m_RenderDoneCallback(m_RenderDoneCtx);
		}
		return;
	}

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glViewport(0, 0, m_DstSize.x, m_DstSize.y);

	if (CreateFBOObj()) {
		LOGCATE("VideoEGLRender::Draw...CreateFBOObj Failed!");
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
	glBindVertexArray(m_VaoId);
	glClear(GL_COLOR_BUFFER_BIT);
	m_FrameRenderSample->Draw();
	if (m_MarkRenderSample) {
		m_MarkRenderSample->Draw();
	}

	if (m_StickerRenderSample) {
		m_StickerRenderSample->Draw();
	}

	if (m_FontRenderSample) {
		m_FontRenderSample->Draw();
	}

	GetRenderFrameFromPBO();
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);

	if (m_RenderDoneCallback && m_RenderDoneCtx) {
		m_RenderDoneCallback(m_RenderDoneCtx);
	}
}

void EGLVideoRender::SetSampleFrameData(FrameImage *image) {
	if (m_FrameRenderSample) {
		m_FrameRenderSample->SetSampleFrameData(image);
	}

	if (m_MarkRenderSample) {
		m_MarkRenderSample->SetRenderImageSize(image->width, image->height);
	}
}

int EGLVideoRender::CreateEGLEnv() {
	// EGL config attributes
	const EGLint confAttr[] =
		{
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
			EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
			EGL_RED_SIZE,   8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE,  8,
			EGL_ALPHA_SIZE, 8,// if you need the alpha channel
			EGL_DEPTH_SIZE, 16,// if you need the depth buffer
			EGL_STENCIL_SIZE,8,
			EGL_NONE
		};

	// EGL context attributes
	const EGLint ctxAttr[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	// surface attributes
	// the surface size is set to the input frame size
	const EGLint surfaceAttr[] = {
		EGL_WIDTH, 1,
		EGL_HEIGHT,1,
		EGL_NONE
	};

	EGLint eglMaxVer, eglMinVer;
	EGLint numConfig;

	int resultCode = 0;

	do {
		m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if (m_eglDisplay == EGL_NO_DISPLAY) {
			LOGCATE("EGLVideoRender::CreateEGLEnv Unable to open connection to local windowing system");
			resultCode = -1;
			break;
		}

		if (!eglInitialize(m_eglDisplay, &eglMaxVer, &eglMinVer)) {
			LOGCATE("EGLVideoRender::CreateEGLEnv Unable to initialize EGL");
			resultCode = -1;
			break;
		}

		if (!eglChooseConfig(m_eglDisplay, confAttr, &m_eglConfig, 1, &numConfig)) {
			LOGCATE("EGLVideoRender::CreateEGLEnv some config is wrong");
			resultCode = -1;
			break;
		}

		m_eglSurface = eglCreatePbufferSurface(m_eglDisplay, m_eglConfig, surfaceAttr);

		if (m_eglSurface == EGL_NO_SURFACE) {
			switch (eglGetError()) {
				case EGL_BAD_ALLOC:
					// Not enough resources available. Handle and recover
					LOGCATE("EGLVideoRender::CreateEGLEnv Not enough resources available");
					break;
				case EGL_BAD_CONFIG:
					// Verify that provided EGLConfig is valid
					LOGCATE("EGLVideoRender::CreateEGLEnv provided EGLConfig is invalid");
					break;
				case EGL_BAD_PARAMETER:
					// Verify that the EGL_WIDTH and EGL_HEIGHT are
					// non-negative values
					LOGCATE("EGLVideoRender::CreateEGLEnv provided EGL_WIDTH and EGL_HEIGHT is invalid");
					break;
				case EGL_BAD_MATCH:
					// Check window and EGLConfig attributes to determine
					// compatibility and pbuffer-texture parameters
					LOGCATE("EGLVideoRender::CreateEGLEnv Check window and EGLConfig attributes");
					break;
			}
		}

		m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, ctxAttr);
		if (m_eglContext == EGL_NO_CONTEXT) {
			EGLint error = eglGetError();
			if (error == EGL_BAD_CONFIG) {
				// Handle error and recover
				LOGCATE("EGLVideoRender::CreateEGLEnv EGL_BAD_CONFIG");
				resultCode = -1;
				break;
			}
		}

		if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext)) {
			LOGCATE("EGLVideoRender::CreateEGLEnv MakeCurrent failed");
			resultCode = -1;
			break;
		}
	} while (false);

	if (resultCode != 0) {
		LOGCATE("EGLVideoRender::CreateEGLEnv fail, ret = %d", resultCode);
	}

	return resultCode;
}

void EGLVideoRender::DestroyEGLEnv() {
	if (m_eglDisplay != EGL_NO_DISPLAY) {
		eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(m_eglDisplay, m_eglContext);
		eglDestroySurface(m_eglDisplay, m_eglSurface);
		eglReleaseThread();
		eglTerminate(m_eglDisplay);
	}

	m_eglDisplay = EGL_NO_DISPLAY;
	m_eglContext = EGL_NO_CONTEXT;
	m_eglSurface = EGL_NO_SURFACE;
}

int EGLVideoRender::CreateFBOObj() {
	if (m_FboTextureId == GL_NONE) {
		glGenTextures(1, &m_FboTextureId);
		glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
	}
	if (m_FboId == GL_NONE) {
		glGenFramebuffers(1, &m_FboId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FboId);
		glBindTexture(GL_TEXTURE_2D, m_FboTextureId);
		glActiveTexture(GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_DstSize.x, m_DstSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					 nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FboTextureId, 0);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if (status != GL_FRAMEBUFFER_COMPLETE) {
			LOGCATE("VideoEGLRender::CreateFboObj glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE, status = %d", status);
			return -1;
		}
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	}

	return 0;
}

void EGLVideoRender::GetRenderFrameFromPBO() {
	if (m_RenderFrameCallback && m_RenderFrameCtx) {
		int dstWidth = m_DstSize.x;
		int dstHeight = m_DstSize.y;
		int imgByteSize = dstWidth * dstHeight * 4;

		if (m_PboId == GL_NONE) {
			glGenBuffers(1, &m_PboId);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, m_PboId);
			glBufferData(GL_PIXEL_PACK_BUFFER, imgByteSize, nullptr, GL_STREAM_READ);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, GL_NONE);
		}

		glBindBuffer(GL_PIXEL_PACK_BUFFER, m_PboId);
		glReadPixels(0, 0, dstWidth, dstHeight, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		GLubyte *bufPtr = static_cast<GLubyte *>(glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0,
																  imgByteSize,
																  GL_MAP_READ_BIT));

		if (bufPtr) {
			FrameImage frameImage;
			frameImage.width = dstWidth;
			frameImage.height = dstHeight;
			frameImage.format = IMAGE_FORMAT_RGBA;
			frameImage.pLineSize[0] = dstWidth * 4;
			frameImage.ppPlane[0] = bufPtr;
			m_RenderFrameCallback(m_RenderFrameCtx, &frameImage);
		}

		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	}
}