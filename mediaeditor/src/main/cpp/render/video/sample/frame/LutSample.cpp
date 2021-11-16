//
// Created by 江守亭 on 2021/10/11.
//

#include "LutSample.h"
LutSample* LutSample::s_Instance = nullptr;
std::mutex LutSample::m_Mutex;
LutSample::LutSample() {
	m_LutImageTextureId = GL_NONE;
}

LutSample::~LutSample() {
	FrameImageUtil::FreeFrameImage(m_LutImage);
}

const char vShaderStr[] =
	"#version 300 es\n"
	"layout(location = 0) in vec4 a_position;\n"
	"layout(location = 1) in vec2 a_texCoord;\n"
	"uniform mat4 u_MVPMatrix;\n"
	"out vec2 v_texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = u_MVPMatrix * a_position;\n"
	"    v_texCoord = a_texCoord;\n"
	"}";

const char fShaderStr[] = "#version 300 es\n"
						  "precision highp float;\n"
						  "in vec2 v_texCoord;\n"
						  "layout(location = 0) out vec4 outColor;\n"
						  "uniform sampler2D s_texture0;\n"
						  "uniform sampler2D s_texture1;\n"
						  "uniform sampler2D s_texture2;\n"
						  "uniform sampler2D s_LutTexture; //LUT纹理采样器对象\n"
						  "uniform int u_nImgType;// 1:RGBA, 2:NV21, 3:NV12, 4:I420\n"
						  "\n"
						  "vec4 yuvToRgb(vec2 texCoord) {\n"
						  "    vec4 rgbColor;\n"
						  "    if(u_nImgType == 1) //RGBA\n"
						  "    {\n"
						  "        rgbColor = texture(s_texture0, texCoord);\n"
						  "    }\n"
						  "    else if(u_nImgType == 2) //NV21\n"
						  "    {\n"
						  "        vec3 yuv;\n"
						  "        yuv.x = texture(s_texture0, texCoord).r;\n"
						  "        yuv.y = texture(s_texture1, texCoord).a - 0.5;\n"
						  "        yuv.z = texture(s_texture1, texCoord).r - 0.5;\n"
						  "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
						  "        0.0, \t-0.344, \t1.770,\n"
						  "        1.403,  -0.714,     0.0) * yuv;\n"
						  "        rgbColor = vec4(rgb, 1.0);\n"
						  "    }\n"
						  "    else if(u_nImgType == 3) //NV12\n"
						  "    {\n"
						  "        vec3 yuv;\n"
						  "        yuv.x = texture(s_texture0, texCoord).r;\n"
						  "        yuv.y = texture(s_texture1, texCoord).r - 0.5;\n"
						  "        yuv.z = texture(s_texture1, texCoord).a - 0.5;\n"
						  "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
						  "        0.0, \t-0.344, \t1.770,\n"
						  "        1.403,  -0.714,     0.0) * yuv;\n"
						  "        rgbColor = vec4(rgb, 1.0);\n"
						  "    }\n"
						  "    else if(u_nImgType == 4) //I420\n"
						  "    {\n"
						  "        vec3 yuv;\n"
						  "        yuv.x = texture(s_texture0, texCoord).r;\n"
						  "        yuv.y = texture(s_texture1, texCoord).r - 0.5;\n"
						  "        yuv.z = texture(s_texture2, texCoord).r - 0.5;\n"
						  "        highp vec3 rgb = mat3(1.0,       1.0,     1.0,\n"
						  "        0.0, \t-0.344, \t1.770,\n"
						  "        1.403,  -0.714,     0.0) * yuv;\n"
						  "        rgbColor = vec4(rgb, 1.0);\n"
						  "    }\n"
						  "    else\n"
						  "    {\n"
						  "        rgbColor = vec4(1.0);\n"
						  "    }\n"
						  "\n"
						  "    return rgbColor;\n"
						  "}\n"
						  "\n"
						  "void main()\n"
						  "{\n"
						  "    highp vec4 imageColor = yuvToRgb(v_texCoord);\n"
						  "    float blueColor = imageColor.b * 63.0;\n"
						  "    \n"
						  "    vec2 quad1;\n"
						  "    quad1.y = floor(floor(blueColor) / 8.0);\n"
						  "    quad1.x = floor(blueColor) - (quad1.y * 8.0);\n"
						  "\n"
						  "    vec2 quad2;\n"
						  "    quad2.y = floor(ceil(blueColor) / 8.0);\n"
						  "    quad2.x = ceil(blueColor) - quad2.y * 8.0;\n"
						  "    \n"
						  "    vec2 texPos1;\n"
						  "    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * imageColor.r);\n"
						  "    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * imageColor.g);\n"
						  "\n"
						  "    vec2 texPos2;\n"
						  "    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * imageColor.r);\n"
						  "    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * imageColor.g);\n"
						  "    \n"
						  "    vec4 newColor1 = texture(s_LutTexture, texPos1);\n"
						  "    vec4 newColor2 = texture(s_LutTexture, texPos2);\n"
						  "    \n"
						  "    vec4 newColor = mix(newColor1, newColor2, fract(blueColor));\n"
						  "    outColor = mix(imageColor, vec4(newColor.rgb, imageColor.w), 1.0);\n"
						  "}\n"
						  "";
int LutSample::Init() {
	int ret = InitTextureId(vShaderStr, fShaderStr);
	if (ret != 0) {
		LOGCATE("LutSample::Init...InitTextureId Failed!");
		return ret;
	}

	glGenTextures(1, &m_LutImageTextureId);
	glBindTexture(GL_TEXTURE_2D, m_LutImageTextureId);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	return 0;
}

void LutSample::UnInit() {
	UnInitTextureId();
	if (m_LutImageTextureId != GL_NONE) {
		glDeleteTextures(1, &m_LutImageTextureId);
		m_LutImageTextureId = GL_NONE;
	}
}

void LutSample::Draw() {
	if (m_ProgramId == GL_NONE) return;

	DrawTextureInner();
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_LutImageTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_LutImage->width, m_LutImage->height,
				 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LutImage->ppPlane[0]);
	GLUtil::setInt(m_ProgramId, "s_LutTexture", 5);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void LutSample::SetLutImageData(FrameImage *lutImage) {
	m_LutImage = lutImage;
}

LutSample * LutSample::GetInstance() {
	if(s_Instance == nullptr)
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		if(s_Instance == nullptr)
		{
			s_Instance = new LutSample();
		}

	}
	return s_Instance;
}