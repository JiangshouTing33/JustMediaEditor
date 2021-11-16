//
// Created by 江守亭 on 2021/11/3.
//

#ifndef JUSTMEDIAEDITOR_DEFAULTFRAMERENDERSAMPLE_H
#define JUSTMEDIAEDITOR_DEFAULTFRAMERENDERSAMPLE_H

#include <FrameRenderSample.h>
class DefaultFrameRenderSample: public FrameRenderSample {
public:
	DefaultFrameRenderSample() {}
	~DefaultFrameRenderSample() {}

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();
};


#endif //JUSTMEDIAEDITOR_DEFAULTFRAMERENDERSAMPLE_H
