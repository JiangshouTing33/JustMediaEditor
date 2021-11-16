//
// Created by 江守亭 on 2021/10/9.
//

#ifndef JUSTMEDIAEDITOR_PARTGRAYSAMPLE_H
#define JUSTMEDIAEDITOR_PARTGRAYSAMPLE_H

#include <FrameRenderSample.h>
using namespace std;
class PartGraySample : public FrameRenderSample {
public:
    PartGraySample();
    ~PartGraySample();

	static PartGraySample *GetInstance();
	static void ReleaseInstance();

    virtual int Init();
    virtual void UnInit();
    virtual void Draw();

private:
	static std::mutex m_Mutex;
	static PartGraySample* s_Instance;
};


#endif //JUSTMEDIAEDITOR_PARTGRAYSAMPLE_H
