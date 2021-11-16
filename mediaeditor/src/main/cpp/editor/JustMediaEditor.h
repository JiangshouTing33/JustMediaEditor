//
// Created by 江守亭 on 2021/9/28.
//

#ifndef JUSTMEDIAEDITOR_JUSTMEDIAEDITOR_H
#define JUSTMEDIAEDITOR_JUSTMEDIAEDITOR_H

#include <jni.h>
#include <VideoDecoder.h>
#include <AudioDecoder.h>
#include <BaseMuxer.h>
#include <EGLVideoRender.h>
#include <DataFilterUtil.h>
#include <LutSample.h>
#include <AudioSingleInputFilter.h>
#include <AudioDoubleInputFilter.h>
#include <StickerRenderSample.h>
#include <DefaultFrameRenderSample.h>

#define JAVA_EDITOR_MESSAGE_CALLBACK_API_NAME "editorMessageCallback"

enum JustEditorMessageType {
    EDITOR_MESSAGE_TYPE_DURATION,
    EDITOR_MESSAGE_TYPE_CURRENT_TIME,
    EDITOR_MESSAGE_TYPE_DONE
};

class JustMediaEditor {
public:
    JustMediaEditor() {}
    ~JustMediaEditor() {}

    void Init(JNIEnv *jniEnv, jobject obj, const char *srcVideoUrl, const char* srcAudioUrl, const char *dstUrl);
    void UnInit();

    void Start();
    void Stop();
    void Pause();

    void SetCoverImage(int width, int height, const uint8_t* data, double duration);
	void SetImageMarkData(int width, int height, uint8_t *pdata, float markPositionX, float markPositionY);
	void AddTextDisplayParam(const wchar_t *text, const char *fontName, long startTime, long endTime,
							  float red, float green, float blue, float positionX, float positionY);
	void AddStickerDisplayOption(int width, int height, const uint8_t* data, long startTime, long endTime, float positionX, float positionY);
	void SetLutImageData(int width, int height, const uint8_t* data);
	void SetAudioFilterData(
		int audioFilterType, const char* desc,
		const char *inFilterNameOne, const char *inFilterNameTwo, const char *outFilterName);
	void SetBackgroundMusicData(const char* backgroundAudioUrl);
	void ChangeVideoSpeed(float videoSpeed);
	void SetMuxerParam(int width, int height, long bitRate);

private:
    JNIEnv *GetJNIEnv(bool *isAttach);
    void StartMuxer();
    void PostMessageToJava(int messageType, double message);
    void InitVideoRenderSample();
    void TryStartAudioFilter();

    static void OnVideoDecodeMessageCallback(void *ctx, int message, void *data);
    static void OnAudioDecodeMessageCallback(void *ctx, int message, void *data);
    static void OnMuxerMessageCallback(void *ctx, int message, void *data);
    static void OnVideoFrameAvailable(void *ctx, void* image);
    static void OnBackgroundAudioDecodeMessageCallback(void *ctx, int message, void *data);
    static void OnAudioFilterMessageCallback(void *ctx, int message, void *data);
private:
    VideoDecoder *m_VideoDecoder = nullptr;
    AudioDecoder *m_AudioDecoder = nullptr;
    BaseMuxer *m_BaseMuxer = nullptr;
	char m_DstUrl[URL_MAX_LENGTH];
	JavaVM *m_JavaVM = nullptr;
	jobject m_JavaObj = nullptr;
	bool m_VideoStreamNeedInit = false;
	bool m_AudioStreamNeedInit = false;
	int m_MuxerType = MUXER_TYPE_NONE;

    //Video Render
	EGLVideoRender *m_EGLVideoRender = nullptr;
	FrameRenderSample *frameRenderSample = nullptr;
	MarkRenderSample *m_MarkRenderSample = nullptr;
	FontRenderSample *m_FontRenderSample = nullptr;
	StickerRenderSample *m_StickerRenderSample = nullptr;
	FrameImage m_CoverImage;
	double m_CoverImageDuration = 0;
	FrameImage m_MarkImage;
	float m_MarkPositionX;
	float m_MarkPositionY;
	std::deque<TextDisplayParam *> m_TextDisplayParams;
	std::deque<StickerDisplayOption> m_StickerDisplayOptions;
	float m_SpeedFactor = 1.f;
	int m_DstWidth = 0;
	int m_DstHeight = 0;
	long m_DstBitRate = 0L;

	//Audio Render
	BaseAudioFilter *m_BaseAudioFilter = nullptr;
	AudioDecoder *m_BackgroundDecoder = nullptr;
	int m_AudioFilterType = AudioFilterType::TYPE_NONE;
	volatile bool m_AudioFilterOneInitSuccess = true;
	volatile bool m_AudioFilterTwoInitSuccess = true;
};


#endif //JUSTMEDIAEDITOR_JUSTMEDIAEDITOR_H
