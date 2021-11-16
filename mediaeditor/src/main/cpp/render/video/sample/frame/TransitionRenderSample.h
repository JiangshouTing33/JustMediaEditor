//
// Created by 江守亭 on 2021/10/27.
//

#ifndef JUSTMEDIAEDITOR_TRANSITIONRENDERSAMPLE_H
#define JUSTMEDIAEDITOR_TRANSITIONRENDERSAMPLE_H

#include <FrameRenderSample.h>
class TransitionRenderSample : public FrameRenderSample {
public:
	TransitionRenderSample();
	~TransitionRenderSample();

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();

	void SetFShaderStr(const char *fShader);
	void SetOneFrameImageCount(int count);
	void SetNextRenderImage(FrameImage *image, int imageIndex);

private:
	GLuint m_NextTextureIds;
	FrameImage m_NextImage;
	const char* m_TransitionFShader = nullptr;

	int m_ImageIndex = 0;
	int m_OneFrameImageCount = 1;
};


#endif //JUSTMEDIAEDITOR_TRANSITIONRENDERSAMPLE_H
