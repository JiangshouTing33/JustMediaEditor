//
// Created by 江守亭 on 2021/9/25.
//

#include <unistd.h>
#include "BaseDecoder.h"
void BaseDecoder::Start() {
    if (p_Thread == nullptr) {
        StartDecodingThread();
    } else {
        ChangeStatus(DECODE_STATUS_DECODING);
        m_Cond.notify_all();
    }
}

void BaseDecoder::Stop() {
    ChangeStatus(DECODE_STATUS_STOP);
    m_Cond.notify_all();
}

void BaseDecoder::Decoding() {
    ChangeStatus(DECODE_STATUS_DECODING);
    m_Cond.notify_all();
}

void BaseDecoder::Pause() {
    ChangeStatus(DECODE_STATUS_PAUSE);
}

void BaseDecoder::Rending() {
    ChangeStatus(DECODE_STATUS_RENDING);
}

void BaseDecoder::Init(const char *url, AVMediaType mediaType) {
    strcpy(m_Url, url);
    m_MediaType = mediaType;
}

void BaseDecoder::UnInit() {
    if (p_Thread) {
        Stop();
        p_Thread->join();
        delete p_Thread;
        p_Thread = nullptr;
    }
}

void BaseDecoder::SetSpeedFactor(float speedFactor) {
	if (speedFactor <= 0) {
		speedFactor = 1.f;
	}
	m_SpeedFactor = speedFactor;
}

void BaseDecoder::StartDecodingThread() {
    p_Thread = new thread(DoAVDecoding, this);
}

int BaseDecoder::InitFFDecoder() {
    int result = 0;
    do {
        ChangeStatus(DECODE_STATUS_INIT);
        p_AVfmtCtx = avformat_alloc_context();
        if (p_AVfmtCtx == nullptr) {
            result = -1;
            LOGCATE("BaseDecoder::InitFFDecoder Error, avformat_alloc_context Failed");
            break;
        }

        result = avformat_open_input(&p_AVfmtCtx, m_Url, nullptr, nullptr);
        if (result != 0) {
            LOGCATE("BaseDecoder::InitFFDecoder Error, avformat_open_input Failed, ret = %s, m_Url = %s", av_err2str(result), m_Url);
            break;
        }

        for (int i = 0; i < p_AVfmtCtx->nb_streams; ++i) {
            if (p_AVfmtCtx->streams[i]->codecpar->codec_type == m_MediaType) {
                m_StreamIndex = i;
            }
        }
        if (m_StreamIndex == -1) {
            result = -1;
            LOGCATE("BaseDecoder::InitFFDecoder Error, cant find stream Index: mediaType = %s", av_get_media_type_string(m_MediaType));
            break;
        }
        p_AVStream = p_AVfmtCtx->streams[m_StreamIndex];
        AVCodecParameters *codecParameters = p_AVStream->codecpar;

        p_AvCodec = avcodec_find_decoder(codecParameters->codec_id);
        if (p_AvCodec == nullptr) {
            result = -1;
            LOGCATE("BaseDecoder::InitFFDecoder Error, avcodec_find_decoder failed, avCodec = %s", avcodec_get_name(codecParameters->codec_id));
            break;
        }

        p_AVCdcCtx = avcodec_alloc_context3(p_AvCodec);
        if (p_AVCdcCtx == nullptr) {
            result = -1;
            LOGCATE("BaseDecoder::InitFFDecoder Error, avcodec_alloc_context3 failed, avCodec = %s", avcodec_get_name(codecParameters->codec_id));
            break;
        }

        result = avcodec_parameters_to_context(p_AVCdcCtx, codecParameters);
        if (result != 0) {
            LOGCATE("BaseDecoder::InitFFDecoder Error, avcodec_parameters_to_context failed, ret = %s", av_err2str(result));
            break;
        }

        AVDictionary *avDictionary = nullptr;
        av_dict_set(&avDictionary, "stimeout", "20000000", 0);
        av_dict_set(&avDictionary, "max_delay", "30000000", 0);
        av_dict_set(&avDictionary, "rtsp_transport", "tcp", 0);

        result = avcodec_open2(p_AVCdcCtx, p_AvCodec, &avDictionary);
        if (result != 0) {
            LOGCATE("BaseDecoder::InitFFDecoder Error, avcodec_open2 failed, ret = %s", av_err2str(result));
            break;
        }

        p_AVFrame = av_frame_alloc();
        if (p_AVFrame == nullptr) {
            result = -1;
            LOGCATE("BaseDecoder::InitFFDecoder Error, av_frame_alloc failed");
            break;
        }

        p_AVPacket = av_packet_alloc();
        if (p_AVPacket == nullptr) {
            result = -1;
            LOGCATE("BaseDecoder::InitFFDecoder Error, av_packet_alloc failed");
            break;
        }

    } while (false);

    return result;
}

void BaseDecoder::UnInitFFDecoder() {
    if (p_AVFrame) {
        av_frame_free(&p_AVFrame);
        p_AVFrame = nullptr;
    }

    if (p_AVPacket) {
        av_packet_free(&p_AVPacket);
        p_AVPacket = nullptr;
    }

    if (p_AVCdcCtx) {
        avcodec_free_context(&p_AVCdcCtx);
        p_AVCdcCtx = nullptr;
    }

    if (p_AVfmtCtx) {
        avformat_free_context(p_AVfmtCtx);
        p_AVfmtCtx = nullptr;
    }
}

void BaseDecoder::DecodingLoop() {
    while (true) {
        while (m_DecoderStatus == DECODE_STATUS_PAUSE) {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(50));
        }

        while (m_DecoderStatus == DECODE_STATUS_RENDING) {
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_Cond.wait_for(lock, std::chrono::milliseconds(1));
        }

        if (m_DecoderStatus == DECODE_STATUS_STOP) {
            break;
        }

        if (DecodeOnePacket() != 0) {
            ChangeStatus(DECODE_STATUS_STOP);
        }
    }
}

int BaseDecoder::DecodeOnePacket() {
    int ret, result = 0;
    do {
        ret = av_read_frame(p_AVfmtCtx, p_AVPacket);
        if (ret == AVERROR_EOF) {
            av_init_packet(p_AVPacket);
            p_AVPacket->data = NULL;
            p_AVPacket->size = 0;

            ret = avcodec_send_packet(p_AVCdcCtx, p_AVPacket);
            if (ret == AVERROR_EOF) {
                result = -1;
                break;
            } else if (ret < 0) {
                result = 0;
                LOGCATE("BaseDecoder::DecodeOnePacket..avcodec_send_packet failed, ret = %s", av_err2str(ret));
                break;
            }

            while (avcodec_receive_frame(p_AVCdcCtx, p_AVFrame) == 0) {
                OnFrameAvailable(p_AVFrame);
            }
        } else if (ret < 0) {
            result = -1;
            LOGCATE("BaseDecoder::DecodeOnePacket..av_read_frame failed, ret = %s", av_err2str(ret));
            break;
        }

        if (p_AVPacket->stream_index == m_StreamIndex) {
            ret = avcodec_send_packet(p_AVCdcCtx, p_AVPacket);
            if (ret == AVERROR_EOF) {
                result = -1;
                break;
            } else if (ret < 0) {
                result = 0;
                LOGCATE("BaseDecoder::DecodeOnePacket..avcodec_send_packet failed, ret = %s", av_err2str(ret));
                break;
            }

            if (m_DecoderStatus == DECODE_STATUS_INIT) {
                ReadyAndWaiting();
            }

            while (avcodec_receive_frame(p_AVCdcCtx, p_AVFrame) == 0) {
                OnFrameAvailable(p_AVFrame);
            }
        }
    } while (false);

    av_packet_unref(p_AVPacket);
    return result;
}

void BaseDecoder::ChangeStatus(DecodeStatus decodeStatus) {
    unique_lock<mutex> lock(m_Mutex);
    m_DecoderStatus = decodeStatus;
    lock.unlock();
}

void BaseDecoder::ReadyAndWaiting() {
    ChangeStatus(DECODE_STATUS_READY);
    if (m_DecodeMessageCallback && m_MessageContext) {
        m_DecodeMessageCallback(m_MessageContext, DECODE_MESSAGE_READY, nullptr);
    }
    OnDecoderReady();
    while (m_DecoderStatus == DECODE_STATUS_READY) {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(1));
    }
}

void BaseDecoder::OnRenderDone(void *ctx) {
    BaseDecoder *baseDecoder = static_cast<BaseDecoder *>(ctx);
    if (baseDecoder->m_DecoderStatus == DECODE_STATUS_RENDING) {
        baseDecoder->ChangeStatus(DECODE_STATUS_DECODING);
    }
}

void BaseDecoder::DoAVDecoding(BaseDecoder *baseDecoder) {
    do {
        if (baseDecoder->InitFFDecoder() != 0) {
            break;
        }
        baseDecoder->DecodingLoop();
    } while (false);
    baseDecoder->UnInitFFDecoder();
    baseDecoder->OnDecoderDone();
}
