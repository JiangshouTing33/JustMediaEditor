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
    if(u_nImgType == 1)
    {
        rgbColor = texture(s_texture0, texCoord);
    }
    else if(u_nImgType == 2)
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
    else if(u_nImgType == 3)
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
    else if(u_nImgType == 4)
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

const vec2 direction = vec2(-1.0, 1.0);

const float smoothness = 0.5;
const vec2 center = vec2(0.5, 0.5);

vec4 transition (vec2 uv) {
    vec2 v = normalize(direction);
    v /= abs(v.x) + abs(v.y);
    float d = v.x * center.x + v.y * center.y;
    float m = 1.0 - smoothstep(-smoothness, 0.0, v.x * uv.x + v.y * uv.y - (d - 0.5 + progress * (1.0 + smoothness)));
    return mix(yuvToRgb((uv - 0.5) * (1.0 - m) + 0.5), texture(s_NextTexture, (uv - 0.5) * m + 0.5), m);
}

void main()
{
    outColor = transition(v_texCoord);
}