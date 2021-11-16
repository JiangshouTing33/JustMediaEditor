//
// Created by 江守亭 on 2021/10/9.
//

#include <BaseVideoRender.h>

void BaseVideoRender::SetRenderFrameData(FrameImage *image) {
	if (image->ppPlane[0]) {
		std::unique_lock<std::mutex> lock(m_Mutex);
		if (m_RenderImage.ppPlane[0]) {
			FrameImageUtil::FreeFrameImage(&m_RenderImage);
		}

		m_RenderImage.width = image->width;
		m_RenderImage.height = image->height;
		m_RenderImage.format = image->format;

		FrameImageUtil::AllocFrameImage(&m_RenderImage);
		FrameImageUtil::CopyFrameImage(&m_RenderImage, image);
		SetSampleFrameData(&m_RenderImage);
	}
}