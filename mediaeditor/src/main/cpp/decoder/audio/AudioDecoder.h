//
// Created by 江守亭 on 2021/9/26.
//

#ifndef JUSTMEDIAEDITOR_AUDIODECODER_H
#define JUSTMEDIAEDITOR_AUDIODECODER_H

#include <BaseDecoder.h>
class AudioDecoder : public BaseDecoder {
public:
    AudioDecoder(const char* url) {
        Init(url, AVMEDIA_TYPE_AUDIO);
    }

    ~AudioDecoder() {
        UnInit();
    }

private:
    virtual void OnDecoderReady();
    virtual void OnDecoderDone();
    virtual void OnFrameAvailable(AVFrame *avFrame);
};


#endif //JUSTMEDIAEDITOR_AUDIODECODER_H
