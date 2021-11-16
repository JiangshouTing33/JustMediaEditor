//
// Created by 江守亭 on 2021/10/13.
//

#include "AudioDoubleInputFilter.h"
int AudioDoubleInputFilter::InitInFilterCtx() {
	if (m_InFilterOneInitResult == 0 && m_InFilterTwoInitResult == 0) {
		return 0;
	}
	LOGCATE("AudioDoubleInputFilter::InitInFilterCtx.Failed. m_InFilterOneInitResult = %d, m_InFilterTwoInitResult = %d",
			m_InFilterOneInitResult, m_InFilterTwoInitResult);
	return -1;
}

int AudioDoubleInputFilter::InitInOutputs() {
	int result = - 1;
	if (m_InFilterOneInitResult || m_InFilterTwoInitResult) {
		LOGCATE("AudioDoubleInputFilter::InitInOutputs..Failed, m_InFilterOneInitResult = %d, m_InFilterTwoInitResult = %d",
				m_InFilterOneInitResult, m_InFilterTwoInitResult);
		result = -1;
		return result;
	}

	p_InputOne = avfilter_inout_alloc();
	AVFilterInOut *p_OutputsOne = avfilter_inout_alloc();
	AVFilterInOut *p_OutputsTwo = avfilter_inout_alloc();

	if (p_InputOne == nullptr
		|| p_OutputsOne == nullptr
		|| p_OutputsTwo == nullptr) {
		result = -1;
		LOGCATE("AudioDoubleInputFilter::InitInOutputs..avfilter_inout_alloc Failed");
		return result;
	}

	p_OutputsOne->name = av_strdup(m_InputOneName);
	p_OutputsOne->filter_ctx = p_InFilterOneCtx;
	p_OutputsOne->next = p_OutputsTwo;
	p_OutputsOne->pad_idx = 0;

	p_OutputsTwo->name = av_strdup(m_InputTwoName);
	p_OutputsTwo->filter_ctx = p_InFilterTwoCtx;
	p_OutputsTwo->next = nullptr;
	p_OutputsTwo->pad_idx = 0;

	p_InputOne->name = av_strdup(m_OutName);
	p_InputOne->pad_idx = 0;
	p_InputOne->filter_ctx = p_OutFilterCtx;
	p_InputOne->next = nullptr;

	AVFilterInOut *outputs[2];
	outputs[0] = p_OutputsOne;
	outputs[1] = p_OutputsTwo;

	if (p_avFilterGraph == nullptr || strlen(m_FilterDesc) == 0 ) {
		LOGCATE("AudioDoubleInputFilter::InitFilter..PreCheck Failed!");
		return -1;
	}

	result = avfilter_graph_parse_ptr(p_avFilterGraph, m_FilterDesc, &p_InputOne, outputs, nullptr);
	if (result < 0) {
		LOGCATE("AudioDoubleInputFilter::InitFilter..avfilter_graph_parse2 Failed ret = %s", av_err2str(result));
		return result;
	}

	result = avfilter_graph_config(p_avFilterGraph, nullptr);
	if (result < 0) {
		LOGCATE("AudioDoubleInputFilter::InitFilter..avfilter_graph_config Failed ret = %s", av_err2str(result));
		return result;
	}

	avfilter_inout_free(&p_InputOne);
	avfilter_inout_free(outputs);

	char *tmp = avfilter_graph_dump(p_avFilterGraph, nullptr);
	LOGCATD("AudioDoubleInputFilter::InitFilter::avfilter_graph_dump..tmp = %s", tmp);

	return 0;
}

int AudioDoubleInputFilter::BufferSrcAddFrame() {
	int result = 0;
	int ret = 0;

	AVFrame *frameOne = av_frame_alloc();
	AVFrame *frameTwo = av_frame_alloc();

	do {
		if ((av_audio_fifo_size(p_AudioFifoOne) < m_InputOne_FrameSize && m_InputOneExit)
			|| (av_audio_fifo_size(p_AudioFifoTwo) < m_InputTwo_FrameSize && m_InputTwoExit)) {
			result = -1;
//			LOGCATE("AudioDoubleInputFilter::BufferSrcAddFrame..One Src Exit.");
			break;
		}

		while (av_audio_fifo_size(p_AudioFifoOne) < m_InputOne_FrameSize
			   || av_audio_fifo_size(p_AudioFifoTwo) < m_InputTwo_FrameSize) {
			usleep( 5 * 1000);
		}
		frameOne->nb_samples = m_InputOne_FrameSize;
		frameOne->sample_rate = m_InputOne_SampleRate;
		frameOne->format = m_InputOne_SampleFmt;
		frameOne->channel_layout = m_InputOne_ChannelLayout;
		ret = av_frame_get_buffer(frameOne, 0);
		if (ret != 0) {
			result = -1;
			LOGCATE("AudioDoubleInputFilter::BufferSrcAddFrame..av_frame_get_buffer(frameOne)..Failed");
			break;
		}

		frameTwo->nb_samples = m_InputTwo_FrameSize;
		frameTwo->sample_rate = m_InputTwo_SampleRate;
		frameTwo->format = m_InputTwo_SampleFmt;
		frameTwo->channel_layout = m_InputTwo_ChannelLayout;
		ret = av_frame_get_buffer(frameTwo, 0);
		if (ret != 0) {
			result = -1;
			LOGCATE("AudioDoubleInputFilter::BufferSrcAddFrame..av_frame_get_buffer(frameTwo)..Failed");
			break;
		}

		unique_lock<std::mutex> lk_one(m_MutexReadOne);
		ret = av_audio_fifo_read(p_AudioFifoOne, reinterpret_cast<void **>(frameOne->data), frameOne->nb_samples);
		if (ret <= 0) {
			LOGCATE("AudioDoubleInputFilter::BufferSrcAddFrame..av_audio_fifo_read(p_AudioFifoOne) failed, ret = %s", av_err2str(ret));
			result = -1;
			break;
		}
		lk_one.unlock();

		unique_lock<std::mutex> lk_two(m_MutexReadTwo);
		ret = av_audio_fifo_read(p_AudioFifoTwo, reinterpret_cast<void **>(frameTwo->data), frameTwo->nb_samples);
		if (ret <= 0) {
			LOGCATE("AudioDoubleInputFilter::BufferSrcAddFrame..av_audio_fifo_read(p_AudioFifoTwo) failed, ret = %s", av_err2str(ret));
			result = -1;
			break;
		}
		lk_two.unlock();

		ret = av_buffersrc_add_frame(p_InFilterOneCtx, frameOne);
		if (ret < 0) {
			LOGCATE("AudioDoubleInputFilter::MixFilterInner..av_buffersrc_add_frame(frameOne) failed, ret = %s", av_err2str(ret));
			result = -1;
			break;
		}

		ret = av_buffersrc_add_frame(p_InFilterTwoCtx, frameTwo);
		if (ret < 0) {
			LOGCATE("AudioDoubleInputFilter::MixFilterInner..av_buffersrc_add_frame(frameTwo) failed, ret = %s", av_err2str(ret));
			result = -1;
			break;
		}
	} while (false);

	if (frameOne) {
		av_frame_free(&frameOne);
	}
	if (frameTwo) {
		av_frame_free(&frameTwo);
	}

	return result;
}

int AudioDoubleInputFilter::InitInFilterOne(AVCodecContext *codecCtx) {
	char args[512];
	int result = -1;
	if (codecCtx == nullptr) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterOne, param AVCodecContext cant be Null!");
		result = -1;
		return result;
	}

	if (!codecCtx->channel_layout)
		codecCtx->channel_layout = av_get_default_channel_layout(codecCtx->channels);

	snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
			 codecCtx->time_base.num, codecCtx->time_base.den == 0 ? codecCtx->sample_rate : codecCtx->time_base.den, codecCtx->sample_rate,
			 av_get_sample_fmt_name(codecCtx->sample_fmt), codecCtx->channel_layout);

	const AVFilter *p_InFilterOne = avfilter_get_by_name("abuffer");
	if (p_InFilterOne == nullptr) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterOne avfilter_get_by_name(abuffer) Failed");
		result = -1;
		return result;
	}

	result = avfilter_graph_create_filter(&p_InFilterOneCtx, p_InFilterOne, m_InputOneName, args,
										  nullptr, p_avFilterGraph);
	if (result < 0) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterOne avfilter_graph_create_filter Failed, ret = %s", av_err2str(result));
		return result;
	}

	p_AudioFifoOne = av_audio_fifo_alloc(codecCtx->sample_fmt, codecCtx->channels, 30 * codecCtx->frame_size);
	if (p_AudioFifoOne == nullptr) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterOne av_audio_fifo_alloc failed");
		result = -1;
		return result;
	}

	m_InputOne_SampleFmt = codecCtx->sample_fmt;
	m_InputOne_ChannelLayout = codecCtx->channel_layout;
	m_InputOne_SampleRate = codecCtx->sample_rate;
	m_InputOne_FrameSize = codecCtx->frame_size;

	m_InFilterOneInitResult = result;
	return m_InFilterOneInitResult;
}

int AudioDoubleInputFilter::InitInFilterTwo(AVCodecContext *codecCtx) {
	char args[512];
	int result = -1;
	if (codecCtx == nullptr) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterTwo, param AVCodecContext cant be Null!");
		result = -1;
		return result;
	}

	if (!codecCtx->channel_layout)
		codecCtx->channel_layout = av_get_default_channel_layout(codecCtx->channels);

	snprintf(args, sizeof(args), "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
			 codecCtx->time_base.num, codecCtx->time_base.den == 0 ? codecCtx->sample_rate : codecCtx->time_base.den, codecCtx->sample_rate,
			 av_get_sample_fmt_name(codecCtx->sample_fmt), codecCtx->channel_layout);

	const AVFilter *p_InFilterTwo = avfilter_get_by_name("abuffer");
	if (p_InFilterTwo == nullptr) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterTwo avfilter_get_by_name(abuffer) Failed");
		result = -1;
		return result;
	}

	result = avfilter_graph_create_filter(&p_InFilterTwoCtx, p_InFilterTwo, m_InputTwoName, args,
										  nullptr, p_avFilterGraph);

	if (result < 0) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterTwo avfilter_graph_create_filter Failed, ret = %s", av_err2str(result));
		return result;
	}

	p_AudioFifoTwo = av_audio_fifo_alloc(codecCtx->sample_fmt, codecCtx->channels, 30 * codecCtx->frame_size);
	if (p_AudioFifoTwo == nullptr) {
		LOGCATE("AudioDoubleInputFilter::InitInFilterTwo av_audio_fifo_alloc failed");
		result = -1;
		return result;
	}

	m_InputTwo_SampleFmt = codecCtx->sample_fmt;
	m_InputTwo_ChannelLayout = codecCtx->channel_layout;
	m_InputTwo_SampleRate = codecCtx->sample_rate;
	m_InputTwo_FrameSize = codecCtx->frame_size;

	m_InFilterTwoInitResult = result;
	return m_InFilterTwoInitResult;
}

int AudioDoubleInputFilter::OnInputOneFrameAvailable(AVFrame *frame) {
	std::lock_guard<std::mutex> lk(m_MutexWriteOne);
	return OnInputFrameAvailable(frame, p_AudioFifoOne);
}

int AudioDoubleInputFilter::OnInputTwoFrameAvailable(AVFrame *frame) {
	std::lock_guard<std::mutex> lk(m_MutexWriteTwo);
	return OnInputFrameAvailable(frame, p_AudioFifoTwo);
}

int AudioDoubleInputFilter::OnInputFrameAvailable(AVFrame *frame, AVAudioFifo *audioFifo) {
	if (frame && audioFifo) {
		int result = av_audio_fifo_write(audioFifo, reinterpret_cast<void **>(frame->data), frame->nb_samples);
		if (result < 0) {
			LOGCATE("AudioDoubleInputFilter::OnInputOneFrameReady..av_audio_fifo_write Faile, ret = %s", av_err2str(result));
			return result;
		}

		return 0;
	}

	return -1;
}

void AudioDoubleInputFilter::UnInitFilterCtx() {
	if (p_InFilterOneCtx != nullptr) {
		avfilter_free(p_InFilterOneCtx);
		p_InFilterOneCtx = nullptr;
	}

	if (p_InFilterTwoCtx != nullptr) {
		avfilter_free(p_InFilterTwoCtx);
		p_InFilterTwoCtx = nullptr;
	}

	if (p_OutFilterCtx != nullptr) {
		avfilter_free(p_OutFilterCtx);
		p_OutFilterCtx = nullptr;
	}

	if (p_AudioFifoOne != nullptr) {
		av_audio_fifo_free(p_AudioFifoOne);
		p_AudioFifoOne = nullptr;
	}

	if (p_AudioFifoTwo != nullptr) {
		av_audio_fifo_free(p_AudioFifoTwo);
		p_AudioFifoTwo = nullptr;
	}
}

void AudioDoubleInputFilter::TryFlushFrameWithoutFilter() {
	while (!m_FilterExit) {
		if ((av_audio_fifo_size(p_AudioFifoOne) > m_InputOne_FrameSize || !m_InputOneExit)
			&& (av_audio_fifo_size(p_AudioFifoTwo) > m_InputTwo_FrameSize || !m_InputTwoExit)) {
			break;
		}

		if ((av_audio_fifo_size(p_AudioFifoOne) < m_InputOne_FrameSize && m_InputOneExit)
			&& (av_audio_fifo_size(p_AudioFifoTwo) < m_InputTwo_FrameSize && m_InputTwoExit)) {
			break;
		}

		if (av_audio_fifo_size(p_AudioFifoOne) < m_InputOne_FrameSize && m_InputOneExit) {
			AVFrame *frameTwo = av_frame_alloc();
			frameTwo->nb_samples = m_InputTwo_FrameSize;
			frameTwo->sample_rate = m_InputTwo_SampleRate;
			frameTwo->format = m_InputTwo_SampleFmt;
			frameTwo->channel_layout = m_InputTwo_ChannelLayout;
			if (av_frame_get_buffer(frameTwo, 0) != 0) {
				LOGCATE("AudioDoubleInputFilter::OutputFrameWithoutFilter..av_frame_get_buffer(frameTwo)..Failed");
				av_frame_free(&frameTwo);
				break;
			}

			unique_lock<std::mutex> lk_two(m_MutexReadTwo);
			int ret = av_audio_fifo_read(p_AudioFifoTwo, reinterpret_cast<void **>(frameTwo->data), frameTwo->nb_samples);
			if (ret < 0) {
//				LOGCATE("AudioDoubleInputFilter::OutputFrameWithoutFilter..av_audio_fifo_read(p_AudioFifoTwo) failed, ret = %s", av_err2str(ret));
				av_frame_free(&frameTwo);
				break;
			}

			if (frameTwo->format != DEFAULT_FILTER_SAMPLE_FMT) {
				frameTwo = AudioConvertFormat(frameTwo);
			}

			if (frameTwo == nullptr) {
				break;
			}
			if (m_OnAudioFilterMessageCallback && m_FilterMessageCallbackCtx) {
				m_OnAudioFilterMessageCallback(m_FilterMessageCallbackCtx, FilterMessage::FILTER_MESSAGE_FRAME, frameTwo);
			}

			if (frameTwo) {
				av_frame_free(&frameTwo);
			}
			lk_two.unlock();
		} else if (av_audio_fifo_size(p_AudioFifoTwo) < m_InputTwo_FrameSize && m_InputTwoExit) {
			AVFrame *frameOne = av_frame_alloc();
			frameOne->nb_samples = m_InputOne_FrameSize;
			frameOne->sample_rate = m_InputOne_SampleRate;
			frameOne->format = m_InputOne_SampleFmt;
			frameOne->channel_layout = m_InputOne_ChannelLayout;
			if (av_frame_get_buffer(frameOne, 0) != 0) {
				LOGCATE("AudioDoubleInputFilter::OutputFrameWithoutFilter..av_frame_get_buffer(frameOne)..Failed");
				av_frame_free(&frameOne);
				break;
			}

			unique_lock<std::mutex> lk_one(m_MutexReadOne);
			int ret = av_audio_fifo_read(p_AudioFifoOne, reinterpret_cast<void **>(frameOne->data), frameOne->nb_samples);
			if (ret < 0) {
				LOGCATE("AudioDoubleInputFilter::BufferSrcAddFrame..av_audio_fifo_read(p_AudioFifoOne) failed, ret = %s", av_err2str(ret));
				av_frame_free(&frameOne);
				break;
			}
			if (frameOne->format != DEFAULT_FILTER_SAMPLE_FMT) {
				frameOne = AudioConvertFormat(frameOne);
			}

			if (frameOne == nullptr) {
				break;
			}
			if (m_OnAudioFilterMessageCallback && m_FilterMessageCallbackCtx) {
				m_OnAudioFilterMessageCallback(m_FilterMessageCallbackCtx, FilterMessage::FILTER_MESSAGE_FRAME, frameOne);
			}

			if (frameOne) {
				av_frame_free(&frameOne);
			}
			lk_one.unlock();
		}
	}
}

AVFrame *AudioDoubleInputFilter::AudioConvertFormat(AVFrame *frame) {
	if (frame == nullptr) return nullptr;

	if (frame->format == DEFAULT_FILTER_SAMPLE_FMT) return frame;

	int ret;

	if (p_SwrContext == nullptr) {
		p_SwrContext = swr_alloc();

		if (p_SwrContext == nullptr) {
			LOGCATE("AudioDoubleInputFilter::AudioConvertFormat..error, swr_alloc failed");
			return nullptr;
		}

		av_opt_set_channel_layout(p_SwrContext, "in_channel_layout", frame->channel_layout, 0);
		av_opt_set_channel_layout(p_SwrContext, "out_channel_layout", DEFAULT_FILTER_CHANNEL_LAYOUT, 0);
		av_opt_set_int(p_SwrContext, "in_sample_rate", frame->sample_rate, 0);
		av_opt_set_int(p_SwrContext, "out_sample_rate", DEFAULT_FILTER_SAMPLE_RATE, 0);
		av_opt_set_sample_fmt(p_SwrContext, "in_sample_fmt", AVSampleFormat(frame->format), 0);
		av_opt_set_sample_fmt(p_SwrContext, "out_sample_fmt", DEFAULT_FILTER_SAMPLE_FMT, 0);

		ret = swr_init(p_SwrContext);
		if (ret < 0) {
			LOGCATE("AudioDoubleInputFilter::AudioConvertFormat..error, swr_init failed, ret = %s", av_err2str(ret));
			return nullptr;
		}
	}

	AVFrame *resultFrame = av_frame_alloc();
	resultFrame->format = DEFAULT_FILTER_SAMPLE_FMT;
	resultFrame->nb_samples = DEFAULT_FILTER_FRAME_SIZE;
	resultFrame->sample_rate = DEFAULT_FILTER_SAMPLE_RATE;
	resultFrame->channel_layout = DEFAULT_FILTER_CHANNEL_LAYOUT;

	if (av_frame_get_buffer(resultFrame, 0) != 0) {
		LOGCATE("AudioDoubleInputFilter::AudioConvertFormat..av_frame_get_buffer()..Failed");
		av_frame_free(&resultFrame);
		return nullptr;
	}

	ret = swr_convert(p_SwrContext, resultFrame->data, resultFrame->nb_samples,
					  (const uint8_t **)(frame->data), frame->nb_samples);

	if (ret < 0) {
		LOGCATE("AudioDoubleInputFilter::AudioConvertFormat..error, swr_convert failed, ret = %d", ret);
		return nullptr;
	}

	av_frame_free(&frame);

	return resultFrame;
}