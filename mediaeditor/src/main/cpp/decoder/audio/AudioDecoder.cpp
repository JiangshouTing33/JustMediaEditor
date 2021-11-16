//
// Created by 江守亭 on 2021/9/26.
//

#include "AudioDecoder.h"
void AudioDecoder::OnDecoderReady() {
    double duration = (float )p_AVfmtCtx->duration / (m_SpeedFactor * AV_TIME_BASE);//us to ms
    if (m_DecodeMessageCallback && m_MessageContext) {
        m_DecodeMessageCallback(m_MessageContext, DECODE_MESSAGE_DURATION, &duration);
    }
}

void AudioDecoder::OnDecoderDone() {
    if (m_DecodeMessageCallback && m_MessageContext) {
        m_DecodeMessageCallback(m_MessageContext, DECODE_MESSAGE_DONE, nullptr);
    }
}

void AudioDecoder::OnFrameAvailable(AVFrame *avFrame) {
//    LOGCATD("AudioDecoder::OnFrameAvailable..avframe.fps = %ld", avFrame->pts);
    if (m_DecodeMessageCallback && m_MessageContext) {
        m_DecodeMessageCallback(m_MessageContext, DECODE_MESSAGE_FRAME, avFrame);
    }
}