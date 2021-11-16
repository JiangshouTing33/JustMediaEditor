//
// Created by 江守亭 on 2021/10/25.
//

#include "JustPic2VideoEditor.h"
void JustPic2VideoEditor::Init(JNIEnv *jniEnv, jobject obj, const char *srcAudioUrl, const char *dstUrl, int dstWidth, int dstHeight, int fps, long videoBitRate) {
	if (!dstUrl || strlen(dstUrl) <= 0 || !srcAudioUrl || strlen(srcAudioUrl) <= 0) {
		return;
	}

	jniEnv->GetJavaVM(&m_JavaVM);
	m_JavaObj = jniEnv->NewGlobalRef(obj);

	strcpy(m_DstUrl, dstUrl);

	m_DstWidth = dstWidth;
	m_DstHeight = dstHeight;
	m_Fps = fps;
	m_VideoBitRate = videoBitRate;

	if (m_VideoDecoder == nullptr) {
		m_VideoDecoder = new ManualVideoDecoder(DEFAULT_IMAGE_SIZE_ONE_SECOND, DEFAULT_PER_IMAGE_DURATION);

		if (m_EGLVideoRender == nullptr) {
			m_EGLVideoRender = new EGLVideoRender();
		}
		m_EGLVideoRender->SetRenderFrameCallback(this, OnVideoFrameAvailable);
		if (m_TransitionRenderSample == nullptr) {
			m_TransitionRenderSample = new TransitionRenderSample();
		}
		m_EGLVideoRender->SetFrameRenderSample(m_TransitionRenderSample);
		m_VideoDecoder->SetTransitionRenderSample(m_TransitionRenderSample);
		m_VideoDecoder->SetVideoRender(m_EGLVideoRender);
		m_VideoDecoder->SetVideoDecodeMessageCallback(this, OnVideoDecodeMessageCallback);
	}

	if (m_AudioDecoder == nullptr) {
		m_AudioDecoder = new AudioDecoder(srcAudioUrl);
		m_AudioDecoder->SetDecodeStatusChangeCallback(this, OnAudioDecodeMessageCallback);
	}

	if (m_BaseMuxer == nullptr) {
		m_BaseMuxer = new BaseMuxer(dstUrl, MUXER_TYPE_VIDEO_AUDIO);
		m_BaseMuxer->SetMuxerMessageCallback(this, OnMuxerMessageCallback);
	}
}

void JustPic2VideoEditor::UnInit() {
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

	if (m_MarkRenderSample) {
		m_MarkRenderSample->UnInit();
		delete m_MarkRenderSample;
		m_MarkRenderSample = nullptr;
	}

	if (!m_StickerDisplayOptions.empty()) {
		StickerDisplayOption option = m_StickerDisplayOptions.front();
		FrameImageUtil::FreeFrameImage(option.m_StickerImage);
		m_StickerDisplayOptions.pop_front();
	}
}

void JustPic2VideoEditor::Start() {
	if (m_VideoDecoder) {
		m_VideoDecoder->Start();
	}

	if (m_AudioDecoder) {
		m_AudioDecoder->Start();
	}
}

void JustPic2VideoEditor::Pause() {
	if (m_VideoDecoder) {
		m_VideoDecoder->Pause();
	}

	if (m_AudioDecoder) {
		m_AudioDecoder->Pause();
	}

	if (m_BaseMuxer) {
		m_BaseMuxer->Pause();
	}
}

void JustPic2VideoEditor::Stop() {
	if (m_VideoDecoder) {
		m_VideoDecoder->Stop();
	}

	if (m_AudioDecoder) {
		m_AudioDecoder->Stop();
	}

	if (m_BaseMuxer) {
		m_BaseMuxer->Stop();
	}
}

void JustPic2VideoEditor::SetImageSourceWithIndex(int index, int width, int height, uint8_t *pdata) {
	if (m_VideoDecoder) {
		m_VideoDecoder->SetImageSourceWithIndex(index, width, height, pdata);
	}
}

void JustPic2VideoEditor::SetImageCount(int imageCount) {
	if (m_VideoDecoder) {
		m_VideoDecoder->SetImageCount(imageCount);
	}
}

void JustPic2VideoEditor::SetTransitionFShaderStr(const char *fShader) {
	if (m_TransitionRenderSample) {
		m_TransitionRenderSample->SetFShaderStr(fShader);
	}
}

void JustPic2VideoEditor::SetCoverImage(int width, int height, const uint8_t *data, double duration) {
	if (m_VideoDecoder && data && duration > 0) {
		m_CoverImage.width = width;
		m_CoverImage.height = height;
		m_CoverImage.format = IMAGE_FORMAT_RGBA;
		FrameImageUtil::AllocFrameImage(&m_CoverImage);
		m_CoverImage.pLineSize[0] = width * 4;
		memcpy(m_CoverImage.ppPlane[0], data, width * height * 4);
		m_VideoDecoder->SetCoverImage(&m_CoverImage, duration);
	}
}

void JustPic2VideoEditor::SetImageMarkData(int width, int height, uint8_t *pdata, float markPositionX, float markPositionY) {
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

void JustPic2VideoEditor::AddTextDisplayParam(const wchar_t *text, const char *fontName, long startTime,
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

void JustPic2VideoEditor::AddStickerDisplayOption(int width, int height, const uint8_t *data, long startTime, long endTime, float positionX, float positionY) {
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

JNIEnv * JustPic2VideoEditor::GetJNIEnv(bool *isAttach) {
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

void JustPic2VideoEditor::StartMuxer() {
	if (m_VideoStreamNeedInit || m_AudioStreamNeedInit) return;

	if (m_BaseMuxer) {
		if (m_BaseMuxer->Init() == 0) {
			m_BaseMuxer->Start();
		} else {
			Stop();
		}
	}
}

void JustPic2VideoEditor::PostMessageToJava(int messageType, double message) {
	bool isAttach = false;
	JNIEnv *env = GetJNIEnv(&isAttach);
	if(env == nullptr) return;
	jmethodID mid = env->GetMethodID(env->GetObjectClass(m_JavaObj), JAVA_PIC_2_V_EDITOR_MESSAGE_CALLBACK_API_NAME, "(IF)V");
	env->CallVoidMethod(m_JavaObj, mid, messageType, message);
	if(isAttach)
		m_JavaVM->DetachCurrentThread();
}

void JustPic2VideoEditor::InitVideoRenderSample() {
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

void JustPic2VideoEditor::OnVideoDecodeMessageCallback(void *ctx, int message, void *data) {
	JustPic2VideoEditor *justPic2VideoEditor = static_cast<JustPic2VideoEditor *>(ctx);

	if (message == VIDEO_DECODE_MESSAGE_READY) {
		if (justPic2VideoEditor->m_BaseMuxer) {
			VideoEncodeParam vEncodeParam = {
				justPic2VideoEditor->m_DstWidth,
				justPic2VideoEditor->m_DstHeight,
				justPic2VideoEditor->m_VideoBitRate,
				justPic2VideoEditor->m_Fps
			};
			justPic2VideoEditor->m_BaseMuxer->SetVideoEncodeParam(vEncodeParam);
			if (justPic2VideoEditor->m_EGLVideoRender) {
				justPic2VideoEditor->m_EGLVideoRender->SetDstSize(justPic2VideoEditor->m_DstWidth,
																  justPic2VideoEditor->m_DstHeight);
				justPic2VideoEditor->InitVideoRenderSample();
			}
			justPic2VideoEditor->m_VideoDecoder->Decoding();
			justPic2VideoEditor->m_VideoStreamNeedInit = false;
			justPic2VideoEditor->StartMuxer();
		}
	} else if (message == VIDEO_DECODE_MESSAGE_DURATION) {
		double *p_duration = static_cast<double *>(data);
		double duration = *p_duration;
		justPic2VideoEditor->PostMessageToJava(PIC_2_V_EDITOR_MESSAGE_TYPE_DURATION, duration);
	} else if (message == VIDEO_DECODE_MESSAGE_DONE) {
		if (justPic2VideoEditor->m_BaseMuxer) {
			justPic2VideoEditor->m_BaseMuxer->VideoFrameInputDone();
		}
	}
}

void JustPic2VideoEditor::OnAudioDecodeMessageCallback(void *ctx, int message, void *data) {
	JustPic2VideoEditor *justPic2VideoEditor = static_cast<JustPic2VideoEditor *>(ctx);
	if (message == DECODE_MESSAGE_DONE) {
		if (justPic2VideoEditor->m_BaseMuxer) {
			justPic2VideoEditor->m_BaseMuxer->AudioFrameInputDone();
		}
	} else if (message == DECODE_MESSAGE_FRAME) {
		if (justPic2VideoEditor->m_BaseMuxer) {
			justPic2VideoEditor->m_BaseMuxer->OnAudioFrame2Encode(static_cast<AVFrame *>(data));
		}
	} else if (message == DECODE_MESSAGE_READY) {
		if (justPic2VideoEditor->m_BaseMuxer) {
			AVCodecContext *avCodecContext = justPic2VideoEditor->m_AudioDecoder->GetAVCodecContext();
			AudioEncodeParam aEncodeParam = {
				44100,
				AV_CH_LAYOUT_STEREO
			};
			justPic2VideoEditor->m_BaseMuxer->SetAudioEncodeParam(aEncodeParam);
			int ret = justPic2VideoEditor->m_BaseMuxer->InitAudioFrameQueue(avCodecContext->sample_fmt, avCodecContext->channel_layout,
																		avCodecContext->frame_size, avCodecContext->sample_rate);

			if (ret == 0) {
				justPic2VideoEditor->m_AudioDecoder->Decoding();
				justPic2VideoEditor->m_AudioStreamNeedInit = false;
				justPic2VideoEditor->StartMuxer();
			} else {
				justPic2VideoEditor->m_AudioDecoder->Stop();
			}
		}
	}
}

void JustPic2VideoEditor::OnMuxerMessageCallback(void *ctx, int message, void *data) {
	JustPic2VideoEditor *justPic2VideoEditor = static_cast<JustPic2VideoEditor *>(ctx);
	if (message == MUXER_MESSAGE_TIME_STAMP) {
		double *time = static_cast<double *>(data);
		double timeStamp = *time;
		justPic2VideoEditor->PostMessageToJava(PIC_2_V_EDITOR_MESSAGE_TYPE_CURRENT_TIME, timeStamp);
	} else if (message == MUXER_MESSAGE_DONE) {
		justPic2VideoEditor->PostMessageToJava(PIC_2_V_EDITOR_MESSAGE_TYPE_DONE, 1);
	} else if (message == MUXER_MESSAGE_VIDEO_QUEUE_FULL) {
		if (justPic2VideoEditor->m_VideoDecoder) {
			justPic2VideoEditor->m_VideoDecoder->Pause();
			usleep(3000 * 1000);
			justPic2VideoEditor->m_VideoDecoder->Decoding();
		}
	}
}

void JustPic2VideoEditor::OnVideoFrameAvailable(void *ctx, void *image) {
	JustPic2VideoEditor *justPic2VideoEditor = static_cast<JustPic2VideoEditor *>(ctx);
	if (justPic2VideoEditor->m_BaseMuxer) {
		justPic2VideoEditor->m_BaseMuxer->OnVideoFrame2Encode(static_cast<FrameImage *>(image));
	}
}


