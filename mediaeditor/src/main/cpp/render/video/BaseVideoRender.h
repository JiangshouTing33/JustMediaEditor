//
// Created by 江守亭 on 2021/10/8.
//

#ifndef JUSTMEDIAEDITOR_BASEVIDEORENDER_H
#define JUSTMEDIAEDITOR_BASEVIDEORENDER_H

#include <ImageDef.h>
#include <BaseRender.h>
#include <mutex>

typedef void (*OnRenderFrameCallback) (void*, void*) ;

enum VideoRenderType {
	RENDER_TYPE_SAMPLE,
	RENDER_TYPE_EGL
};
class BaseVideoRender : public BaseRender {
public:
    virtual int Init() = 0;
    virtual void UnInit() = 0;
    virtual void Draw() = 0;

	virtual void SetRenderFrameData(FrameImage *image);
protected:
	virtual void SetSampleFrameData(FrameImage *image) = 0;
	FrameImage m_RenderImage;
	std::mutex m_Mutex;
};

#endif //JUSTMEDIAEDITOR_BASEVIDEORENDER_H
