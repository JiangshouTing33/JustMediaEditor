//
// Created by 江守亭 on 2021/9/25.
//

#ifndef JUSTMEDIAEDITOR_BASEDECODER_H
#define JUSTMEDIAEDITOR_BASEDECODER_H
extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}
#include <thread>
#include <LogUtil.h>
#define URL_MAX_LENGTH 4096
using namespace std;
typedef void (*DecodeMessageCallback) (void *, int status,  void *data);
enum DecodeStatus {
    DECODE_STATUS_UNKNOWN,
    DECODE_STATUS_INIT,
    DECODE_STATUS_READY,
    DECODE_STATUS_DECODING,
    DECODE_STATUS_RENDING,
    DECODE_STATUS_PAUSE,
    DECODE_STATUS_STOP
};

enum DecodeMessage {
    DECODE_MESSAGE_READY,
    DECODE_MESSAGE_FRAME,
    DECODE_MESSAGE_DURATION,
    DECODE_MESSAGE_DONE
};

class BaseDecoder {
public:
    BaseDecoder() {}
    ~BaseDecoder() {}
    void Start();
    void Stop();
    void Decoding();
    void Pause();
    void SetDecodeStatusChangeCallback(void *ctx, DecodeMessageCallback decodeMessageCallback) {
        m_MessageContext = ctx;
        m_DecodeMessageCallback = decodeMessageCallback;
    }
    AVCodecContext *GetAVCodecContext() {
        return p_AVCdcCtx;
    }
    AVStream *GetAVStream() {
        return p_AVStream;
    }
    void Init(const char *url, AVMediaType mediaType);
    void UnInit();
    void SetSpeedFactor(float speedFactor);

protected:
    virtual void OnDecoderReady() = 0;
    virtual void OnDecoderDone() = 0;
    virtual void OnFrameAvailable(AVFrame *avFrame) = 0;
    void Rending();
    static void OnRenderDone(void *ctx);

protected:
    void *m_MessageContext = nullptr;
    DecodeMessageCallback m_DecodeMessageCallback;
    AVFormatContext *p_AVfmtCtx = nullptr;
    float m_SpeedFactor = 1.f;
    AVCodecContext *p_AVCdcCtx = nullptr;

private:
    void StartDecodingThread();
    int InitFFDecoder();
    void UnInitFFDecoder();
    void DecodingLoop();
    int DecodeOnePacket();
    void ChangeStatus(DecodeStatus decodeStatus);
    void ReadyAndWaiting();
    static void DoAVDecoding(BaseDecoder *baseDecoder);

private:
    const AVCodec *p_AvCodec = nullptr;
    AVStream *p_AVStream = nullptr;
    AVFrame *p_AVFrame = nullptr;
    AVPacket *p_AVPacket = nullptr;
    int m_StreamIndex = -1;

    AVMediaType m_MediaType = AVMEDIA_TYPE_UNKNOWN;
    char m_Url[URL_MAX_LENGTH];

    thread *p_Thread = nullptr;
    volatile int m_DecoderStatus = DECODE_STATUS_UNKNOWN;
    mutex m_Mutex;
    condition_variable m_Cond;
};


#endif //JUSTMEDIAEDITOR_BASEDECODER_H
