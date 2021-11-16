//
// Created by 江守亭 on 2021/9/26.
//

#include "VideoDecoder.h"
VideoDecoder::VideoDecoder(const char *url) {
    Init(url, AVMEDIA_TYPE_VIDEO);
}

VideoDecoder::~VideoDecoder() {
    UnInit();
}

void VideoDecoder::OnDecoderReady() {
    SetVideoRenderDoneCallback(this, OnRenderDone);
    InitVideoRender();

    double duration = (float )p_AVfmtCtx->duration / (m_SpeedFactor * AV_TIME_BASE);//us to ms
    if (m_CoverImage && m_CoverImage->ppPlane[0] && m_CoverImageDuration > 0 && m_VideoRender) {
        VideoRenderCoverImage();
        duration += m_CoverImageDuration;
    }

    if (m_DecodeMessageCallback && m_MessageContext) {
        m_DecodeMessageCallback(m_MessageContext, DECODE_MESSAGE_DURATION, &duration);
    }
}

void VideoDecoder::OnDecoderDone() {
    if (m_DecodeMessageCallback && m_MessageContext) {
        m_DecodeMessageCallback(m_MessageContext, DECODE_MESSAGE_DONE, nullptr);
    }
}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {
    if (frame && m_VideoRender) {
        FrameImage image;
        image.width = frame->width;
        image.height = frame->height;
        switch (frame->format) {
            case AV_PIX_FMT_YUVJ420P:
            case AV_PIX_FMT_YUV420P:
                image.format = IMAGE_FORMAT_I420;
                image.pLineSize[0] = frame->linesize[0];
                image.pLineSize[1] = frame->linesize[1];
                image.pLineSize[2] = frame->linesize[2];
                image.ppPlane[0] = frame->data[0];
                image.ppPlane[1] = frame->data[1];
                image.ppPlane[2] = frame->data[2];
                if(frame->data[0] && frame->data[1] && !frame->data[2] && frame->linesize[0] == frame->linesize[1] && frame->linesize[2] == 0) {
                    // on some android device, output of h264 mediacodec decoder is NV12 兼容某些设备可能出现的格式不匹配问题
                    image.format = IMAGE_FORMAT_NV12;
                }
                break;
            case AV_PIX_FMT_NV12:
                image.format = IMAGE_FORMAT_NV12;
                image.pLineSize[0] = frame->linesize[0];
                image.pLineSize[1] = frame->linesize[1];
                image.ppPlane[0] = frame->data[0];
                image.ppPlane[1] = frame->data[1];
                break;
            case AV_PIX_FMT_NV21:
                image.format = IMAGE_FORMAT_NV21;
                image.pLineSize[0] = frame->linesize[0];
                image.pLineSize[1] = frame->linesize[1];
                image.ppPlane[0] = frame->data[0];
                image.ppPlane[1] = frame->data[1];
                break;
            case AV_PIX_FMT_RGBA:
            default:
                image.format = IMAGE_FORMAT_RGBA;
                image.pLineSize[0] = frame->linesize[0];
                image.ppPlane[0] = frame->data[0];
                break;
        }
        Rending();
        double currentDecoderTime = (frame->pts * av_q2d(GetAVStream()->time_base) + m_CoverImageDuration) * 1000;
        RenderInner(&image, currentDecoderTime);
    }
}

void VideoDecoder::VideoRenderCoverImage() {
    if (m_CoverImage && m_CoverImage->ppPlane[0]) {
        int m_ImageSizeOneSecond = av_q2d(GetAVStream()->r_frame_rate);
        while (m_RenderCoverImageIndex <= m_ImageSizeOneSecond * m_CoverImageDuration) {
            m_RenderCoverImageIndex++;
            Rending();
            double currentDecoderTime = 1000 * m_RenderCoverImageIndex / m_ImageSizeOneSecond;
            RenderInner(m_CoverImage, currentDecoderTime);
        }
        m_RenderCoverImageIndex = 0;
    }
}