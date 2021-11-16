//
// Created by 江守亭 on 2021/10/9.
//

#ifndef JUSTMEDIAEDITOR_EGLVIDEORENDER_H
#define JUSTMEDIAEDITOR_EGLVIDEORENDER_H

#include <BaseVideoRender.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <EGL/eglext.h>
#include <gtc/matrix_transform.hpp>
#include <FrameRenderSample.h>
#include <MarkRenderSample.h>
#include <FontRenderSample.h>
#include <StickerRenderSample.h>

using namespace glm;
class EGLVideoRender : public BaseVideoRender {
public:
    EGLVideoRender();
    ~EGLVideoRender();

    void SetDstSize(int width, int height);
	void SetFrameRenderSample(FrameRenderSample *frameRenderSample);
    void SetMarkRenderSample(MarkRenderSample *markRenderSample);
    void SetFontRenderSample(FontRenderSample *fontRenderSample);
    void SetStickerRenderSample(StickerRenderSample *stickerRenderSample);
	void SetTextDisplayParams(std::deque<TextDisplayParam*> textDisplayParams);
	void SetStickerDisplayOptions(std::deque<StickerDisplayOption> options);
	void LoadFontFace(std::deque<TextDisplayParam*> displayTextQueue);

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();

private:
	virtual void SetSampleFrameData(FrameImage *image);

    int CreateEGLEnv();
    void DestroyEGLEnv();
    int CreateFBOObj();

	void GetRenderFrameFromPBO();
private:
	GLuint m_VaoId;
	GLuint m_VboIds[3];
	GLuint m_FboId;
	GLuint m_FboTextureId;
	GLuint m_PboId;

	EGLSurface m_eglSurface;
	EGLDisplay m_eglDisplay;
	EGLContext m_eglContext;
	EGLConfig m_eglConfig;

	glm::vec2 m_DstSize;
	bool m_IsEglEnvReady = false;

	FrameRenderSample *m_FrameRenderSample = nullptr;
	MarkRenderSample *m_MarkRenderSample = nullptr;
	FontRenderSample *m_FontRenderSample = nullptr;
	StickerRenderSample *m_StickerRenderSample = nullptr;
};


#endif //JUSTMEDIAEDITOR_EGLVIDEORENDER_H
