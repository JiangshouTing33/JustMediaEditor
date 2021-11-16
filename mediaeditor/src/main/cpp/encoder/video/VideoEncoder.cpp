//
// Created by 江守亭 on 2021/9/27.
//

#include "VideoEncoder.h"
void VideoEncoder::OnVideoFrame2Encode(FrameImage *image) {
    if (image) {
//        LOGCATD("VideoEncoder::OnVideoFrame2Encode..image[width, height, format] = [%d, %d, %d]", image->width, image->height, image->format);
        FrameImage *frameImage = new FrameImage();
        frameImage->width = image->width;
        frameImage->height = image->height;
        frameImage->format = image->format;
        FrameImageUtil::CopyFrameImage(frameImage, image);
        m_VideoFrameQueue.push(frameImage);

        if (m_VideoFrameQueue.size() > VIDEO_FRAME_QUEUE_MAX_SIZE && m_EncodeMessageCallback && m_MessageContext) {
            m_EncodeMessageCallback(m_MessageContext, ENCODE_MESSAGE_VIDEO_QUEUE_FULL, nullptr);
        }
    }
}

int VideoEncoder::InitFrameQueue() {
    return 0;
}

void VideoEncoder::UpdateAVCodecContextParam() {
    p_AVCdcCtx->width = m_VideoEncodeParam.frameWidth;
    p_AVCdcCtx->height = m_VideoEncodeParam.frameHeight;
    p_AVCdcCtx->bit_rate = m_VideoEncodeParam.videoBitRate;
    p_AVCdcCtx->time_base = {1, m_VideoEncodeParam.fps == 0 ? 1 : m_VideoEncodeParam.fps};
    p_AVCdcCtx->pix_fmt = AVPixelFormat(m_VideoEncodeParam.pixFmt);
}

int VideoEncoder::InitFrame() {
    p_VideoFrame = av_frame_alloc();
    if (p_VideoFrame == nullptr) {
        LOGCATE("VideoEncoder::InitFrame()..Failed, av_frame_alloc(p_VideoFrame) error");
        return -1;
    }

    p_TmpFrame = av_frame_alloc();
    if (p_TmpFrame == nullptr) {
        LOGCATE("VideoEncoder::InitFrame()..Failed, av_frame_alloc(p_TmpFrame) error");
        return -1;
    }

    p_VideoFrame->width = p_AVCdcCtx->width;
    p_VideoFrame->height = p_AVCdcCtx->height;
    p_VideoFrame->format = p_AVCdcCtx->pix_fmt;

    int ret = av_frame_get_buffer(p_VideoFrame, 1);
    if (ret < 0) {
        LOGCATE("VideoEncoder::InitFrame()..Failed, av_frame_get_buffer error, ret = %s", av_err2str(ret));
        return ret;
    }

    return 0;
}

void VideoEncoder::UnInitEncoder() {
    while (!m_VideoFrameQueue.empty()) {
        FrameImage *image = m_VideoFrameQueue.front();
        FrameImageUtil::FreeFrameImage(image);
        delete image;
        m_VideoFrameQueue.pop();
    }

    if (p_VideoFrame) {
        av_frame_free(&p_VideoFrame);
        p_VideoFrame = nullptr;
    }

    if (p_TmpFrame) {
        av_frame_free(&p_TmpFrame);
        p_TmpFrame = nullptr;
    }

    if (p_SwsCtx) {
        sws_freeContext(p_SwsCtx);
        p_SwsCtx = nullptr;
    }
}

AVFrame * VideoEncoder::GetFrameFromQueue() {
    int ret;

    AVFrame *avFrame;

    if (m_VideoFrameQueue.empty() && m_InputExit) {
        return nullptr;
    }
    while (m_VideoFrameQueue.empty() && !m_InputExit) {
        std::unique_lock<std::mutex> lock(m_CondMutex);
        m_Cond.wait_for(lock, std::chrono::milliseconds(5));
    }

    FrameImage *frameImage = m_VideoFrameQueue.front();

    do {
        p_TmpFrame->width = frameImage->width;
        p_TmpFrame->height = frameImage->height;
        p_TmpFrame->data[0] = frameImage->ppPlane[0];
        p_TmpFrame->data[1] = frameImage->ppPlane[1];
        p_TmpFrame->data[2] = frameImage->ppPlane[2];
        p_TmpFrame->linesize[0] = frameImage->pLineSize[0];
        p_TmpFrame->linesize[1] = frameImage->pLineSize[1];
        p_TmpFrame->linesize[2] = frameImage->pLineSize[2];

        switch (frameImage->format) {
            case IMAGE_FORMAT_RGBA:
                p_TmpFrame->format = AV_PIX_FMT_RGBA;
                break;
            case IMAGE_FORMAT_NV21:
                p_TmpFrame->format = AV_PIX_FMT_NV21;
                break;
            case IMAGE_FORMAT_NV12:
                p_TmpFrame->format = AV_PIX_FMT_NV12;
                break;
            case IMAGE_FORMAT_I420:
                p_TmpFrame->format = AV_PIX_FMT_YUV420P;
                break;
            default:
                p_TmpFrame->format = AV_PIX_FMT_YUV420P;
                LOGCATE("VideoEncoder::GetFrameFromQueue unSupport format pImage->format=%d", frameImage->format);
                break;
        }

        if (p_TmpFrame->format != m_VideoEncodeParam.pixFmt) {
            ret = av_frame_make_writable(p_VideoFrame);
            if (ret < 0) {
                LOGCATE("VideoEncoder::GetFrameFromQueue av_frame_make_writable error, ret = %s", av_err2str(ret));
                break;
            }
            if (p_SwsCtx == nullptr) {
                p_SwsCtx = sws_getContext(
                        p_TmpFrame->width, p_TmpFrame->height, AVPixelFormat(p_TmpFrame->format),
                        p_AVCdcCtx->width, p_AVCdcCtx->height, p_AVCdcCtx->pix_fmt, SWS_FAST_BILINEAR,
                        nullptr, nullptr, nullptr);
            } else if (m_LastSrcWidth != p_TmpFrame->width || m_LastSrcHeight != p_TmpFrame->height || m_LastSrcFormat != p_TmpFrame->format) {
                sws_freeContext(p_SwsCtx);
                p_SwsCtx = sws_getContext(
                        p_TmpFrame->width, p_TmpFrame->height, AVPixelFormat(p_TmpFrame->format),
                        p_AVCdcCtx->width, p_AVCdcCtx->height, p_AVCdcCtx->pix_fmt, SWS_FAST_BILINEAR,
                        nullptr, nullptr, nullptr);
            }

            if (p_SwsCtx == nullptr) {
                LOGCATE("VideoEncoder::GetFrameFromQueue sws_getContext error");
                break;
            }

            if (m_LastSrcWidth != p_TmpFrame->width) {
                m_LastSrcWidth = p_TmpFrame->width;
            }

            if (m_LastSrcHeight != p_TmpFrame->height) {
                m_LastSrcHeight = p_TmpFrame->height;
            }

            if (m_LastSrcFormat != p_TmpFrame->format) {
                m_LastSrcFormat = p_TmpFrame->format;
            }

            ret = sws_scale(p_SwsCtx, p_TmpFrame->data, p_TmpFrame->linesize,
                    0, p_TmpFrame->height, p_VideoFrame->data, p_VideoFrame->linesize);

            if (ret < 0) {
                LOGCATE("VideoEncoder::GetFrameFromQueue sws_scale error, ret = %s", av_err2str(ret));
                break;
            }

            avFrame = p_VideoFrame;
        } else {
            avFrame = p_TmpFrame;
        }

        avFrame->pts = m_NextPts;
        m_NextPts += 1;
    } while (false);

    return avFrame;
}

void VideoEncoder::ReleaseFrameRes() {
    if (!m_VideoFrameQueue.empty()) {
        FrameImage *frameImage = m_VideoFrameQueue.front();

        m_VideoFrameQueue.pop();
        if (frameImage) {
            FrameImageUtil::FreeFrameImage(frameImage);
            delete frameImage;
            frameImage = nullptr;
        }
    }
}
