//
// Created by 江守亭 on 2021/10/22.
//

#include "VideoBaseDecoder.h"
void VideoBaseDecoder::SetVideoRender(EGLVideoRender *videoRender) {
	m_VideoRender = videoRender;
}

void VideoBaseDecoder::SetCoverImage(FrameImage *image, double duration) {
	m_CoverImage = image;
	m_CoverImageDuration = duration;
}

void VideoBaseDecoder::SetTextDisplayParams(std::deque<TextDisplayParam *> params) {
	m_TextDisplayParams = std::move(params);
	DataFilterUtil::SortTextDisplayParams(&m_TextDisplayParams);
}

void VideoBaseDecoder::SetStickerDisplayOptions(std::deque<StickerDisplayOption> options) {
	m_StickerDisplayOptions = std::move(options);
	DataFilterUtil::SortStickerDisplayOptions(&m_StickerDisplayOptions);
}

void VideoBaseDecoder::SetVideoRenderDoneCallback(void *ctx, OnRenderDoneCallback renderDoneCallback) {
	if (m_VideoRender) {
		m_VideoRender->SetRenderDoneCallback(ctx, renderDoneCallback);
	}
}

void VideoBaseDecoder::SetVideoDecodeMessageCallback(void *ctx, VideoDecodeMessageCallback videoDecodeMessageCallback) {
	m_VideoDecodeMessageContext = ctx;
	m_VideoDecodeMessageCallback = videoDecodeMessageCallback;
}

void VideoBaseDecoder::RenderInner(FrameImage *image, double currentDecoderTime) {
	if (m_VideoRender == nullptr) return;
	m_VideoRender->SetRenderFrameData(image);

	std::deque<TextDisplayParam*> renderTextParam;
	std::deque<StickerDisplayOption> renderStickerOptions;

	while (!m_TextDisplayParams.empty() && m_TextDisplayParams.front()->endTime < currentDecoderTime) {
		m_TextDisplayParams.pop_front();
	}

	while (!m_StickerDisplayOptions.empty() && m_StickerDisplayOptions.front().endTime < currentDecoderTime) {
		m_StickerDisplayOptions.pop_front();
	}

	if (m_TextDisplayParams.empty()) {
		renderTextParam.clear();
	}

	if (m_StickerDisplayOptions.empty()) {
		renderStickerOptions.clear();
	}

	for (int i = 0; i < m_TextDisplayParams.size(); ++i) {
		TextDisplayParam *param = m_TextDisplayParams[i];
		if (param->startTime > currentDecoderTime || param->endTime < currentDecoderTime) {
			break;
		} else {
			renderTextParam.push_back(param);
		}
	}

	for (int i = 0; i < m_StickerDisplayOptions.size(); ++i) {
		StickerDisplayOption option = m_StickerDisplayOptions[i];
		if (option.startTime > currentDecoderTime || option.endTime < currentDecoderTime) {
			break;
		} else {
			renderStickerOptions.push_back(option);
		}
	}

	m_VideoRender->SetTextDisplayParams(renderTextParam);
	m_VideoRender->SetStickerDisplayOptions(renderStickerOptions);
	m_VideoRender->Draw();
	renderTextParam.clear();
	renderStickerOptions.clear();
}

void VideoBaseDecoder::InitVideoRender() {
	if (m_VideoRender) {
		m_VideoRender->Init();
		m_VideoRender->LoadFontFace(m_TextDisplayParams);
	}
}

void VideoBaseDecoder::ChangeVideoStatus(VideoDecodeStatus videoDecodeStatus) {
	unique_lock<mutex> lock(m_VideoDecodeStatusMutex);
	m_VideoDecoderStatus = videoDecodeStatus;
	lock.unlock();
}