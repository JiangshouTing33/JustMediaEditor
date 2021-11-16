//
// Created by 江守亭 on 2021/10/11.
//

#ifndef JUSTMEDIAEDITOR_LUTSAMPLE_H
#define JUSTMEDIAEDITOR_LUTSAMPLE_H

#include <FrameRenderSample.h>
class LutSample : public FrameRenderSample {
public:
	LutSample();
	~LutSample();

	virtual int Init();
	virtual void UnInit();
	virtual void Draw();

	void SetLutImageData(FrameImage *lutImage);

	static LutSample *GetInstance();
private:
	GLuint m_LutImageTextureId;
	FrameImage *m_LutImage;

	static std::mutex m_Mutex;
	static LutSample* s_Instance;
};


#endif //JUSTMEDIAEDITOR_LUTSAMPLE_H
