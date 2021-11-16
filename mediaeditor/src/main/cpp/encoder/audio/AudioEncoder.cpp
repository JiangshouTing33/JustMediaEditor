//
// Created by 江守亭 on 2021/9/27.
//

#include <unistd.h>
#include "AudioEncoder.h"
int AudioEncoder::InitAudioFifoQueue(SrcAudioFrameParam srcAudioFrameParam) {
    if (p_AudioQueue == nullptr) {
        p_AudioQueue = av_audio_fifo_alloc(AVSampleFormat(srcAudioFrameParam.sample_fmt),
                av_get_channel_layout_nb_channels(srcAudioFrameParam.channel_layout), srcAudioFrameParam.nb_samples);

        if (p_AudioQueue == nullptr) {
            m_AudioQueueInitResult = -1;
            LOGCATE("AudioEncoder::InitAudioFifoQueue Error, av_audio_fifo_alloc failed");
            return m_AudioQueueInitResult;
        }

        m_SrcAudioFrameParam = srcAudioFrameParam;
    }

    m_AudioQueueInitResult = 0;
    return m_AudioQueueInitResult;
}

void AudioEncoder::OnAudioFrame2Encode(AVFrame *frame) {
    if (p_AudioQueue && frame) {
//        LOGCATD("AudioEncoder::OnAudioFrame2Encode..frame.nbSamples = %d", frame->nb_samples);
        unique_lock<mutex> lock(m_FrameWriteMutex);
        av_audio_fifo_write(p_AudioQueue, reinterpret_cast<void **>(frame->data), frame->nb_samples);
    }
}

int AudioEncoder::InitFrameQueue() {
    return m_AudioQueueInitResult;
}

void AudioEncoder::UpdateAVCodecContextParam() {
    p_AVCdcCtx->sample_fmt = AVSampleFormat(m_AudioEncodeParam.sampleFormat);
    p_AVCdcCtx->sample_rate = m_AudioEncodeParam.audioSampleRate;
    p_AVCdcCtx->channel_layout = m_AudioEncodeParam.channelLayout;
    p_AVCdcCtx->channels = av_get_channel_layout_nb_channels(p_AVCdcCtx->channel_layout);
    p_AVCdcCtx->bit_rate = m_AudioEncodeParam.bitRate;
    p_AVCdcCtx->time_base = {1, p_AVCdcCtx->sample_rate == 0 ? 1 : p_AVCdcCtx->sample_rate};
}

int AudioEncoder::InitFrame() {
    p_AudioFrame = av_frame_alloc();
    if (p_AudioFrame == nullptr) {
        LOGCATE("AudioEncoder::InitFrame(p_AudioFrame)..error, av_frame_alloc failed");
        return -1;
    }

    p_TmpFrame = av_frame_alloc();
    if (p_TmpFrame == nullptr) {
        LOGCATE("AudioEncoder::InitFrame(p_TmpFrame)..error, av_frame_alloc failed");
        return -1;
    }

    p_AudioFrame->nb_samples = p_AVCdcCtx->frame_size;
    p_AudioFrame->format = p_AVCdcCtx->sample_fmt;
    p_AudioFrame->channel_layout = p_AVCdcCtx->channel_layout;
    p_AudioFrame->sample_rate = p_AVCdcCtx->sample_rate;

    int ret = av_frame_get_buffer(p_AudioFrame, 1);
    if (ret < 0) {
        LOGCATE("AudioEncoder::InitFrame..error, av_frame_get_buffer failed, ret = %s", av_err2str(ret));
        return -1;
    }

    return 0;
}

void AudioEncoder::UnInitEncoder() {
    if (p_AudioQueue) {
        av_audio_fifo_free(p_AudioQueue);
        p_AudioQueue = nullptr;
    }

    if (p_AudioFrame) {
        av_frame_free(&p_AudioFrame);
        p_AudioFrame = nullptr;
    }

    if (p_TmpFrame) {
        av_frame_free(&p_TmpFrame);
        p_TmpFrame = nullptr;
    }

    if (p_SwrContext) {
        swr_free(&p_SwrContext);
        p_SwrContext = nullptr;
    }
}

AVFrame * AudioEncoder::GetFrameFromQueue() {
    int ret = 0;
    AVFrame *avFrame;
    while (av_audio_fifo_size(p_AudioQueue) < m_SrcAudioFrameParam.nb_samples && !m_InputExit) {
        std::unique_lock<std::mutex> lock(m_CondMutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(5));
    }

    if (av_audio_fifo_size(p_AudioQueue) < m_SrcAudioFrameParam.nb_samples && m_InputExit) {
        return nullptr;
    }

    p_TmpFrame->format = m_SrcAudioFrameParam.sample_fmt;
    p_TmpFrame->nb_samples = m_SrcAudioFrameParam.nb_samples;
    p_TmpFrame->sample_rate = m_SrcAudioFrameParam.sample_rate;
    p_TmpFrame->channel_layout = m_SrcAudioFrameParam.channel_layout;

    do {
        ret = av_frame_get_buffer(p_TmpFrame, 1);
        if (ret < 0) {
            LOGCATE("AudioEncoder::GetFrameFromQueue..error, av_frame_get_buffer failed, ret = %s", av_err2str(ret));
            break;
        }

        unique_lock<mutex> lock(m_FrameReadMutex);
        ret = av_audio_fifo_read(p_AudioQueue, reinterpret_cast<void **>(p_TmpFrame->data), p_TmpFrame->nb_samples);
        if (ret < 0) {
            LOGCATE("AudioEncoder::GetFrameFromQueue..error, av_audio_fifo_read failed, ret = %d", ret);
            break;
        }
        lock.unlock();
        if (p_TmpFrame->format != p_AVCdcCtx->sample_fmt) {
            ret = av_frame_make_writable(p_AudioFrame);
            if (ret < 0) {
                LOGCATE("AudioEncoder::GetFrameFromQueue Error while av_frame_make_writable");
                break;
            }
            if (p_SwrContext == nullptr) {
                p_SwrContext = swr_alloc();

                if (p_SwrContext == nullptr) {
                    LOGCATE("AudioEncoder::GetFrameFromQueue..error, swr_alloc failed");
                    break;
                }

                av_opt_set_channel_layout(p_SwrContext, "in_channel_layout", m_SrcAudioFrameParam.channel_layout, 0);
                av_opt_set_channel_layout(p_SwrContext, "out_channel_layout", p_AVCdcCtx->channel_layout, 0);
                av_opt_set_int(p_SwrContext, "in_sample_rate", m_SrcAudioFrameParam.sample_rate, 0);
                av_opt_set_int(p_SwrContext, "out_sample_rate", p_AVCdcCtx->sample_rate, 0);
                av_opt_set_sample_fmt(p_SwrContext, "in_sample_fmt", AVSampleFormat(m_SrcAudioFrameParam.sample_fmt), 0);
                av_opt_set_sample_fmt(p_SwrContext, "out_sample_fmt", p_AVCdcCtx->sample_fmt, 0);

                ret = swr_init(p_SwrContext);
                if (ret < 0) {
                    LOGCATE("AudioEncoder::GetFrameFromQueue..error, swr_init failed, ret = %s", av_err2str(ret));
                    break;
                }
            }

            ret = swr_convert(p_SwrContext, p_AudioFrame->data, p_AudioFrame->nb_samples,
                              (const uint8_t **)(p_TmpFrame->data), m_SrcAudioFrameParam.nb_samples);

            if (ret < 0) {
                LOGCATE("AudioEncoder::GetFrameFromQueue..error, swr_convert failed, ret = %d", ret);
                break;
            }
//            LOGCATD("AudioEncoder::GetFrameFromQueue...swr_convert..ret = %d", ret);
            avFrame = p_AudioFrame;
        } else {
            avFrame = p_TmpFrame;
        }
        if (m_CoverImageDuration != 0) {
            m_NextPts += m_CoverImageDuration / (av_q2d(p_AVCdcCtx->time_base) == 0 ? 1 : av_q2d(p_AVCdcCtx->time_base));
            m_CoverImageDuration = 0;
        }
        avFrame->pts = m_NextPts;
        m_NextPts += avFrame->nb_samples;
    } while (false);

    return avFrame;
}
