//
// Created by 江守亭 on 2021/10/9.
//

#ifndef JUSTMEDIAEDITOR_MARKRENDERSAMPLE_H
#define JUSTMEDIAEDITOR_MARKRENDERSAMPLE_H

#include <BaseSample.h>
class MarkRenderSample : public BaseSample {
public:
	MarkRenderSample();
	~MarkRenderSample();

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();

	/**
	 *
	 * @param markImage
	 * @param positionX  0 - 1
	 * @param positionY  0 - 1
	 */
	void SetMarkImage(FrameImage *markImage, float positionX, float positionY);
	void SetRenderImageSize(int width, int height) {
		m_RenderImageSize.x = width;
		m_RenderImageSize.y = height;
	}

private:
	vec2 GetTrulyMarkTranslate(const vec2& renderImgSize);
private:
	GLuint m_MarkProgramId;
	FrameImage m_MarkImage;
	GLuint m_MarkTextureId;

	vec2 m_MarkPosition;
	vec2 m_MarkTrulySize;
	vec2 m_MarkScale;
	vec2 m_RenderImageSize;
};


#endif //JUSTMEDIAEDITOR_MARKRENDERSAMPLE_H
