//
// Created by 江守亭 on 2021/9/27.
//

#ifndef JUSTMEDIAEDITOR_VIDEOENCODER_H
#define JUSTMEDIAEDITOR_VIDEOENCODER_H

#define VIDEO_FRAME_QUEUE_MAX_SIZE 200

#include <BaseEncoder.h>
#include <ImageDef.h>
#include <queue>
extern "C" {
#include <libswscale/swscale.h>
}
struct VideoEncodeParam {
    int frameWidth;
    int frameHeight;
    long videoBitRate;
    int fps = 1;
    int pixFmt = AV_PIX_FMT_YUV420P;
};

using namespace std;
class VideoEncoder : public BaseEncoder {
public:
    VideoEncoder(AVFormatContext *avFormatContext, AVMediaType avMediaType, AVCodecID avCodecId)
    : BaseEncoder(avFormatContext, avMediaType, avCodecId) {
        m_NeedSync = false;
    }

    ~VideoEncoder() {}

    void SetVideoEncodeParam(VideoEncodeParam videoEncodeParam) {
        m_VideoEncodeParam = videoEncodeParam;
    }

    void OnVideoFrame2Encode(FrameImage *image);
private:
    virtual int InitFrameQueue();
    virtual void UpdateAVCodecContextParam();
    virtual int InitFrame();
    virtual void UnInitEncoder();
    virtual AVFrame * GetFrameFromQueue();
    virtual void ReleaseFrameRes();
private:
    queue<FrameImage *> m_VideoFrameQueue;
    VideoEncodeParam m_VideoEncodeParam;

    AVFrame *p_VideoFrame = nullptr;
    AVFrame *p_TmpFrame = nullptr;
    SwsContext *p_SwsCtx = nullptr;

    int m_LastSrcWidth = 0;
    int m_LastSrcHeight = 0;
    int m_LastSrcFormat = 0;
};


#endif //JUSTMEDIAEDITOR_VIDEOENCODER_H
