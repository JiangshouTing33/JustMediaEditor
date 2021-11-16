//
// Created by 江守亭 on 2021/9/27.
//

#ifndef JUSTMEDIAEDITOR_AUDIOENCODER_H
#define JUSTMEDIAEDITOR_AUDIOENCODER_H

#include <BaseEncoder.h>
extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
#include <libavutil/opt.h>
}
struct AudioEncodeParam {
    int audioSampleRate;
    uint64_t channelLayout;
    int64_t bitRate = 96000;
    int sampleFormat = AV_SAMPLE_FMT_FLTP;
};

struct SrcAudioFrameParam {
    int sample_fmt;
    uint64_t channel_layout;
    int nb_samples;
    int sample_rate;
};

class AudioEncoder : public BaseEncoder {
public:
    AudioEncoder(AVFormatContext *avFormatContext, AVMediaType avMediaType, AVCodecID avCodecId)
    : BaseEncoder(avFormatContext, avMediaType, avCodecId) {
        m_NeedSync = true;
    }
    ~AudioEncoder() {}

    void SetAudioEncodeParam(AudioEncodeParam audioEncodeParam) {
        m_AudioEncodeParam = audioEncodeParam;
    }
    int InitAudioFifoQueue(SrcAudioFrameParam srcAudioFrameParam);
    void OnAudioFrame2Encode(AVFrame *frame);
private:
    virtual int InitFrameQueue();
    virtual void UpdateAVCodecContextParam();
    virtual int InitFrame();
    virtual void UnInitEncoder();
    virtual AVFrame * GetFrameFromQueue();
private:
    AudioEncodeParam m_AudioEncodeParam;
    AVAudioFifo *p_AudioQueue = nullptr;
    AVFrame *p_AudioFrame = nullptr;
    AVFrame *p_TmpFrame = nullptr;
    SwrContext *p_SwrContext = nullptr;

    mutex m_FrameWriteMutex;
    mutex m_FrameReadMutex;

    SrcAudioFrameParam m_SrcAudioFrameParam;
    int m_AudioQueueInitResult = -1;
};


#endif //JUSTMEDIAEDITOR_AUDIOENCODER_H
