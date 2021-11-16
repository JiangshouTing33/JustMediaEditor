//
// Created by 江守亭 on 2021/9/28.
//

#include "JustMediaEditor.h"
void JustMediaEditor::Init(JNIEnv *jniEnv, jobject obj, const char *srcVideoUrl, const char *srcAudioUrl, const char *dstUrl) {
    if (!dstUrl || strlen(dstUrl) <= 0) {
        return;
    }

    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);

    strcpy(m_DstUrl, dstUrl);

    if (srcVideoUrl && strlen(srcVideoUrl) > 0) {
        m_VideoStreamNeedInit = true;
    }

    if (srcAudioUrl && strlen(srcAudioUrl) > 0) {
        m_AudioStreamNeedInit = true;
    }

    if (m_VideoDecoder == nullptr && srcVideoUrl && strlen(srcVideoUrl) > 0) {
        m_VideoDecoder = new VideoDecoder(srcVideoUrl);
        m_VideoDecoder->SetDecodeStatusChangeCallback(this, OnVideoDecodeMessageCallback);

        if (m_EGLVideoRender == nullptr) {
            m_EGLVideoRender = new EGLVideoRender();
        }
        m_EGLVideoRender->SetRenderFrameCallback(this, OnVideoFrameAvailable);
        m_VideoDecoder->SetVideoRender(m_EGLVideoRender);
    }

    if (m_AudioDecoder == nullptr && srcAudioUrl && strlen(srcAudioUrl) > 0) {
        m_AudioDecoder = new AudioDecoder(srcAudioUrl);
        m_AudioDecoder->SetDecodeStatusChangeCallback(this, OnAudioDecodeMessageCallback);
    }

    if (m_BaseMuxer == nullptr) {
        if (m_VideoStreamNeedInit && m_AudioStreamNeedInit) {
			m_MuxerType = MUXER_TYPE_VIDEO_AUDIO;
        } else if (m_VideoStreamNeedInit) {
			m_MuxerType = MUXER_TYPE_VIDEO_ONLY;
        } else if (m_AudioStreamNeedInit) {
			m_MuxerType = MUXER_TYPE_AUDIO_ONLY;
        }
        m_BaseMuxer = new BaseMuxer(dstUrl, m_MuxerType);
        m_BaseMuxer->SetMuxerMessageCallback(this, OnMuxerMessageCallback);
    }
}

void JustMediaEditor::UnInit() {
    if (m_VideoDecoder) {
        delete m_VideoDecoder;
        m_VideoDecoder = nullptr;
    }

    if (m_AudioDecoder) {
        delete m_AudioDecoder;
        m_AudioDecoder = nullptr;
    }

    if (m_BaseMuxer) {
        m_BaseMuxer->UnInit();
        delete m_BaseMuxer;
        m_BaseMuxer = nullptr;
    }

    if (m_EGLVideoRender) {
		m_EGLVideoRender->UnInit();
		delete m_EGLVideoRender;
		m_EGLVideoRender = nullptr;
    }

	if (m_BaseAudioFilter) {
        m_BaseAudioFilter->Stop();
        delete m_BaseAudioFilter;
        m_BaseAudioFilter = nullptr;
	}

	if (!m_StickerDisplayOptions.empty()) {
	    StickerDisplayOption option = m_StickerDisplayOptions.front();
	    FrameImageUtil::FreeFrameImage(option.m_StickerImage);
        m_StickerDisplayOptions.pop_front();
	}
}

void JustMediaEditor::Start() {
    if (m_VideoDecoder) {
        m_VideoDecoder->Start();
    }

    if (m_AudioDecoder) {
        m_AudioDecoder->Start();
    }

    if (m_BackgroundDecoder) {
        m_BackgroundDecoder->Start();
    }
}

void JustMediaEditor::Pause() {
    if (m_VideoDecoder) {
        m_VideoDecoder->Pause();
    }

    if (m_AudioDecoder) {
        m_AudioDecoder->Pause();
    }

    if (m_BackgroundDecoder) {
        m_BackgroundDecoder->Pause();
    }

    if (m_BaseMuxer) {
        m_BaseMuxer->Pause();
    }
}

void JustMediaEditor::Stop() {
    if (m_VideoDecoder) {
        m_VideoDecoder->Stop();
    }

    if (m_AudioDecoder) {
        m_AudioDecoder->Stop();
    }

    if (m_BackgroundDecoder) {
        m_BackgroundDecoder->Stop();
    }

    if (m_BaseMuxer) {
        m_BaseMuxer->Stop();
    }
}

void JustMediaEditor::SetCoverImage(int width, int height, const uint8_t *data, double duration) {
    if (m_VideoDecoder && data && duration > 0) {
        m_CoverImage.width = width;
        m_CoverImage.height = height;
        m_CoverImage.format = IMAGE_FORMAT_RGBA;
        m_CoverImageDuration = duration;
        FrameImageUtil::AllocFrameImage(&m_CoverImage);
        m_CoverImage.pLineSize[0] = width * 4;
        memcpy(m_CoverImage.ppPlane[0], data, width * height * 4);
        m_VideoDecoder->SetCoverImage(&m_CoverImage, duration);
    }
}

void JustMediaEditor::SetImageMarkData(int width, int height, uint8_t *pdata, float markPositionX, float markPositionY) {
    if (pdata && width > 0 && height > 0 && m_EGLVideoRender) {
        m_MarkImage.width = width;
        m_MarkImage.height = height;
        m_MarkImage.format = IMAGE_FORMAT_RGBA;
        FrameImageUtil::AllocFrameImage(&m_MarkImage);
        m_MarkImage.pLineSize[0] = m_MarkImage.width * 4;
        memcpy(m_MarkImage.ppPlane[0], pdata, m_MarkImage.width * m_MarkImage.height * 4);
        m_MarkPositionX = markPositionX;
        m_MarkPositionY = markPositionY;

        if (m_MarkRenderSample == nullptr) {
            m_MarkRenderSample = new MarkRenderSample();
            m_EGLVideoRender->SetMarkRenderSample(m_MarkRenderSample);
        }
    }
}

void JustMediaEditor::AddTextDisplayParam(const wchar_t *text, const char *fontName, long startTime,
    long endTime, float red, float green, float blue, float positionX, float positionY) {
    TextDisplayParam *displayParam = new TextDisplayParam {
        text,
        fontName,
        startTime,
        endTime,
        1,
        1,
        glm::vec3 (red, green, blue),
        glm::vec2 (positionX, positionY)
    };
	if (DataFilterUtil::TextDisplayParamFilter(displayParam) && m_EGLVideoRender) {
        m_TextDisplayParams.push_back(displayParam);
        if (m_FontRenderSample == nullptr) {
            m_FontRenderSample = new FontRenderSample();
            m_EGLVideoRender->SetFontRenderSample(m_FontRenderSample);
        }
	}
}

void JustMediaEditor::AddStickerDisplayOption(int width, int height, const uint8_t *data, long startTime, long endTime, float positionX, float positionY) {
	FrameImage *stickerImage = new FrameImage ();
	stickerImage->width = width;
	stickerImage->height = height;
	stickerImage->format = IMAGE_FORMAT_RGBA;

	FrameImageUtil::AllocFrameImage(stickerImage);
	stickerImage->pLineSize[0] = width * 4;
	memcpy(stickerImage->ppPlane[0], data, width * height * 4);

	vec2 position = vec2(positionX, positionY);

	StickerDisplayOption stickerDisplayOption = StickerDisplayOption {
		stickerImage,
		startTime,
		endTime,
		position
	};

	if (DataFilterUtil::StickerDisplayOptionFilter(stickerDisplayOption) && m_EGLVideoRender) {
        m_StickerDisplayOptions.push_back(stickerDisplayOption);
        if (m_StickerRenderSample == nullptr) {
            m_StickerRenderSample = new StickerRenderSample();
            m_EGLVideoRender->SetStickerRenderSample(m_StickerRenderSample);
        }
	}
}

void JustMediaEditor::SetLutImageData(int width, int height, const uint8_t *data) {
    FrameImage *lutImage = new FrameImage ();
    lutImage->width = width;
    lutImage->height = height;
    lutImage->format = IMAGE_FORMAT_RGBA;

    FrameImageUtil::AllocFrameImage(lutImage);
    lutImage->pLineSize[0] = width * 4;
    memcpy(lutImage->ppPlane[0], data, width * height * 4);
    if (frameRenderSample == nullptr) {
        frameRenderSample = new LutSample();
        dynamic_cast<LutSample *>(frameRenderSample)->SetLutImageData(lutImage);
    }
}

void JustMediaEditor::SetAudioFilterData(int audioFilterType,
    const char *desc, const char *inFilterNameOne, const char *inFilterNameTwo, const char *outFilterName) {
    m_AudioFilterType = audioFilterType;
    switch (m_AudioFilterType) {
        case AudioFilterType::TYPE_ONE_INPUT:
            m_BaseAudioFilter = new AudioSingleInputFilter(desc, inFilterNameOne, outFilterName);
            m_AudioFilterOneInitSuccess = false;
            break;
        case AudioFilterType::TYPE_TWO_INPUT:
            m_BaseAudioFilter = new AudioDoubleInputFilter(desc, inFilterNameOne, inFilterNameTwo, outFilterName);
            m_AudioFilterOneInitSuccess = false;
            m_AudioFilterTwoInitSuccess = false;
            break;
    }

    if (m_BaseAudioFilter) {
        m_BaseAudioFilter->SetAudioFilterMessageCallback(this, OnAudioFilterMessageCallback);
    }
}

void JustMediaEditor::SetBackgroundMusicData(const char *backgroundAudioUrl) {
    if (m_AudioFilterType == AudioFilterType::TYPE_TWO_INPUT && m_BaseAudioFilter) {
        if (m_BackgroundDecoder == nullptr) {
            m_BackgroundDecoder = new AudioDecoder(backgroundAudioUrl);
            m_BackgroundDecoder->SetDecodeStatusChangeCallback(this, OnBackgroundAudioDecodeMessageCallback);
        }
    }
}

void JustMediaEditor::ChangeVideoSpeed(float videoSpeed) {
    if (videoSpeed <= 0) {
        videoSpeed = 1.f;
    }
	m_SpeedFactor = videoSpeed;
    if (m_VideoDecoder) {
        m_VideoDecoder->SetSpeedFactor(m_SpeedFactor);
    }

    if (m_AudioDecoder) {
        m_AudioDecoder->SetSpeedFactor(m_SpeedFactor);
    }
}

void JustMediaEditor::SetMuxerParam(int width, int height, long bitRate) {
    m_DstWidth = width;
    m_DstHeight = height;
    m_DstBitRate = bitRate;
}

JNIEnv * JustMediaEditor::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == m_JavaVM) {
        LOGCATE("JustMediaEditor::GetJNIEnv..Error..m_JavaVM == nullptr");
        return nullptr;
    }
    *isAttach = false;
    status = m_JavaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = m_JavaVM->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGCATE("FJustMediaEditor::GetJNIEnv..Error.. failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}

void JustMediaEditor::StartMuxer() {
    if (m_VideoStreamNeedInit || m_AudioStreamNeedInit) return;

    if (m_BaseMuxer) {
        if (m_BaseMuxer->Init() == 0) {
            if (m_CoverImageDuration != 0) {
                m_BaseMuxer->SetCoverImageDuration(m_CoverImageDuration);
            }
            m_BaseMuxer->Start();
        } else {
            Stop();
        }
    }
}

void JustMediaEditor::PostMessageToJava(int messageType, double message) {
    bool isAttach = false;
    JNIEnv *env = GetJNIEnv(&isAttach);
    if(env == nullptr) return;
    jmethodID mid = env->GetMethodID(env->GetObjectClass(m_JavaObj), JAVA_EDITOR_MESSAGE_CALLBACK_API_NAME, "(IF)V");
    env->CallVoidMethod(m_JavaObj, mid, messageType, message);
    if(isAttach)
        m_JavaVM->DetachCurrentThread();
}

void JustMediaEditor::InitVideoRenderSample() {
    if (m_MarkImage.ppPlane[0] && m_MarkRenderSample) {
        m_MarkRenderSample->SetMarkImage(&m_MarkImage, m_MarkPositionX, m_MarkPositionY);
    }

    if (m_FontRenderSample && !m_TextDisplayParams.empty() && m_VideoDecoder) {
        m_VideoDecoder->SetTextDisplayParams(m_TextDisplayParams);
    }

    if (m_StickerRenderSample && !m_StickerDisplayOptions.empty() && m_VideoDecoder) {
        m_VideoDecoder->SetStickerDisplayOptions(m_StickerDisplayOptions);
    }
}

void JustMediaEditor::TryStartAudioFilter() {
    if (m_AudioFilterOneInitSuccess && m_AudioFilterTwoInitSuccess && m_BaseAudioFilter) {
        m_BaseAudioFilter->Start();
    }
}

void JustMediaEditor::OnVideoDecodeMessageCallback(void *ctx, int message, void *data) {
    JustMediaEditor *justMediaEditor = static_cast<JustMediaEditor *>(ctx);

    if (message == DECODE_MESSAGE_DONE) {
        if (justMediaEditor->m_BaseMuxer) {
            justMediaEditor->m_BaseMuxer->VideoFrameInputDone();
        }
    } else if (message == DECODE_MESSAGE_DURATION) {
        double *p_duration = static_cast<double *>(data);
        double duration = *p_duration;
        justMediaEditor->PostMessageToJava(EDITOR_MESSAGE_TYPE_DURATION, duration);
    } else if (message == DECODE_MESSAGE_READY) {
        if (justMediaEditor->m_BaseMuxer) {
            AVCodecContext *avCodecContext = justMediaEditor->m_VideoDecoder->GetAVCodecContext();
            AVStream *avStream = justMediaEditor->m_VideoDecoder->GetAVStream();

            VideoEncodeParam vEncodeParam = {
                    justMediaEditor->m_DstWidth <= 0 ? avCodecContext->width : justMediaEditor->m_DstWidth,
                    justMediaEditor->m_DstHeight <= 0 ? avCodecContext->height : justMediaEditor->m_DstHeight,
                    justMediaEditor->m_DstBitRate <= 0 ? avCodecContext->bit_rate : justMediaEditor->m_DstBitRate,
                    (int)(round(av_q2d(avStream->avg_frame_rate)) * justMediaEditor->m_SpeedFactor)
            };
            justMediaEditor->m_BaseMuxer->SetVideoEncodeParam(vEncodeParam);
            if (justMediaEditor->m_EGLVideoRender) {
                if (justMediaEditor->frameRenderSample == nullptr) {
                    justMediaEditor->frameRenderSample = new DefaultFrameRenderSample();
                }
                justMediaEditor->m_EGLVideoRender->SetFrameRenderSample(justMediaEditor->frameRenderSample);
                justMediaEditor->m_EGLVideoRender->SetDstSize(
                    justMediaEditor->m_DstWidth <= 0 ? avCodecContext->width : justMediaEditor->m_DstWidth,
                    justMediaEditor->m_DstHeight <= 0 ? avCodecContext->height : justMediaEditor->m_DstHeight);
                justMediaEditor->InitVideoRenderSample();
            }
            justMediaEditor->m_VideoDecoder->Decoding();
            justMediaEditor->m_VideoStreamNeedInit = false;
            justMediaEditor->StartMuxer();
        }
    }
}

void JustMediaEditor::OnAudioDecodeMessageCallback(void *ctx, int message, void *data) {
    JustMediaEditor *justMediaEditor = static_cast<JustMediaEditor *>(ctx);
    if (message == DECODE_MESSAGE_DONE) {
        if (justMediaEditor->m_BaseAudioFilter) {
            justMediaEditor->m_BaseAudioFilter->OnInputOneExit();
        } else if (justMediaEditor->m_BaseMuxer) {
            justMediaEditor->m_BaseMuxer->AudioFrameInputDone();
        }
    } else if (message == DECODE_MESSAGE_DURATION) {
    	if (justMediaEditor->m_MuxerType == MUXER_TYPE_AUDIO_ONLY) {
			double *p_duration = static_cast<double *>(data);
			double duration = *p_duration;
			justMediaEditor->PostMessageToJava(EDITOR_MESSAGE_TYPE_DURATION, duration);
    	}
	} else if (message == DECODE_MESSAGE_FRAME) {
        if (justMediaEditor->m_BaseAudioFilter) {
            justMediaEditor->m_BaseAudioFilter->OnInputOneFrameAvailable(static_cast<AVFrame *>(data));
        } else if (justMediaEditor->m_BaseMuxer) {
            justMediaEditor->m_BaseMuxer->OnAudioFrame2Encode(static_cast<AVFrame *>(data));
        }
    } else if (message == DECODE_MESSAGE_READY) {
        if (justMediaEditor->m_BaseAudioFilter) {
            AVCodecContext *avCodecContext = justMediaEditor->m_AudioDecoder->GetAVCodecContext();
            justMediaEditor->m_AudioFilterOneInitSuccess = justMediaEditor->m_BaseAudioFilter->InitInFilterOne(avCodecContext) == 0;
            justMediaEditor->TryStartAudioFilter();
        } else if (justMediaEditor->m_BaseMuxer) {
            AVCodecContext *avCodecContext = justMediaEditor->m_AudioDecoder->GetAVCodecContext();
            AudioEncodeParam aEncodeParam = {
                44100,
                AV_CH_LAYOUT_STEREO
            };
            justMediaEditor->m_BaseMuxer->SetAudioEncodeParam(aEncodeParam);
            int ret = justMediaEditor->m_BaseMuxer->InitAudioFrameQueue(avCodecContext->sample_fmt, avCodecContext->channel_layout,
                                                                        avCodecContext->frame_size, avCodecContext->sample_rate);

            if (ret == 0) {
                justMediaEditor->m_AudioDecoder->Decoding();
                justMediaEditor->m_AudioStreamNeedInit = false;
                justMediaEditor->StartMuxer();
            } else {
                justMediaEditor->m_AudioDecoder->Stop();
            }
        }
    }
}

void JustMediaEditor::OnMuxerMessageCallback(void *ctx, int message, void *data) {
    JustMediaEditor *justMediaEditor = static_cast<JustMediaEditor *>(ctx);
    if (message == MUXER_MESSAGE_TIME_STAMP) {
        double *time = static_cast<double *>(data);
        double timeStamp = *time;
        justMediaEditor->PostMessageToJava(EDITOR_MESSAGE_TYPE_CURRENT_TIME, timeStamp);
    } else if (message == MUXER_MESSAGE_DONE) {
        justMediaEditor->PostMessageToJava(EDITOR_MESSAGE_TYPE_DONE, 1);
    } else if (message == MUXER_MESSAGE_VIDEO_QUEUE_FULL) {
        if (justMediaEditor->m_VideoDecoder) {
            justMediaEditor->m_VideoDecoder->Pause();
            usleep(3000 * 1000);
            justMediaEditor->m_VideoDecoder->Decoding();
        }
    }
}

void JustMediaEditor::OnVideoFrameAvailable(void *ctx, void *image) {
    JustMediaEditor *justMediaEditor = static_cast<JustMediaEditor *>(ctx);
    if (justMediaEditor->m_BaseMuxer) {
        justMediaEditor->m_BaseMuxer->OnVideoFrame2Encode(static_cast<FrameImage *>(image));
    }
}

void JustMediaEditor::OnBackgroundAudioDecodeMessageCallback(void *ctx, int message, void *data) {
    JustMediaEditor *justMediaEditor = static_cast<JustMediaEditor *>(ctx);
    if (justMediaEditor->m_AudioFilterType == AudioFilterType::TYPE_TWO_INPUT && justMediaEditor->m_BaseAudioFilter) {
        AudioDoubleInputFilter *audioDoubleInputFilter = dynamic_cast<AudioDoubleInputFilter *>(justMediaEditor->m_BaseAudioFilter);
        if (message == DECODE_MESSAGE_DONE) {
            audioDoubleInputFilter->OnInputTwoExit();
        } else if (message == DECODE_MESSAGE_FRAME) {
            audioDoubleInputFilter->OnInputTwoFrameAvailable(static_cast<AVFrame *>(data));
        } else if (message == DECODE_MESSAGE_READY) {
            AVCodecContext *avCodecContext = justMediaEditor->m_BackgroundDecoder->GetAVCodecContext();
            justMediaEditor->m_AudioFilterTwoInitSuccess = audioDoubleInputFilter->InitInFilterTwo(avCodecContext) == 0;
            justMediaEditor->TryStartAudioFilter();
        }
    }
}

void JustMediaEditor::OnAudioFilterMessageCallback(void *ctx, int message, void *data) {
    JustMediaEditor *justMediaEditor = static_cast<JustMediaEditor *>(ctx);
    if (message == FILTER_MESSAGE_READY) {
        AudioEncodeParam aEncodeParam = {
            DEFAULT_FILTER_SAMPLE_RATE,
            DEFAULT_FILTER_CHANNEL_LAYOUT
        };
        justMediaEditor->m_BaseMuxer->SetAudioEncodeParam(aEncodeParam);
        int ret = justMediaEditor->m_BaseMuxer->InitAudioFrameQueue(DEFAULT_FILTER_SAMPLE_FMT, DEFAULT_FILTER_CHANNEL_LAYOUT,
                                                                    DEFAULT_FILTER_FRAME_SIZE, DEFAULT_FILTER_SAMPLE_RATE);
        if (ret == 0) {
            if (justMediaEditor->m_AudioDecoder) {
                justMediaEditor->m_AudioDecoder->Decoding();
            }

            if (justMediaEditor->m_BackgroundDecoder) {
                justMediaEditor->m_BackgroundDecoder->Decoding();
            }
            justMediaEditor->m_AudioStreamNeedInit = false;
            justMediaEditor->StartMuxer();
        } else {
            if (justMediaEditor->m_AudioDecoder) {
                justMediaEditor->m_AudioDecoder->Stop();
            }

            if (justMediaEditor->m_BackgroundDecoder) {
                justMediaEditor->m_BackgroundDecoder->Stop();
            }
        }
    } else if (message == FILTER_MESSAGE_FRAME) {
        if (justMediaEditor->m_BaseMuxer) {
            justMediaEditor->m_BaseMuxer->OnAudioFrame2Encode(static_cast<AVFrame *>(data));
        }
    } else if (message == FILTER_MESSAGE_DONE) {
        if (justMediaEditor->m_BaseMuxer) {
            justMediaEditor->m_BaseMuxer->AudioFrameInputDone();
        }
    }
}