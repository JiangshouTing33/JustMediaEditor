//
// Created by 江守亭 on 2021/10/13.
//

#ifndef JUSTMEDIAEDITOR_AUDIOSINGLEINPUTFILTER_H
#define JUSTMEDIAEDITOR_AUDIOSINGLEINPUTFILTER_H

#include <BaseAudioFilter.h>
class AudioSingleInputFilter : public BaseAudioFilter {
public:
	AudioSingleInputFilter(const char* desc, const char *inFilterName, const char *outFilterName) : BaseAudioFilter(outFilterName) {
		strcpy(m_FilterDesc, desc);
		strcpy(m_InFilterName, inFilterName);
	}
	~AudioSingleInputFilter() {}
	virtual int InitInFilterOne(AVCodecContext *codecCtx);

	virtual int InitInFilterCtx();
	virtual int OnInputOneFrameAvailable(AVFrame *frame);
	virtual int InitInOutputs();
	virtual int BufferSrcAddFrame();
	virtual void UnInitFilterCtx();

private:
	virtual void TryFlushFrameWithoutFilter();

private:
	char m_FilterDesc[FILTER_DESC_MAX_LENGTH];
	char m_InFilterName[FILTER_NAME_MAX_LENGTH];
	int m_InFilterReady = -1;

	AVFilterContext *p_InFilterCtx = nullptr;
	AVAudioFifo *p_AudioFifo = nullptr;

	std::mutex m_MutexRead;
	std::mutex m_MutexWrite;

	int m_Input_SampleFmt = 0;
	int m_Input_ChannelLayout = 0;
	int m_Input_SampleRate = 0;
	int m_Input_FrameSize = 0;
};


#endif //JUSTMEDIAEDITOR_AUDIOSINGLEINPUTFILTER_H
