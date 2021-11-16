//
// Created by 江守亭 on 2021/9/27.
//

#include "BaseMuxer.h"

BaseMuxer::BaseMuxer(const char* url, int muxerType) {
    strcpy(m_Url, url);

    m_MuxerType = muxerType;
    if (muxerType == MUXER_TYPE_VIDEO_AUDIO) {
        m_VideoEncoderNeedInit = true;
        m_AudioEncoderNeedInit = true;
    } else if (muxerType == MUXER_TYPE_VIDEO_ONLY) {
        m_VideoEncoderNeedInit = true;
        m_AudioEncoderNeedInit = false;
    } else if (muxerType == MUXER_TYPE_AUDIO_ONLY) {
        m_VideoEncoderNeedInit = false;
        m_AudioEncoderNeedInit = true;
    } else {
        m_VideoEncoderNeedInit = false;
        m_AudioEncoderNeedInit = false;
    }

    avformat_alloc_output_context2(&p_AVFmtCtx, nullptr, nullptr, m_Url);

    if (p_AVFmtCtx == nullptr) {
        LOGCATE("BaseMuxer::BaseMuxer..avformat_alloc_output_context2 Failed");
        return;
    }

    if (p_AVFmtCtx->oformat->video_codec != AV_CODEC_ID_NONE && m_VideoEncoderNeedInit) {
        m_VideoEncoder = new VideoEncoder(p_AVFmtCtx, AVMEDIA_TYPE_VIDEO,
                                          p_AVFmtCtx->oformat->video_codec);
        m_VideoEncoder->SetEncodeMessageCallback(this, OnVideoEncodeMessageCallback);
    }

    if (p_AVFmtCtx->oformat->audio_codec != AV_CODEC_ID_NONE && m_AudioEncoderNeedInit) {
        m_AudioEncoder = new AudioEncoder(p_AVFmtCtx, AVMEDIA_TYPE_AUDIO,
                                          p_AVFmtCtx->oformat->audio_codec);
        m_AudioEncoder->SetEncodeMessageCallback(this, OnAudioEncodeMessageCallback);
    }

    SetEncodeTimeStampCallback();
}

BaseMuxer::~BaseMuxer() {
    if (p_AVFmtCtx) {
        avformat_free_context(p_AVFmtCtx);
        p_AVFmtCtx = nullptr;
    }
}

int BaseMuxer::Init() {
    int ret, result = 0;

    do {
        if (p_AVFmtCtx == nullptr) {
            result = -1;
            LOGCATE("BaseMuxer::Init..Error, avformat_alloc_output_context2 failed, url = %s ", m_Url);
            break;
        }

        if (m_VideoEncoder) {
            ret = m_VideoEncoder->Init();
            if (ret < 0) {
                result = -1;
                LOGCATE("BaseMuxer::Init..Error, m_VideoEncoder->Init() failed, url = %s ", m_Url);
                break;
            }
        }

        if (m_AudioEncoder) {
            ret = m_AudioEncoder->Init();
            if (ret < 0) {
                result = -1;
                LOGCATE("BaseMuxer::Init..Error, m_AudioEncoder->Init() failed, url = %s ", m_Url);
                break;
            }
        }

        av_dump_format(p_AVFmtCtx, 0, m_Url, 1);

        if (!(p_AVFmtCtx->oformat->flags & AVFMT_NOFILE)) {
            ret = avio_open(&p_AVFmtCtx->pb, m_Url, AVIO_FLAG_WRITE);
            if (ret < 0) {
                LOGCATE("BaseMuxer::Init..Error, avio_open failed, url = %s, ret = %s", m_Url, av_err2str(ret));
                result = -1;
                break;
            }
        }
        ret = avformat_write_header(p_AVFmtCtx, nullptr);
        if (ret < 0) {
            LOGCATE("BaseMuxer::Init..Error, avformat_write_header failed, url = %s, ret = %s", m_Url, av_err2str(ret));
            result = -1;
            break;
        }
    } while (false);

    return result;
}

void BaseMuxer::UnInit() {
    if (p_AVFmtCtx) {
        avformat_free_context(p_AVFmtCtx);
        p_AVFmtCtx = nullptr;
    }

    if (m_AudioEncoder) {
        m_AudioEncoder->UnInit();
        delete m_AudioEncoder;
        m_AudioEncoder = nullptr;
    }

    if (m_VideoEncoder) {
        m_VideoEncoder->UnInit();
        delete m_VideoEncoder;
        m_VideoEncoder = nullptr;
    }
}

void BaseMuxer::Start() {
    if (m_VideoEncoder) {
        m_VideoEncoder->Start();
    }

    if (m_AudioEncoder) {
        m_AudioEncoder->Start();
    }
}

void BaseMuxer::Stop() {
    if (m_VideoEncoder) {
        m_VideoEncoder->Stop();
    }

    if (m_AudioEncoder) {
        m_AudioEncoder->Stop();
    }
}

void BaseMuxer::Pause() {
    if (m_VideoEncoder) {
        m_VideoEncoder->Pause();
    }

    if (m_AudioEncoder) {
        m_AudioEncoder->Pause();
    }
}

void BaseMuxer::SetVideoEncodeParam(VideoEncodeParam videoEncodeParam) {
    if (m_VideoEncoder) {
        m_VideoEncoder->SetVideoEncodeParam(videoEncodeParam);
    }
}

void BaseMuxer::SetAudioEncodeParam(AudioEncodeParam audioEncodeParam) {
    if (m_AudioEncoder) {
        m_AudioEncoder->SetAudioEncodeParam(audioEncodeParam);
    }
}

void BaseMuxer::OnAudioFrame2Encode(AVFrame *frame) {
    if (m_AudioEncoder) {
        m_AudioEncoder->OnAudioFrame2Encode(frame);
    }
}

void BaseMuxer::OnVideoFrame2Encode(FrameImage *image) {
    if (m_VideoEncoder) {
        m_VideoEncoder->OnVideoFrame2Encode(image);
    }
}

void BaseMuxer::VideoFrameInputDone() {
    if (m_VideoEncoder) {
        m_VideoEncoder->InputDone();
    }
}

void BaseMuxer::AudioFrameInputDone() {
    if (m_AudioEncoder) {
        m_AudioEncoder->InputDone();
    }
}

void BaseMuxer::SetCoverImageDuration(double duration) {
    if (m_VideoEncoder) {
        m_VideoEncoder->SetCoverImageDuration(duration);
    }

    if (m_AudioEncoder) {
        m_AudioEncoder->SetCoverImageDuration(duration);
    }
}

void BaseMuxer::SetEncodeTimeStampCallback() {
    if (m_MuxerType == MUXER_TYPE_AUDIO_ONLY) {
        if (m_AudioEncoder) {
            m_AudioEncoder->SetEncodeTimeStampCallback(this, OnEncodeTimeStampCallback);
        }
    } else {
        if (m_VideoEncoder) {
            m_VideoEncoder->SetEncodeTimeStampCallback(this, OnEncodeTimeStampCallback);
        }
    }
}

int BaseMuxer::InitAudioFrameQueue(int sample_fmt, uint64_t channel_layout, int nb_samples, int sample_rate) {
    if (m_AudioEncoder) {
        SrcAudioFrameParam param = {
                sample_fmt,
                channel_layout,
                nb_samples,
                sample_rate
        };

        return m_AudioEncoder->InitAudioFifoQueue(param);
    }
    return -1;
}

void BaseMuxer::OnEncodeTimeStampCallback(void *ctx, double timeStamp) {
    BaseMuxer *baseMuxer = static_cast<BaseMuxer *>(ctx);
    if (baseMuxer->m_MuxerMessageCallback && baseMuxer->m_MessageContext) {
        baseMuxer->m_MuxerMessageCallback(baseMuxer->m_MessageContext, MUXER_MESSAGE_TIME_STAMP, &timeStamp);
    }
}

void BaseMuxer::OnVideoEncodeMessageCallback(void *ctx, int message, void *data) {
    BaseMuxer *baseMuxer = static_cast<BaseMuxer *>(ctx);
    if (message == ENCODE_MESSAGE_DONE) {
        baseMuxer->m_VideoEncodeDone = true;
        if (baseMuxer->m_AudioEncoder) {
            baseMuxer->m_AudioEncoder->Stop();
        }
        baseMuxer->WriteTrailer();
    } else if (message == ENCODE_MESSAGE_WAIT_SYNC) {
        if (baseMuxer->m_AudioEncoder == nullptr
        || baseMuxer->m_VideoEncoder->GetNextPresentationTimeStamp() <= baseMuxer->m_AudioEncoder->GetNextPresentationTimeStamp()) {
            baseMuxer->m_VideoEncoder->Encoding();
        }
    } else if (message == ENCODE_MESSAGE_VIDEO_QUEUE_FULL) {
        if (baseMuxer->m_MuxerMessageCallback && baseMuxer->m_MessageContext) {
            baseMuxer->m_MuxerMessageCallback(baseMuxer->m_MessageContext, MUXER_MESSAGE_VIDEO_QUEUE_FULL, nullptr);
        }
    }
}

void BaseMuxer::OnAudioEncodeMessageCallback(void *ctx, int message, void *data) {
    BaseMuxer *baseMuxer = static_cast<BaseMuxer *>(ctx);
    if (message == ENCODE_MESSAGE_DONE) {
        baseMuxer->m_AudioEncodeDone = true;
        if (baseMuxer->m_VideoEncoder) {
            baseMuxer->m_VideoEncoder->Stop();
        }
        baseMuxer->WriteTrailer();
    } else if (message == ENCODE_MESSAGE_WAIT_SYNC) {
//        LOGCATD("BaseMuxer::OnAudioEncodeMessageCallback..videoEncodeTime = %lf, audioEncodeTime = %lf",
//                baseMuxer->m_AudioEncoder->GetNextPresentationTimeStamp(), baseMuxer->m_VideoEncoder->GetNextPresentationTimeStamp());
        if (baseMuxer->m_VideoEncoder == nullptr
            || baseMuxer->m_AudioEncoder->GetNextPresentationTimeStamp() <= baseMuxer->m_VideoEncoder->GetNextPresentationTimeStamp()) {
            baseMuxer->m_AudioEncoder->Encoding();
        }
    }
}

void BaseMuxer::WriteTrailer() {
    if ((!m_VideoEncoderNeedInit || m_VideoEncodeDone) && (!m_AudioEncoderNeedInit || m_AudioEncodeDone) && p_AVFmtCtx) {
        int ret = av_write_trailer(p_AVFmtCtx);
        if (ret != 0) {
            LOGCATE("BaseMuxer::WriteTrailer()..av_write_trailer Error, result=%s ", av_err2str(ret));
            return;
        }
        if (m_MuxerMessageCallback && m_MessageContext) {
            m_MuxerMessageCallback(m_MessageContext, MUXER_MESSAGE_DONE, nullptr);
        }
    }
}