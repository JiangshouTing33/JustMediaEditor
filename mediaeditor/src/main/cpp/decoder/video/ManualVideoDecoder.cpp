//
// Created by 江守亭 on 2021/10/25.
//

#include "ManualVideoDecoder.h"
ManualVideoDecoder::ManualVideoDecoder(int imageSizeOneSecond, float perImageDuration) {
	m_PerImageDuration = perImageDuration;
	m_ImageSizeOneSecond = imageSizeOneSecond;
	m_ImageDurationFrame = (int)(imageSizeOneSecond * perImageDuration);
}

ManualVideoDecoder::~ManualVideoDecoder() {
	if (m_VideoDecoderThread != nullptr) {
		Stop();
		m_VideoDecoderThread->join();
		delete m_VideoDecoderThread;
		m_VideoDecoderThread = nullptr;
	}
	for (auto & m_ImageRe : m_ImageRes) {
		FrameImageUtil::FreeFrameImage(&m_ImageRe);
	}
}

void ManualVideoDecoder::SetImageSourceWithIndex(int index, int width, int height, uint8_t *pdata) {
	if (index < PICTURE_MAX_SIZE && pdata) {
		std::lock_guard<std::mutex> lk(m_Mutex);
		m_ImageRes[index].width = width;
		m_ImageRes[index].height = height;
		m_ImageRes[index].format = IMAGE_FORMAT_RGBA;
		FrameImageUtil::AllocFrameImage(&m_ImageRes[index]);
		m_ImageRes[index].pLineSize[0] = m_ImageRes[index].width * 4;
		memcpy(m_ImageRes[index].ppPlane[0], pdata, m_ImageRes[index].width * 4 * m_ImageRes[index].height);
	}
}

void ManualVideoDecoder::SetImageCount(int imageCount) {
	m_ImageCount = imageCount;
}

void ManualVideoDecoder::SetTransitionRenderSample(TransitionRenderSample *transitionRenderSample) {
	m_TransitionRenderSample = transitionRenderSample;
}

void ManualVideoDecoder::Start() {
	if (m_VideoDecoderThread == nullptr) {
		m_VideoDecoderThread = new std::thread(StartVideoDecoder, this);
	} else {
		ChangeVideoStatus(VIDEO_DECODE_STATUS_DECODING);
		m_Cond.notify_all();
	}
}

void ManualVideoDecoder::Stop() {
	ChangeVideoStatus(VIDEO_DECODE_STATUS_STOP);
	m_Cond.notify_all();
}

void ManualVideoDecoder::Decoding() {
	ChangeVideoStatus(VIDEO_DECODE_STATUS_DECODING);
	m_Cond.notify_all();
}

void ManualVideoDecoder::Rending() {
	ChangeVideoStatus(VIDEO_DECODE_STATUS_RENDING);
}

void ManualVideoDecoder::Pause() {
	ChangeVideoStatus(VIDEO_DECODE_STATUS_DECODING);
}

void ManualVideoDecoder::VideoDecoderLoop() {
	while (true) {
		if (m_VideoDecoderStatus == VIDEO_DECODE_STATUS_STOP) {
			break;
		}

		if (m_VideoDecoderStatus == VIDEO_DECODE_STATUS_PAUSE) {
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Cond.wait_for(lock, std::chrono::milliseconds(50));
		}

		if (m_VideoDecoderStatus == VIDEO_DECODE_STATUS_RENDING) {
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Cond.wait_for(lock, std::chrono::milliseconds(1));
		}

		VideoRender();
	}
}

void ManualVideoDecoder::VideoDecoderReady() {
	SetVideoRenderDoneCallback(this, OnVideoRenderDone);
	double duration = m_ImageCount * m_PerImageDuration;

	if (m_CoverImage && m_CoverImage->ppPlane[0]) {
		m_CoverImageDuration = m_PerImageDuration;
		duration += m_PerImageDuration;
	}

	m_RenderImageIndex = 0;
	m_RenderIndexInner = 0;
	if (m_VideoDecodeMessageCallback && m_VideoDecodeMessageContext) {
		m_VideoDecodeMessageCallback(m_VideoDecodeMessageContext, VIDEO_DECODE_MESSAGE_READY, nullptr);
		m_VideoDecodeMessageCallback(m_VideoDecodeMessageContext, VIDEO_DECODE_MESSAGE_DURATION, &duration);
	}
	if (m_TransitionRenderSample != nullptr) {
		m_TransitionRenderSample->SetOneFrameImageCount(m_ImageDurationFrame);
	}
	InitVideoRender();
}

void ManualVideoDecoder::VideoDecodeDone() {
	if (m_VideoDecodeMessageCallback && m_VideoDecodeMessageContext) {
		m_VideoDecodeMessageCallback(m_VideoDecodeMessageContext, VIDEO_DECODE_MESSAGE_DONE, nullptr);
	}
}

void ManualVideoDecoder::VideoRender() {
	if (m_RenderIndexInner >= m_ImageDurationFrame) {
		m_RenderIndexInner = 0;
		m_RenderImageIndex ++;
	} else {
		m_RenderIndexInner++;
	}

	FrameImage image = m_ImageRes[m_RenderImageIndex];
	if (image.ppPlane[0]) {
		Rending();
		double currentDecoderTime = ((m_RenderImageIndex * m_ImageDurationFrame + m_RenderIndexInner) / m_ImageSizeOneSecond + m_CoverImageDuration) * 1000;
		SetNextRenderImageData(m_RenderImageIndex, m_RenderIndexInner);
		RenderInner(&image, currentDecoderTime);
	} else {
		ChangeVideoStatus(VIDEO_DECODE_STATUS_STOP);
	}
}

void ManualVideoDecoder::VideoRenderCoverImage() {
	if (m_CoverImage && m_CoverImage->ppPlane[0]) {
		while (m_RenderIndexInner < m_ImageDurationFrame) {
			m_RenderIndexInner++;
			Rending();
			double currentDecoderTime = 1000 * m_RenderIndexInner / m_ImageSizeOneSecond;
			SetNextRenderImageData(-1, m_RenderIndexInner);
			RenderInner(m_CoverImage, currentDecoderTime);
		}
		m_RenderIndexInner = 0;
	}
}

void ManualVideoDecoder::SetNextRenderImageData(int imageIndex, int renderInnerIndex) {
	if (m_VideoRender == nullptr) return;
	int nextImageIndex = imageIndex + 1;
	FrameImage image = m_ImageRes[nextImageIndex];
	if (image.ppPlane[0] && m_TransitionRenderSample) {
		m_TransitionRenderSample->SetNextRenderImage(&image, renderInnerIndex);
	}
}

void ManualVideoDecoder::OnVideoRenderDone(void *ctx) {
	auto *manualVideoDecoder = static_cast<ManualVideoDecoder *>(ctx);
	if (manualVideoDecoder->m_VideoDecoderStatus == VIDEO_DECODE_STATUS_RENDING) {
		manualVideoDecoder->Decoding();
	}
}

void ManualVideoDecoder::StartVideoDecoder(ManualVideoDecoder *manualVideoDecoder) {
	do {
		manualVideoDecoder->VideoDecoderReady();
		manualVideoDecoder->VideoRenderCoverImage();
		manualVideoDecoder->VideoDecoderLoop();
	} while (false);
	manualVideoDecoder->VideoDecodeDone();
}

