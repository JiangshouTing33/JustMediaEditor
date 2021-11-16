//
// Created by 江守亭 on 2021/10/25.
//

#ifndef JUSTMEDIAEDITOR_JUSTPIC2VIDEOEDITOR_H
#define JUSTMEDIAEDITOR_JUSTPIC2VIDEOEDITOR_H

#define JAVA_PIC_2_V_EDITOR_MESSAGE_CALLBACK_API_NAME "pic2VideoEditorMessageCallback"

#include <jni.h>
#include <ManualVideoDecoder.h>
#include <AudioDecoder.h>
#include <BaseMuxer.h>
#include <LutSample.h>

enum JustPic2VEditorMessageType {
	PIC_2_V_EDITOR_MESSAGE_TYPE_DURATION,
	PIC_2_V_EDITOR_MESSAGE_TYPE_CURRENT_TIME,
	PIC_2_V_EDITOR_MESSAGE_TYPE_DONE
};

class JustPic2VideoEditor {
public:
	JustPic2VideoEditor() {}
	~JustPic2VideoEditor() {}

	void Init(JNIEnv *jniEnv, jobject obj, const char* srcAudioUrl, const char *dstUrl, int dstWidth, int dstHeight, int fps, long videoBitRate);
	void UnInit();

	void Start();
	void Stop();
	void Pause();

	void SetImageSourceWithIndex(int index, int width, int height, uint8_t *pdata);
	void SetImageCount(int imageCount);
	void SetTransitionFShaderStr(const char *fShader);

	void SetCoverImage(int width, int height, const uint8_t* data, double duration);
	void SetImageMarkData(int width, int height, uint8_t *pdata, float markPositionX, float markPositionY);
	void AddTextDisplayParam(const wchar_t *text, const char *fontName, long startTime, long endTime,
							 float red, float green, float blue, float positionX, float positionY);
	void AddStickerDisplayOption(int width, int height, const uint8_t* data, long startTime, long endTime, float positionX, float positionY);

private:
	JNIEnv *GetJNIEnv(bool *isAttach);
	void StartMuxer();
	void PostMessageToJava(int messageType, double message);
	void InitVideoRenderSample();

	static void OnVideoDecodeMessageCallback(void *ctx, int message, void *data);
	static void OnAudioDecodeMessageCallback(void *ctx, int message, void *data);
	static void OnMuxerMessageCallback(void *ctx, int message, void *data);
	static void OnVideoFrameAvailable(void *ctx, void* image);

private:
	ManualVideoDecoder *m_VideoDecoder = nullptr;
	AudioDecoder *m_AudioDecoder = nullptr;
	BaseMuxer *m_BaseMuxer = nullptr;
	char m_DstUrl[URL_MAX_LENGTH];
	JavaVM *m_JavaVM = nullptr;
	jobject m_JavaObj = nullptr;
	bool m_VideoStreamNeedInit = true;
	bool m_AudioStreamNeedInit = true;
	int m_DstWidth = 0;
	int m_DstHeight = 0;
	int m_Fps = 0;
	long m_VideoBitRate = 0;

	//Video Render
	EGLVideoRender *m_EGLVideoRender = nullptr;
	MarkRenderSample *m_MarkRenderSample = nullptr;
	FontRenderSample *m_FontRenderSample = nullptr;
	StickerRenderSample *m_StickerRenderSample = nullptr;
	TransitionRenderSample *m_TransitionRenderSample = nullptr;

	FrameImage m_CoverImage;
	FrameImage m_MarkImage;
	float m_MarkPositionX;
	float m_MarkPositionY;
	std::deque<TextDisplayParam *> m_TextDisplayParams;
	std::deque<StickerDisplayOption> m_StickerDisplayOptions;
};


#endif //JUSTMEDIAEDITOR_JUSTPIC2VIDEOEDITOR_H
