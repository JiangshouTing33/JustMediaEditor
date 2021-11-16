//
// Created by 江守亭 on 2021/10/12.
//

#include "BaseAudioFilter.h"
BaseAudioFilter::BaseAudioFilter(const char *outName, int sample_fmt, int64_t channel_layout, int sample_rate) {
	strcpy(m_OutName, outName);
	m_OutSampleFmt = sample_fmt;
	m_OutChannelLayout = channel_layout;
	m_OutSampleRate = sample_rate;

	if (p_avFilterGraph == nullptr) {
		p_avFilterGraph = avfilter_graph_alloc();
	}
}

BaseAudioFilter::~BaseAudioFilter() {
	if (p_avFilterGraph) {
		avfilter_graph_free(&p_avFilterGraph);
		p_avFilterGraph = nullptr;
	}
}

void BaseAudioFilter::Start() {
	if (m_FilterThread == nullptr) {
		m_FilterThread = new thread(FilterAudio, this);
	}
}

void BaseAudioFilter::Stop() {
	FilterStop();
	if (m_FilterThread) {
		m_FilterThread->join();
		delete m_FilterThread;
		m_FilterThread = nullptr;
	}
}

int BaseAudioFilter::InitFilter() {
	if (InitInFilterCtx() != 0) {
		LOGCATE("BaseAudioFilter::InitFilter..InitInFilterCtx() Failed");
		return -1;
	}

	if (InitOutFilterCtx() != 0) {
		LOGCATE("BaseAudioFilter::InitFilter..InitOutFilterCtx() Failed");
		return -1;
	}

	if (InitInOutputs() != 0) {
		LOGCATE("BaseAudioFilter::InitFilter..InitInOutputs() Failed");
		return -1;
	}

	return 0;
}

int BaseAudioFilter::FilterLoop() {
	int ret;
	while (!m_FilterExit) {
		ret = BufferSrcAddFrame();
		if (ret != 0) {
			break;
		}
		AVFrame *outFrame = av_frame_alloc();
		while (!ret) {
			ret = av_buffersink_get_frame(p_OutFilterCtx, outFrame);
			if (ret == AVERROR(EAGAIN)) {
				continue;
			} else if (ret == AVERROR_EOF) {
				break;
			} else if (ret < 0) {
				LOGCATE("BaseAudioFilter::FilterLoop..av_buffersink_get_frame failed, ret = %s", av_err2str(ret));
				break;
			}
			if (m_OnAudioFilterMessageCallback && m_FilterMessageCallbackCtx) {
				m_OnAudioFilterMessageCallback(m_FilterMessageCallbackCtx, FilterMessage::FILTER_MESSAGE_FRAME, outFrame);
			}
		}
		if (outFrame) {
			av_frame_free(&outFrame);
		}
	}
	return ret;
}

void BaseAudioFilter::UnInitFilter() {
	if (p_OutFilterCtx != nullptr) {
		avfilter_free(p_OutFilterCtx);
		p_OutFilterCtx = nullptr;
	}

	UnInitFilterCtx();
}

int BaseAudioFilter::InitOutFilterCtx() {
	int result = -1;

	if (p_avFilterGraph == nullptr) {
		result = -1;
		LOGCATE("BaseAudioFilter::InitOutFilterCtx..avfilter_graph_alloc Filter check failed!");
		return result;
	}

	const AVFilter *outFilter = avfilter_get_by_name("abuffersink");
	if (outFilter == nullptr) {
		result = -1;
		LOGCATE("BaseAudioFilter::InitOutFilterCtx..avfilter_get_by_name failed");
		return result;
	}

	result = avfilter_graph_create_filter(&p_OutFilterCtx, outFilter, m_OutName, nullptr,
										  nullptr, p_avFilterGraph);

	if (result < 0) {
		LOGCATE("BaseAudioFilter::InitOutFilterCtx..avfilter_graph_create_filter failed, ret = %s", av_err2str(result));
		result = -1;
		return result;
	}

	AVSampleFormat outSampleFmt[] = {AVSampleFormat(m_OutSampleFmt), AV_SAMPLE_FMT_NONE};
	int64_t outChannelLayout[] = {m_OutChannelLayout, -1};
	int outSampleRate[] = {m_OutSampleRate, -1};

	result = av_opt_set_int_list(p_OutFilterCtx, "sample_fmts", outSampleFmt, -1, AV_OPT_SEARCH_CHILDREN);
	if (result < 0) {
		LOGCATE("BaseAudioFilter::InitOutFilterCtx..av_opt_set_int_list(sample_format) failed, ret = %s", av_err2str(result));
		result = -1;
		return result;
	}

	result = av_opt_set_int_list(p_OutFilterCtx, "channel_layouts", outChannelLayout, -1, AV_OPT_SEARCH_CHILDREN);
	if (result < 0) {
		LOGCATE("BaseAudioFilter::InitOutFilterCtx..av_opt_set_int_list(channel_layouts) failed, ret = %s", av_err2str(result));
		result = -1;
		return result;
	}

	result = av_opt_set_int_list(p_OutFilterCtx, "sample_rates", outSampleRate, -1, AV_OPT_SEARCH_CHILDREN);
	if (result < 0) {
		LOGCATE("BaseAudioFilter::InitOutFilterCtx..av_opt_set_int_list(sample_rates) failed, ret = %s", av_err2str(result));
		result = -1;
		return result;
	}
	return 0;
}

void BaseAudioFilter::FilterReady() {
	if (m_OnAudioFilterMessageCallback && m_FilterMessageCallbackCtx) {
		m_OnAudioFilterMessageCallback(m_FilterMessageCallbackCtx, FilterMessage::FILTER_MESSAGE_READY, nullptr);
	}
}

void BaseAudioFilter::FilterDone() {
	if (m_OnAudioFilterMessageCallback && m_FilterMessageCallbackCtx) {
		m_OnAudioFilterMessageCallback(m_FilterMessageCallbackCtx, FilterMessage::FILTER_MESSAGE_DONE, nullptr);
	}
}

void BaseAudioFilter::FilterAudio(BaseAudioFilter *audioFilter) {
	do {
		if (audioFilter->InitFilter() != 0) {
			break;
		}

		audioFilter->FilterReady();
		audioFilter->FilterLoop();
		audioFilter->TryFlushFrameWithoutFilter();
	} while (false);
	audioFilter->UnInitFilter();
	audioFilter->FilterDone();
}


