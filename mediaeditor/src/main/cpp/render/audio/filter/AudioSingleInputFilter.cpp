//
// Created by 江守亭 on 2021/10/13.
//

#include "AudioSingleInputFilter.h"
int AudioSingleInputFilter::InitInFilterCtx() {
	return m_InFilterReady;
}

int AudioSingleInputFilter::InitInOutputs() {
	int result = - 1;

	if (p_avFilterGraph == nullptr || strlen(m_FilterDesc) == 0 ) {
		LOGCATE("AudioSingleInputFilter::InitInOutputs..PreCheck Failed!");
		result = -1;
		return result;
	}

	AVFilterInOut *input = avfilter_inout_alloc();
	AVFilterInOut *output = avfilter_inout_alloc();

	if (input == nullptr || output == nullptr) {
		result = -1;
		LOGCATE("AudioSingleInputFilter::InitInOutputs..avfilter_inout_alloc Failed");
		return result;
	}

//	LOGCATD("AudioSingleInputFilter::InitInOutputs..m_FilterDesc = %s, m_InFilterName = %s, p_InFilterCtx = %p, m_OutName = %s, p_OutFilterCtx = %p",
//			m_FilterDesc, m_InFilterName, p_InFilterCtx, m_OutName, p_OutFilterCtx);
	output->name = av_strdup(m_InFilterName);
	output->filter_ctx = p_InFilterCtx;
	output->next = nullptr;
	output->pad_idx = 0;

	input->name = av_strdup(m_OutName);
	input->filter_ctx = p_OutFilterCtx;
	input->next = nullptr;
	input->pad_idx = 0;

	result = avfilter_graph_parse_ptr(p_avFilterGraph, m_FilterDesc, &input, &output, nullptr);
	if (result < 0) {
		LOGCATE("AudioSingleInputFilter::InitInOutputs..avfilter_graph_parse2 Failed ret = %s", av_err2str(result));
		return result;
	}

	result = avfilter_graph_config(p_avFilterGraph, nullptr);
	if (result < 0) {
		LOGCATE("AudioSingleInputFilter::InitInOutputs..avfilter_graph_config Failed ret = %s", av_err2str(result));
		return result;
	}

	avfilter_inout_free(&input);
	avfilter_inout_free(&output);

	char *tmp = avfilter_graph_dump(p_avFilterGraph, nullptr);

	return result;
}

int AudioSingleInputFilter::BufferSrcAddFrame() {
	int result = 0;
	int ret = 0;

	AVFrame *frame = av_frame_alloc();

	do {
		if ((av_audio_fifo_size(p_AudioFifo) < m_Input_FrameSize && m_InputOneExit)) {
			result = -1;
//			LOGCATE("AudioSingleInputFilter::BufferSrcAddFrame..Input Exit..");
			break;
		}
		while (av_audio_fifo_size(p_AudioFifo) < m_Input_FrameSize) {
			usleep( 5 * 1000);
		}
		frame->nb_samples = m_Input_FrameSize;
		frame->sample_rate = m_Input_SampleRate;
		frame->format = m_Input_SampleFmt;
		frame->channel_layout = m_Input_ChannelLayout;

		ret = av_frame_get_buffer(frame, 0);
		if (ret != 0) {
			result = -1;
			LOGCATE("AudioSingleInputFilter::BufferSrcAddFrame..av_frame_get_buffer(frameOne)..Failed");
			break;
		}

		unique_lock<std::mutex> lk_one(m_MutexRead);
		ret = av_audio_fifo_read(p_AudioFifo, reinterpret_cast<void **>(frame->data), frame->nb_samples);
		if (ret <= 0) {
			LOGCATE("AudioSingleInputFilter::BufferSrcAddFrame..av_audio_fifo_read(p_AudioFifo) failed, ret = %s", av_err2str(ret));
			result = -1;
			break;
		}
		lk_one.unlock();

		ret = av_buffersrc_add_frame(p_InFilterCtx, frame);
		if (ret < 0) {
			LOGCATE("AudioSingleInputFilter::BufferSrcAddFrame..av_buffersrc_add_frame() failed, ret = %s", av_err2str(ret));
			result = -1;
			break;
		}
	} while (false);

	if (frame) {
		av_frame_free(&frame);
	}

	return result;
}

void AudioSingleInputFilter::UnInitFilterCtx() {
	if (p_InFilterCtx != nullptr) {
		avfilter_free(p_InFilterCtx);
		p_InFilterCtx = nullptr;
	}

	if (p_AudioFifo != nullptr) {
		av_audio_fifo_free(p_AudioFifo);
		p_AudioFifo = nullptr;
	}
}

int AudioSingleInputFilter::InitInFilterOne(AVCodecContext *codecCtx) {
	int result = -1;

	if (codecCtx == nullptr) {
		result = -1;
		LOGCATE("AudioSingleInputFilter::InitInFilter..Failed..codecCtx is Null");
		return result;
	}
	char args[512];
	if (!codecCtx->channel_layout)
		codecCtx->channel_layout = av_get_default_channel_layout(codecCtx->channels);

	snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
			 codecCtx->time_base.num, codecCtx->time_base.den, codecCtx->sample_rate,
			 av_get_sample_fmt_name(codecCtx->sample_fmt), codecCtx->channel_layout);

	const AVFilter *inFilter = avfilter_get_by_name("abuffer");
	if (inFilter == nullptr) {
		LOGCATE("AudioSingleInputFilter::InitInFilter avfilter_get_by_name(abuffer) Failed");
		result = -1;
		return result;
	}

	result = avfilter_graph_create_filter(&p_InFilterCtx, inFilter, m_InFilterName, args,nullptr, p_avFilterGraph);
	if (result < 0) {
		LOGCATE("AudioSingleInputFilter::InitInFilter avfilter_graph_create_filter Failed, ret = %s", av_err2str(result));
		return result;
	}

	p_AudioFifo = av_audio_fifo_alloc(codecCtx->sample_fmt, codecCtx->channels, 30 * codecCtx->frame_size);
	if (p_AudioFifo == nullptr) {
		LOGCATE("AudioSingleInputFilter::InitInFilter av_audio_fifo_alloc failed");
		result = -1;
		return result;
	}

	m_Input_SampleFmt = codecCtx->sample_fmt;
	m_Input_ChannelLayout = codecCtx->channel_layout;
	m_Input_SampleRate = codecCtx->sample_rate;
	m_Input_FrameSize = codecCtx->frame_size;

	m_InFilterReady = result;

	return result;
}

int AudioSingleInputFilter::OnInputOneFrameAvailable(AVFrame *frame) {
	if (frame && p_AudioFifo) {
		std::lock_guard<std::mutex> lk(m_MutexWrite);
		int result = av_audio_fifo_write(p_AudioFifo, reinterpret_cast<void **>(frame->data), frame->nb_samples);
		if (result < 0) {
			LOGCATE("AudioSingleInputFilter::OnInputFrameAvailable..av_audio_fifo_write Faile, ret = %s", av_err2str(result));
			return result;
		}
		return 0;
	}

	return -1;
}

void AudioSingleInputFilter::TryFlushFrameWithoutFilter() {}