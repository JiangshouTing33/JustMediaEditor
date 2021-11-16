//
// Created by 江守亭 on 2021/10/25.
//

#ifndef JUSTMEDIAEDITOR_MANUALVIDEODECODER_H
#define JUSTMEDIAEDITOR_MANUALVIDEODECODER_H

#define PICTURE_MAX_SIZE 20
#define DEFAULT_IMAGE_SIZE_ONE_SECOND 30
#define DEFAULT_PER_IMAGE_DURATION 3
#include <VideoBaseDecoder.h>
#include <thread>
#include <TransitionRenderSample.h>

using namespace std;
class ManualVideoDecoder: public VideoBaseDecoder {
public:
	ManualVideoDecoder(int imageSizeOneSecond, float perImageDuration);
	~ManualVideoDecoder();

	void SetImageSourceWithIndex(int index, int width, int height, uint8_t *pdata);
	void SetImageCount(int imageCount);
	void SetTransitionRenderSample(TransitionRenderSample *transitionRenderSample);

	void Start();
	void Stop();
	void Decoding();
	void Rending();
	void Pause();

private:
	void VideoDecoderLoop();
	void VideoRender();
	void VideoDecoderReady();
	void VideoDecodeDone();

	void VideoRenderCoverImage();
	void SetNextRenderImageData(int imageIndex, int renderInnerIndex);

	static void OnVideoRenderDone(void *ctx);
	static void StartVideoDecoder(ManualVideoDecoder *manualVideoDecoder);

private:
	FrameImage m_ImageRes[PICTURE_MAX_SIZE];
	std::mutex m_Mutex;
	condition_variable m_Cond;
	std::thread *m_VideoDecoderThread = nullptr;
	TransitionRenderSample *m_TransitionRenderSample = nullptr;

	int m_RenderImageIndex = 0;
	int m_RenderIndexInner = 0;
	int m_ImageCount = 0;
	float m_PerImageDuration = DEFAULT_PER_IMAGE_DURATION;
	int m_ImageSizeOneSecond = DEFAULT_IMAGE_SIZE_ONE_SECOND;
	int m_ImageDurationFrame = DEFAULT_IMAGE_SIZE_ONE_SECOND * DEFAULT_PER_IMAGE_DURATION;
};


#endif //JUSTMEDIAEDITOR_MANUALVIDEODECODER_H
