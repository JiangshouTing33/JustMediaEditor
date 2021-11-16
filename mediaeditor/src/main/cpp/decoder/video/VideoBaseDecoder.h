//
// Created by 江守亭 on 2021/10/22.
//

#ifndef JUSTMEDIAEDITOR_VIDEOBASEDECODER_H
#define JUSTMEDIAEDITOR_VIDEOBASEDECODER_H

#include <ImageDef.h>
#include <EGLVideoRender.h>
#include <FrameRenderSample.h>
#include <PartGraySample.h>
#include <DataFilterUtil.h>

typedef void (*VideoDecodeMessageCallback) (void *, int status, void *data);
enum VideoDecodeStatus {
	VIDEO_DECODE_STATUS_UNKNOWN,
	VIDEO_DECODE_STATUS_DECODING,
	VIDEO_DECODE_STATUS_RENDING,
	VIDEO_DECODE_STATUS_PAUSE,
	VIDEO_DECODE_STATUS_STOP
};
enum VideoDecodeMessage {
	VIDEO_DECODE_MESSAGE_READY,
	VIDEO_DECODE_MESSAGE_DURATION,
	VIDEO_DECODE_MESSAGE_DONE
};
class VideoBaseDecoder {
public:
	virtual void SetVideoRender(EGLVideoRender *videoRender);
	virtual void SetCoverImage(FrameImage *image, double duration);
	virtual void SetTextDisplayParams(std::deque<TextDisplayParam* > params);
	virtual void SetStickerDisplayOptions(std::deque<StickerDisplayOption> options);
	virtual void SetVideoRenderDoneCallback(void* ctx, OnRenderDoneCallback renderDoneCallback);
	virtual void SetVideoDecodeMessageCallback(void *ctx, VideoDecodeMessageCallback videoDecodeMessageCallback);

protected:
	void RenderInner(FrameImage *image, double currentDecoderTime);
	void InitVideoRender();
	void ChangeVideoStatus(VideoDecodeStatus videoDecodeStatus);

protected:
	EGLVideoRender *m_VideoRender = nullptr;
	FrameImage *m_CoverImage = nullptr;
	std::deque<TextDisplayParam *> m_TextDisplayParams;
	std::deque<StickerDisplayOption> m_StickerDisplayOptions;
	double m_CoverImageDuration = 0;
	void *m_VideoDecodeMessageContext = nullptr;
	VideoDecodeMessageCallback m_VideoDecodeMessageCallback;
	volatile int m_VideoDecoderStatus = VIDEO_DECODE_STATUS_UNKNOWN;
	mutex m_VideoDecodeStatusMutex;
};


#endif //JUSTMEDIAEDITOR_VIDEOBASEDECODER_H
