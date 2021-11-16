//
// Created by 江守亭 on 2021/9/27.
//

#ifndef JUSTMEDIAEDITOR_BASEMUXER_H
#define JUSTMEDIAEDITOR_BASEMUXER_H

#include <VideoEncoder.h>
#include <AudioEncoder.h>

enum MuxerType {
    MUXER_TYPE_NONE = -1,
    MUXER_TYPE_VIDEO_ONLY,
    MUXER_TYPE_AUDIO_ONLY,
    MUXER_TYPE_VIDEO_AUDIO
};

enum MuxerMessageType {
    MUXER_MESSAGE_TIME_STAMP,
    MUXER_MESSAGE_VIDEO_QUEUE_FULL,
    MUXER_MESSAGE_DONE
};

typedef void (*MuxerMessageCallback) (void *, int, void *);

class BaseMuxer {
public:
    BaseMuxer(const char* url, int muxerType);

    ~BaseMuxer();

    int Init();

    void UnInit();

    void Start();

    void Stop();

    void Pause();

    void SetVideoEncodeParam(VideoEncodeParam videoEncodeParam);

    void SetAudioEncodeParam(AudioEncodeParam audioEncodeParam);

    void OnAudioFrame2Encode(AVFrame *frame);

    void OnVideoFrame2Encode(FrameImage *image);

    int InitAudioFrameQueue(int sample_fmt, uint64_t channel_layout, int nb_samples, int sample_rate);

    void VideoFrameInputDone();

    void AudioFrameInputDone();

    void SetMuxerMessageCallback(void *ctx, MuxerMessageCallback muxerMessageCallback) {
        m_MessageContext = ctx;
        m_MuxerMessageCallback = muxerMessageCallback;
    }

    void SetCoverImageDuration(double duration);
private:
    void WriteTrailer();
    void SetEncodeTimeStampCallback();

    static void OnEncodeTimeStampCallback(void *ctx, double timeStamp);
    static void OnVideoEncodeMessageCallback(void *ctx, int message, void *data);
    static void OnAudioEncodeMessageCallback(void *ctx, int message, void *data);

private:
    VideoEncoder *m_VideoEncoder = nullptr;
    AudioEncoder *m_AudioEncoder = nullptr;

    AVFormatContext *p_AVFmtCtx = nullptr;

    char m_Url[4096];

    volatile bool m_VideoEncodeDone = false;
    volatile bool m_AudioEncodeDone = false;

    bool m_VideoEncoderNeedInit = false;
    bool m_AudioEncoderNeedInit = false;

    int m_MuxerType = MUXER_TYPE_NONE;

    void *m_MessageContext = nullptr;
    MuxerMessageCallback m_MuxerMessageCallback = nullptr;
};


#endif //JUSTMEDIAEDITOR_BASEMUXER_H
