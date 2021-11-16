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

//direction of movement :  0 : up, 1, down
const bool direction = true ; // = 1
//luminance threshold
const float l_threshold = 0.8 ; // = 0.8
//does the movement takes effect above or below luminance threshold ?
const bool above = false ; // = false

//Random function borrowed from everywhere
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
    0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
    -0.577350269189626,  // -1.0 + 2.0 * C.x
    0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);

    // Other corners
    vec2 i1;
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
    + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;

    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

    // Compute final noise value at P
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}

// Simplex noise -- end

float luminance(vec4 color){
    //(0.299*R + 0.587*G + 0.114*B)
    return color.r*0.299+color.g*0.587+color.b*0.114;
}

vec2 center = vec2(1.0, direction);

vec4 transition(vec2 uv) {
    vec2 p = uv.xy / vec2(1.0).xy;
    if (progress == 0.0) {
        return yuvToRgb(p);
    } else if (progress == 1.0) {
        return texture(s_NextTexture, p);
    } else {
        float x = progress;
        float dist = distance(center, p)- progress*exp(snoise(vec2(p.x, 0.0)));
        float r = x - rand(vec2(p.x, 0.1));
        float m;
        if(above){
            m = dist <= r && luminance(yuvToRgb(p))>l_threshold ? 1.0 : (progress*progress*progress);
        }
        else{
            m = dist <= r && luminance(yuvToRgb(p))<l_threshold ? 1.0 : (progress*progress*progress);
        }
        return mix(yuvToRgb(p), texture(s_NextTexture, p), m);
    }
}


void main()
{
    outColor = transition(v_texCoord);
}