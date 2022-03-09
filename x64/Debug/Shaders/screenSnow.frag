#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float SIZE;
uniform float time;
uniform float timeX;
uniform float timeY;
uniform float fadeTime;
uniform sampler2D screenText;
uniform sampler2D randomText;

const float NOISE_GRANULARITY = 0.5/255.0;

float newFlake(vec2 pos, vec2 id)
{
    vec2 idPos = id / SIZE;
    //idPos = mod(idPos, 1.0);

    vec4 rand = texture(randomText, idPos);
    if(rand.x > 0.1) return 0.0;

    vec2 idPos1 = (id + vec2(rand.x)) / 20.0f;
    float r = rand.x * 200;
    float rad = rand.y * rand.z;
    rad = mod(rad, 1.0 / (SIZE*2.0));
    r = mod(r, 1.0 / SIZE);

    vec2 flakePos = idPos;// + r;
    flakePos.y += r;
    flakePos.x += sin(time*rand.z*3)/(SIZE);

    //pos = mod(pos, 1.0);
    //flakePos = mod(flakePos, 1.0);
    float dist = length(pos - flakePos);

    dist = clamp(rad - dist, 0, rad);
    dist = smoothstep(0, 1.5, dist/rad);

    return dist;
}

void main()
{
    vec4 outColor = mix(vec4(0.9), texture(screenText, TexCoord), 0.9);
    
    vec2 uv = TexCoord;
    uv += vec2(-timeX, -timeY);
    //uv = mod(uv, 1.0);

    vec2 id = uv * SIZE;
    id = floor(id);

    //id = mod(id, SIZE);

    //outColor = normalize(vec4(id.x / SIZE, id.y / SIZE, 1, 1));
    outColor = mix(vec4(0.7), outColor, smoothstep(1.0, 0.7, TexCoord.y));
    for(int i = -2; i <= 2; i++)
    {
        float fl = -1.0;
        for(int j = -1; j <= 1; j++)
        {
            vec2 offset = vec2(i,j);
            vec2 newId = id + offset;

            //if(newId.x < 0) newId.x = SIZE;
            //if(newId.x > SIZE) newId.x = 0;
            //if(newId.y > SIZE) newId.y = 0;

            fl = newFlake(uv, newId);
            outColor = mix(outColor, vec4(1.0), fl);
        }
    }


    //vec4 r = texture(noiseText, TexCoord);
    //float d = length(TexCoord - vec2(0.5));

    //outColor = clamp(outColor, 0.0, 1.0);

    //float delta = smoothstep(0.7, 0, d);
    //outColor = mix(vec4(0.0), outColor, 0.7-d);


    //outColor -= mod(r.r, 0.01*d);

    //vignette from https://www.shadertoy.com/view/lsKSWR
    /*vec2 uvVig = TexCoord;
    uvVig *= 1.0 - uvVig.xy;
    float vig = uvVig.x * uvVig.y * 20.0;
    vig = pow(vig, 0.5);*/

    float t = fadeTime;
    float v = smoothstep(t, t+0.01, TexCoord.x + 0.01);
    outColor = mix(vec4(0,0,0,1), outColor, v);
    FragColor = outColor;
} 