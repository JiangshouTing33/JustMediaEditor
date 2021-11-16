//
// Created by 江守亭 on 2021/10/8.
//

#ifndef JUSTMEDIAEDITOR_BASERENDER_H
#define JUSTMEDIAEDITOR_BASERENDER_H

typedef void (*OnRenderFrameCallback) (void*, void*) ;
typedef void (*OnRenderDoneCallback)(void*) ;

class BaseRender {
public:
    virtual void SetRenderFrameCallback(void *ctx, OnRenderFrameCallback renderFrameCallback) {
        m_RenderFrameCtx = ctx;
        m_RenderFrameCallback = renderFrameCallback;
    }

    virtual void SetRenderDoneCallback(void* ctx, OnRenderDoneCallback renderDoneCallback) {
        m_RenderDoneCtx = ctx;
        m_RenderDoneCallback = renderDoneCallback;
    }

protected:
    OnRenderFrameCallback m_RenderFrameCallback = nullptr;
    void *m_RenderFrameCtx = nullptr;

    OnRenderDoneCallback m_RenderDoneCallback = nullptr;
    void *m_RenderDoneCtx = nullptr;
};

#endif //JUSTMEDIAEDITOR_BASERENDER_H
