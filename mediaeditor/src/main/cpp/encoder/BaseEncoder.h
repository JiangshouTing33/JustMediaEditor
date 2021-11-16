//
// Created by 江守亭 on 2021/9/27.
//

#ifndef JUSTMEDIAEDITOR_BASEENCODER_H
#define JUSTMEDIAEDITOR_BASEENCODER_H

extern "C" {
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}
#include <LogUtil.h>
#include <thread>

typedef void (*EncodeMessageCallback) (void *, int, void *);
typedef void (*EncodeTimeStampCallback) (void *, double);
using namespace std;

enum EncodeStatus {
    ENCODE_STATUS_UNKNOWN,
    ENCODE_STATUS_ENCODING,
    ENCODE_STATUS_WAIT_SYNC,
    ENCODE_STATUS_PAUSE,
    ENCODE_STATUS_STOP
};

enum EncodeMessage {
    ENCODE_MESSAGE_WAIT_SYNC,
    ENCODE_MESSAGE_VIDEO_QUEUE_FULL,
    ENCODE_MESSAGE_DONE
};

class BaseEncoder {
public:
    BaseEncoder(AVFormatContext *avFormatContext, AVMediaType avMediaType, AVCodecID avCodecId);
    ~BaseEncoder() {}
    void Start();
    void Encoding();
    void Pause();
    void Stop();
    void RequestSync();
    void InputDone() {
        m_InputExit = true;
    }
    void SetEncodeMessageCallback(void *ctx, EncodeMessageCallback encodeMessageCallback) {
        m_MessageContext = ctx;
        m_EncodeMessageCallback = encodeMessageCallback;
    }

    void SetEncodeTimeStampCallback(void *ctx, EncodeTimeStampCallback encodeTimeStampCallback) {
        m_TimeStampContext = ctx;
        m_EncodeTimeStampCallback = encodeTimeStampCallback;
    }

    double GetNextPresentationTimeStamp() {
        return m_NextTimeStamp;
    }

    void SetCoverImageDuration(double duration);

    int Init();
    void UnInit();
protected:
    virtual int InitFrameQueue() = 0;
    virtual void UpdateAVCodecContextParam() = 0;
    virtual int InitFrame() = 0;
    virtual void UnInitEncoder() = 0;
    virtual AVFrame *GetFrameFromQueue() = 0;
    virtual void OnEncoderDone();
    virtual void ReleaseFrameRes();

protected:
    AVCodecContext *p_AVCdcCtx = nullptr;
    volatile bool m_InputExit = false;
    volatile uint64_t m_NextPts = 0;
    bool m_NeedSync = false;

    void *m_MessageContext = nullptr;
    EncodeMessageCallback m_EncodeMessageCallback = nullptr;
    mutex m_Mutex;
    mutex m_CondMutex;
    condition_variable m_Cond;
    double m_CoverImageDuration = 0;
    AVStream *p_AVStream = nullptr;

private:
    int InitFFEncoder();
    void UnInitFFEncoder();
    void ChangeStatus(EncodeStatus encodeStatus);
    void EncodingLoop();
    int EncodeOneFrame();
    void WaitSync();
    static void DoAVEncoding(BaseEncoder *baseEncoder);

private:
    AVFormatContext *p_AVFmtCtx = nullptr;
    AVMediaType m_AVMediaType = AVMEDIA_TYPE_UNKNOWN;
    AVCodecID m_AVCodecID;

    thread *p_Thread = nullptr;
    volatile int m_EncoderStatus = ENCODE_STATUS_UNKNOWN;

    void *m_TimeStampContext = nullptr;
    EncodeTimeStampCallback m_EncodeTimeStampCallback = nullptr;

    volatile double m_NextTimeStamp = 0;
    volatile int m_InitFFEncoderResult = -1;
};


#endif //JUSTMEDIAEDITOR_BASEENCODER_H
