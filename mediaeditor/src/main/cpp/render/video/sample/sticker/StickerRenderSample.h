//
// Created by 江守亭 on 2021/10/21.
//

#ifndef JUSTMEDIAEDITOR_STICKERRENDERSAMPLE_H
#define JUSTMEDIAEDITOR_STICKERRENDERSAMPLE_H

#include <BaseSample.h>
#include <queue>
using namespace glm;
struct StickerDisplayOption {
	FrameImage *m_StickerImage;
	long startTime;
	long endTime;
	vec2 m_Position;
};
class StickerRenderSample : public BaseSample {
public:
	StickerRenderSample();
	~StickerRenderSample();

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();

	void SetStickerDisplayOptions(std::deque<StickerDisplayOption> stickerDisplayOptions);

private:
	void RenderSticker(const StickerDisplayOption& stickerDisplayOption);
	vec2 GetStickerImageTranslate(const vec2& stickerImageSize, vec2 position);

private:
	std::deque<StickerDisplayOption> m_StickerDisplayOptions;
	GLuint m_StickerProgramId;
	GLuint m_StickerTextureId;
};


#endif //JUSTMEDIAEDITOR_STICKERRENDERSAMPLE_H
