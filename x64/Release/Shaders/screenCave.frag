#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float SIZE;
uniform float time;
uniform float timeY;
uniform float timeX;
uniform float fadeTime;
uniform sampler2D randomText;

const float NOISE_GRANULARITY = 0.5/255.0;
const float V = 1.0 / 1080.0;
const float H = 1.0 / 1920.0;

float newFlake(vec2 pos, vec2 id, vec4 ra)
{
    vec2 idPos = id / SIZE;
    //idPos = mod(idPos, 1.0);

    vec4 rand = ra;

    if(rand.x > 0.1 || rand.y > 0.1 || rand.z > 0.1) return 0.0;
    
    float r = rand.x * 200;
    float rad = rand.y * rand.z;
    rad = mod(rad, 1.0 / (SIZE*2.0));
    rad -= abs(sin(time + (rand.x * 100)) / (SIZE*2.0));
    rad = clamp(rad, 0.000001, 10.0);
    r = mod(r, 1.0 / SIZE);

    vec2 flakePos = idPos;// + r;
    flakePos.y += r;
    flakePos.x += sin(time*rand.z*3 + rand.x)/(SIZE);

    float dist = length(pos - flakePos);

    dist = clamp(rad - dist, 0, rad);
    dist = smoothstep(0, 1.5, dist/rad);

    return dist;
}

//from https://shader-tutorial.dev/advanced/color-banding-dithering/
float random(vec2 coords) {
   return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}


void main()
{
    vec4 outColor = vec4(0);
    
    vec2 uv = TexCoord;
    uv += vec2(-timeX, -timeY);

    vec2 id = uv * SIZE;
    id = floor(id);

    //outColor = mix(vec4(1.0), outColor, clamp(1.3 - TexCoord.y, 0, 1.0));
    vec4 r;
    for(int i = -2; i <= 2; i++)
    {
        float fl = -1.0;
        for(int j = -1; j <= 1; j++)
        {
            vec2 offset = vec2(i,j);
            vec2 newId = id + offset;
            r = texture(randomText, newId/SIZE);
            fl = newFlake(uv, newId, r);
            outColor = mix(outColor, vec4(0.5, 0.0, 0.5, 1.0), fl);
        }
    }

    float t = fadeTime;
    t = (t - 1.0);
    t *= t;
    t = 1 - clamp(t, 0.0, 1.0);

    outColor = mix(outColor, vec4(0,0,0,1), t);
    FragColor = outColor;
} 