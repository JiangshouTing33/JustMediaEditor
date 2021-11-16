//
// Created by 江守亭 on 2021/10/9.
//

#ifndef JUSTMEDIAEDITOR_FRAMERENDERSAMPLE_H
#define JUSTMEDIAEDITOR_FRAMERENDERSAMPLE_H

#include <BaseSample.h>
#define TEXTURE_ID_LENGTH 3
class FrameRenderSample : public BaseSample {
public:
	FrameRenderSample() {}
	virtual ~FrameRenderSample();
	void SetSampleFrameData(FrameImage *image);

protected:
	int InitTextureId(const char *pVertexShaderSource, const char *pFragShaderSource);
	void UnInitTextureId();
	void DrawTextureInner();
protected:
	FrameImage *m_RenderImage;
	GLuint m_TextureIds[TEXTURE_ID_LENGTH];
	std::mutex m_Mutex;
};


#endif //JUSTMEDIAEDITOR_FRAMERENDERSAMPLE_H
