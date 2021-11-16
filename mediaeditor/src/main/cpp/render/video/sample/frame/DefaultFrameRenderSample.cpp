//
// Created by 江守亭 on 2021/11/3.
//

#include "DefaultFrameRenderSample.h"

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

const char fGrayShaderStr[] =
	"#version 300 es\n"
	"precision highp float;\n"
	"in vec2 v_texCoord;\n"
	"layout(location = 0) out vec4 outColor;\n"
	"uniform sampler2D s_texture0;\n"
	"uniform sampler2D s_texture1;\n"
	"uniform sampler2D s_texture2;\n"
	"uniform int u_nImgType;// 1:RGBA, 2:NV21, 3:NV12, 4:I420\n"
	"uniform vec2 u_TexSize;\n"
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
	"    outColor = yuvToRgb(v_texCoord);\n"
	"}\n"
	"";
int DefaultFrameRenderSample::Init() {
	int ret = InitTextureId(vShaderStr, fGrayShaderStr);
	if (ret != 0) {
		LOGCATE("DefaultFrameRenderSample::Init...InitTextureId Failed!");
		return ret;
	}

	return 0;
}

void DefaultFrameRenderSample::UnInit() {
	UnInitTextureId();
}

void DefaultFrameRenderSample::Draw() {
	if (m_ProgramId == GL_NONE) return;
	DrawTextureInner();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}