#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_texture0;
uniform sampler2D s_texture1;
uniform sampler2D s_texture2;
uniform sampler2D s_NextTexture;
uniform int u_nImgType;
uniform vec2 u_TexSize;
uniform float progress;

const float scale = 4.0;
const float smoothness = 0.01;
const float seed = 12.9898;

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

float random(vec2 co)
{
    highp float a = seed;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners porcentages
    return mix(a, b, u.x) +
    (c - a)* u.y * (1.0 - u.x) +
    (d - b) * u.x * u.y;
}

vec4 transition (vec2 uv) {
    vec4 from = yuvToRgb(uv);
    vec4 to = texture(s_NextTexture, uv);
    float n = noise(uv * scale);

    float p = mix(-smoothness, 1.0 + smoothness, progress);
    float lower = p - smoothness;
    float higher = p + smoothness;

    float q = smoothstep(lower, higher, n);

    return mix(
    from,
    to,
    1.0 - q
    );
}

void main()
{
    outColor = transition(v_texCoord);
}