//
// Created by 江守亭 on 2021/10/12.
//

#ifndef JUSTMEDIAEDITOR_BASEAUDIOFILTER_H
#define JUSTMEDIAEDITOR_BASEAUDIOFILTER_H

#include <cstdint>
#include <thread>
#include <LogUtil.h>
#include <unistd.h>
extern "C" {
#include <libavfilter/avfilter.h>
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
#include <libswresample/swresample.h>
};

#define FILTER_DESC_MAX_LENGTH 5096
#define FILTER_NAME_MAX_LENGTH 128
#define DEFAULT_FILTER_SAMPLE_FMT AV_SAMPLE_FMT_FLTP
#define DEFAULT_FILTER_CHANNEL_LAYOUT AV_CH_LAYOUT_STEREO
#define DEFAULT_FILTER_SAMPLE_RATE 44100
#define DEFAULT_FILTER_FRAME_SIZE 1024
typedef void (*OnAudioFilterMessageCallback) (void*, int message, void*) ;
enum AudioFilterType {
	TYPE_NONE,
	TYPE_ONE_INPUT,
	TYPE_TWO_INPUT
};
enum FilterMessage {
	FILTER_MESSAGE_READY,
	FILTER_MESSAGE_FRAME,
	FILTER_MESSAGE_DONE
};
using namespace std;
class BaseAudioFilter {
public:
	BaseAudioFilter(const char *outName) : BaseAudioFilter(outName,
														   DEFAULT_FILTER_SAMPLE_FMT,
														   DEFAULT_FILTER_CHANNEL_LAYOUT,
														   DEFAULT_FILTER_SAMPLE_RATE) {}
	BaseAudioFilter(const char *outName, int sample_fmt, int64_t channel_layout, int sample_rate);
	virtual ~BaseAudioFilter();

	void Start();
	void Stop();

	virtual int InitInFilterOne(AVCodecContext *codecCtx) = 0;
	virtual int OnInputOneFrameAvailable(AVFrame *frame) = 0;
	virtual int InitInFilterCtx() = 0;
	virtual int InitInOutputs() = 0;
	virtual int BufferSrcAddFrame() = 0;
	virtual void UnInitFilterCtx() = 0;

	void SetAudioFilterMessageCallback(void *context, OnAudioFilterMessageCallback audioFilterMessageCallback) {
		m_FilterMessageCallbackCtx = context;
		m_OnAudioFilterMessageCallback = audioFilterMessageCallback;
	}

	void FilterStop() {
		m_FilterExit = true;
	}

	void OnInputOneExit() {
		m_InputOneExit = true;
	}

	int InitFilter();
	int FilterLoop();
	void UnInitFilter();

	virtual int InitOutFilterCtx();
	virtual void FilterReady();
	virtual void FilterDone();

protected:
	virtual void TryFlushFrameWithoutFilter() = 0;

protected:
	AVFilterGraph *p_avFilterGraph = nullptr;
	char m_OutName[FILTER_NAME_MAX_LENGTH];
	AVFilterContext *p_OutFilterCtx = nullptr;
	volatile bool m_InputOneExit = false;

	void *m_FilterMessageCallbackCtx = nullptr;
	OnAudioFilterMessageCallback m_OnAudioFilterMessageCallback;
	volatile bool m_FilterExit = false;
private:
	static void FilterAudio(BaseAudioFilter *audioFilter);

private:
	int m_OutSampleFmt = 0;
	int64_t m_OutChannelLayout = 0;
	int m_OutSampleRate = 0;

	std::thread *m_FilterThread = nullptr;
};


#endif //JUSTMEDIAEDITOR_BASEAUDIOFILTER_H
