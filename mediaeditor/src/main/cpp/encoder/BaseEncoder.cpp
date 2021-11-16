//
// Created by 江守亭 on 2021/9/27.
//

#include <unistd.h>
#include "BaseEncoder.h"

BaseEncoder::BaseEncoder(AVFormatContext *avFormatContext, AVMediaType avMediaType, AVCodecID avCodecId) {
    p_AVFmtCtx = avFormatContext;
    m_AVMediaType = avMediaType;
    m_AVCodecID = avCodecId;
}

void BaseEncoder::Start() {
    if (p_Thread == nullptr) {
        p_Thread = new thread(DoAVEncoding, this);
    }
}

void BaseEncoder::Encoding() {
    if (m_EncoderStatus != ENCODE_STATUS_STOP) {
        ChangeStatus(ENCODE_STATUS_ENCODING);
        m_Cond.notify_all();
    }
}

void BaseEncoder::Pause() {
    ChangeStatus(ENCODE_STATUS_PAUSE);
}

void BaseEncoder::Stop() {
    if (m_EncoderStatus == ENCODE_STATUS_STOP) return;
    ChangeStatus(ENCODE_STATUS_STOP);
    m_Cond.notify_all();
}

void BaseEncoder::RequestSync() {
    if (m_EncodeMessageCallback && m_MessageContext) {
        m_EncodeMessageCallback(m_MessageContext, ENCODE_MESSAGE_WAIT_SYNC, nullptr);
    }
}

void BaseEncoder::SetCoverImageDuration(double duration) {
    m_CoverImageDuration = duration;
}

int BaseEncoder::Init() {
    m_InitFFEncoderResult = InitFFEncoder();
    return m_InitFFEncoderResult;
}

void BaseEncoder::UnInit() {
    if (p_Thread) {
        Stop();
        p_Thread->join();
        delete p_Thread;
        p_Thread = nullptr;
    }
}

void BaseEncoder::OnEncoderDone() {
    if (m_EncodeMessageCallback && m_MessageContext) {
        m_EncodeMessageCallback(m_MessageContext, ENCODE_MESSAGE_DONE, nullptr);
    }
}

int BaseEncoder::InitFFEncoder() {
    int result = 0;
    const AVCodec *avCodec = avcodec_find_encoder(m_AVCodecID);
    if (avCodec == nullptr) {
        result = -1;
        LOGCATE("BaseEncoder::Init Error, avcodec_find_encoder failed, avcodeC = %s", avcodec_get_name(m_AVCodecID));
        return result;
    }
    p_AVStream = avformat_new_stream(p_AVFmtCtx, avCodec);
    if (p_AVStream == nullptr) {
        result = -1;
        LOGCATE("BaseEncoder::Init Error, avformat_new_stream failed, avcodeC = %s", avcodec_get_name(m_AVCodecID));
        return result;
    }

    p_AVCdcCtx = avcodec_alloc_context3(avCodec);
    if (p_AVCdcCtx == nullptr) {
        result = -1;
        LOGCATE("BaseEncoder::Init Error, avcodec_alloc_context3 failed, avcodeC = %s", avcodec_get_name(m_AVCodecID));
        return result;
    }

    p_AVCdcCtx->codec_id = m_AVCodecID;
    p_AVCdcCtx->codec_type = m_AVMediaType;
    UpdateAVCodecContextParam();

    if (p_AVFmtCtx->oformat->flags & AVFMT_GLOBALHEADER) {
        p_AVCdcCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    result = avcodec_open2(p_AVCdcCtx, avCodec, nullptr);
    if (result != 0) {
        LOGCATE("BaseEncoder::Init Error, avcodec_open2 failed, avcodeC = %s, ret = %d", avcodec_get_name(m_AVCodecID), result);
        return result;
    }

    result = avcodec_parameters_from_context(p_AVStream->codecpar, p_AVCdcCtx);
    if (result < 0) {
        LOGCATE("BaseEncoder::Init Error, avcodec_parameters_to_context failed, avcodeC = %s, ret = %d", avcodec_get_name(m_AVCodecID), result);
        return result;
    }

    result = InitFrame();
    if (result < 0) {
        LOGCATE("BaseEncoder::Init Error, InitFrame failed, avcodeC = %s, ret = %d", avcodec_get_name(m_AVCodecID), result);
        return result;
    }

    return 0;
}

void BaseEncoder::UnInitFFEncoder() {
    if (p_AVCdcCtx) {
        avcodec_free_context(&p_AVCdcCtx);
        p_AVCdcCtx = nullptr;
    }
    UnInitEncoder();
}

void BaseEncoder::DoAVEncoding(BaseEncoder *baseEncoder) {
    do {
        if (baseEncoder->InitFrameQueue() != 0) {
            break;
        }

        if (baseEncoder->m_InitFFEncoderResult != 0) {
            break;
        }

        baseEncoder->EncodingLoop();
    } while (false);
    baseEncoder->UnInitFFEncoder();
    baseEncoder->OnEncoderDone();
}

void BaseEncoder::EncodingLoop() {
    Encoding();
    while (true) {
        while (m_EncoderStatus == ENCODE_STATUS_PAUSE) {
            std::unique_lock<std::mutex> lock(m_CondMutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(50));
        }

        while (m_EncoderStatus == ENCODE_STATUS_WAIT_SYNC) {
            std::unique_lock<std::mutex> lock(m_CondMutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(1));
            RequestSync();
        }

        if (m_EncoderStatus == ENCODE_STATUS_STOP) {
            break;
        }

        if (EncodeOneFrame() != 0) {
            Stop();
        } else if (m_NeedSync) {
            WaitSync();
        }
    }
}

void BaseEncoder::WaitSync() {
    if (m_EncoderStatus != ENCODE_STATUS_STOP) {
        ChangeStatus(ENCODE_STATUS_WAIT_SYNC);
    }
}

int BaseEncoder::EncodeOneFrame() {
    int result = 0, ret = 0;

    AVPacket pPacket = {0};
    av_init_packet(&pPacket);

    do {
        AVFrame *avFrame;

        avFrame = GetFrameFromQueue();
        ret = avcodec_send_frame(p_AVCdcCtx, avFrame);
        if (ret == AVERROR(EAGAIN)) {
            result = 0;
            break;
        } else if (ret == AVERROR_EOF) {
            result = -1;
            break;
        } else if (ret < 0) {
            LOGCATE("BaseEncoder::EncodeOneFrame() avcodec_send_frame fail. avcodeC = %s, ret=%s", avcodec_get_name(m_AVCodecID), av_err2str(ret));
            result = -1;
            break;
        }
        ReleaseFrameRes();
        while (ret >= 0) {
            ret = avcodec_receive_packet(p_AVCdcCtx, &pPacket);
            if (ret == AVERROR_EOF) {
                result = -1;
                break;
            } else if (ret == AVERROR(EAGAIN)){
                result = 0;
                break;
            } else if (ret < 0) {
                result = -1;
                LOGCATE("BaseEncoder::EncodeOneFrame() avcodec_receive_packet fail. avcodeC = %s, ret=%s", avcodec_get_name(m_AVCodecID), av_err2str(ret));
                break;
            }
//            LOGCATD("BaseEncoder::EncodeOneFrame() pkt pts=%ld， mediaType = %s", pPacket.pts, av_get_media_type_string(m_AVMediaType));
            av_packet_rescale_ts(&pPacket, p_AVCdcCtx->time_base, p_AVStream->time_base);
            m_NextTimeStamp = pPacket.pts * av_q2d(p_AVStream->time_base);
            pPacket.stream_index = p_AVStream->index;

            ret = av_interleaved_write_frame(p_AVFmtCtx, &pPacket);
            if (ret < 0) {
                result = -1;
                LOGCATE("BaseEncoder::EncodeOneFrame() av_interleaved_write_frame fail. avcodeC = %s, ret=%s", avcodec_get_name(m_AVCodecID), av_err2str(ret));
                break;
            }
            if (m_EncodeTimeStampCallback && m_TimeStampContext) {
                m_EncodeTimeStampCallback(m_TimeStampContext, m_NextTimeStamp);
            }
        }
    } while (false);

    av_packet_unref(&pPacket);

    return result;
}

void BaseEncoder::ReleaseFrameRes() {}

void BaseEncoder::ChangeStatus(EncodeStatus encodeStatus) {
    std::unique_lock<mutex> lock(m_Mutex);
    m_EncoderStatus = encodeStatus;
    lock.unlock();
}