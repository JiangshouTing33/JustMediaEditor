//
// Created by 江守亭 on 2021/9/26.
//

#ifndef JUSTMEDIAEDITOR_VIDEODECODER_H
#define JUSTMEDIAEDITOR_VIDEODECODER_H

#include <BaseDecoder.h>
#include <VideoBaseDecoder.h>

class VideoDecoder : public BaseDecoder, public VideoBaseDecoder {
public:
    VideoDecoder(const char *url);
    ~VideoDecoder();

private:
    virtual void OnDecoderReady();
    virtual void OnDecoderDone();
    virtual void OnFrameAvailable(AVFrame *avFrame);
	void VideoRenderCoverImage();

private:
	int m_RenderCoverImageIndex = 0;
};


#endif //JUSTMEDIAEDITOR_VIDEODECODER_H
