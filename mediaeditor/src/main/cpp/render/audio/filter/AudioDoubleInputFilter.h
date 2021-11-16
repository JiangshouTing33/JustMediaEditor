//
// Created by 江守亭 on 2021/10/13.
//

#ifndef JUSTMEDIAEDITOR_AUDIODOUBLEINPUTFILTER_H
#define JUSTMEDIAEDITOR_AUDIODOUBLEINPUTFILTER_H

#include <BaseAudioFilter.h>
class AudioDoubleInputFilter : public BaseAudioFilter {
public:
	AudioDoubleInputFilter(const char* filterDesc, const char* inputName1, const char* inputName2, const char* outputName) : BaseAudioFilter(outputName) {
		strcpy(m_FilterDesc, filterDesc);
		strcpy(m_InputOneName, inputName1);
		strcpy(m_InputTwoName, inputName2);
	}
	~AudioDoubleInputFilter() {}
	int InitInFilterTwo(AVCodecContext *codecCtx);
	int OnInputTwoFrameAvailable(AVFrame *frame);
	void OnInputTwoExit() {
		m_InputTwoExit = true;
	}

	virtual int InitInFilterOne(AVCodecContext *codecCtx);
	virtual int OnInputOneFrameAvailable(AVFrame *frame);
	virtual int InitInFilterCtx();
	virtual int InitInOutputs();
	virtual int BufferSrcAddFrame();
	virtual void UnInitFilterCtx();
	int OnInputFrameAvailable(AVFrame *frame, AVAudioFifo *audioFifo);

private:
	virtual void TryFlushFrameWithoutFilter();
	AVFrame *AudioConvertFormat(AVFrame *frame);

private:
	AVFilterContext *p_InFilterOneCtx = nullptr;
	AVFilterContext *p_InFilterTwoCtx = nullptr;

	char m_FilterDesc[FILTER_DESC_MAX_LENGTH];
	char m_InputOneName[FILTER_NAME_MAX_LENGTH];
	char m_InputTwoName[FILTER_NAME_MAX_LENGTH];

	AVAudioFifo *p_AudioFifoOne = nullptr;
	AVAudioFifo *p_AudioFifoTwo = nullptr;

	AVFilterInOut *p_InputOne = nullptr;

	SwrContext *p_SwrContext = nullptr;

	std::mutex m_MutexWriteOne;
	std::mutex m_MutexWriteTwo;

	std::mutex m_MutexReadOne;
	std::mutex m_MutexReadTwo;

	int m_InputOne_SampleFmt = 0;
	int m_InputOne_ChannelLayout = 0;
	int m_InputOne_SampleRate = 0;
	int m_InputOne_FrameSize = 0;

	int m_InputTwo_SampleFmt = 0;
	int m_InputTwo_ChannelLayout = 0;
	int m_InputTwo_SampleRate = 0;
	int m_InputTwo_FrameSize = 0;

	volatile int m_InFilterOneInitResult = -1;
	volatile int m_InFilterTwoInitResult = -1;

	volatile bool m_InputTwoExit = false;
};


#endif //JUSTMEDIAEDITOR_AUDIODOUBLEINPUTFILTER_H
