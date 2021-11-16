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

const float amplitude = 30.0;
const float speed = 30.0;

vec4 transition(vec2 p) {
    vec2 dir = p - vec2(.5);
    float dist = length(dir);

    if (dist > progress) {
        return mix(yuvToRgb( p), texture(s_NextTexture, p), progress);
    } else {
        vec2 offset = dir * sin(dist * amplitude - progress * speed);
        return mix(yuvToRgb( p + offset), texture(s_NextTexture, p), progress);
    }
}


void main()
{
    outColor = transition(v_texCoord);
}