//
// Created by 江守亭 on 2021/10/9.
//

#include <BaseSample.h>

void BaseSample::UpdateMVPMatrix(float translateX, float translateY, float scaleX, float scaleY) {
	mat4 Project = glm::ortho(-1.f, 1.f, -1.f, 1.f, 0.1f, 100.f);
	mat4 View = glm::lookAt(
		vec3(0, 0, 4),
		vec3(0,0,0),
		vec3(0,1,0));

	mat4 Model = mat4 (1.f);
	Model = glm::scale(Model, vec3(scaleX, scaleY, 1.f));
	Model = glm::translate(Model, vec3(translateX, translateY,0));

	m_MVPMatrix = Project * View * Model;
}

vec2 BaseSample::GetTrulyDisplayImageSize(int width, int height) {
	if (m_DstSize.x == 0 || m_DstSize.y == 0 || width == 0 || height == 0) {
		return vec2(width, height);
	}

	float dstWidth, dstHeight;

	float widthRatio = (float) m_DstSize.x / width;
	float heightRatio = (float) m_DstSize.y / height;
	if (widthRatio < heightRatio) {
		dstWidth = m_DstSize.x;
		dstHeight = height * widthRatio;
	} else {
		dstWidth = width * heightRatio;
		dstHeight = m_DstSize.y;
	}

	return vec2(dstWidth, dstHeight);
}