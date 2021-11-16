#version 300 es
precision highp float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_texture0;
uniform sampler2D s_texture1;
uniform sampler2D s_texture2;
uniform sampler2D s_NextTexture;
uniform int u_nImgType;
uniform vec2 u_TexSize;
uniform float progress;

const float smoothness = 1.0;
const float PI = 3.141592653589;

vec4 yuvToRgb(vec2 texCoord) {
    vec4 rgbColor;
    if(u_nImgType == 1) //RGBA
    {
        rgbColor = texture(s_texture0, texCoord);
    }
    else if(u_nImgType == 2) //NV21
    {
        vec3 yuv;
        yuv.x = texture(s_texture0, texCoord).r;
        yuv.y = texture(s_texture1, texCoord).a - 0.5;
        yuv.z = texture(s_texture1, texCoord).r - 0.5;
        highp vec3 rgb = mat3(1.0,       1.0,     1.0,
        0.0, 	-0.344, 	1.770,
        1.403,  -0.714,     0.0) * yuv;
        rgbColor = vec4(rgb, 1.0);
    }
    else if(u_nImgType == 3) //NV12
    {
        vec3 yuv;
        yuv.x = texture(s_texture0, texCoord).r;
        yuv.y = texture(s_texture1, texCoord).r - 0.5;
        yuv.z = texture(s_texture1, texCoord).a - 0.5;
        highp vec3 rgb = mat3(1.0,       1.0,     1.0,
        0.0, 	-0.344, 	1.770,
        1.403,  -0.714,     0.0) * yuv;
        rgbColor = vec4(rgb, 1.0);
    }
    else if(u_nImgType == 4) //I420
    {
        vec3 yuv;
        yuv.x = texture(s_texture0, texCoord).r;
        yuv.y = texture(s_texture1, texCoord).r - 0.5;
        yuv.z = texture(s_texture2, texCoord).r - 0.5;
        highp vec3 rgb = mat3(1.0,       1.0,     1.0,
        0.0, 	-0.344, 	1.770,
        1.403,  -0.714,     0.0) * yuv;
        rgbColor = vec4(rgb, 1.0);
    }
    else
    {
        rgbColor = vec4(1.0);
    }

    return rgbColor;
}

vec4 transition(vec2 p) {
    vec2 rp = p*2.-1.;
    return mix(
    texture(s_NextTexture, p),
    yuvToRgb(p),
    smoothstep(0., smoothness, atan(rp.y,rp.x) - (progress-.5) * PI * 2.5)
    );
}

void main()
{
    outColor = transition(v_texCoord);
}