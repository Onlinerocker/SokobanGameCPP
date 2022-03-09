#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform float SIZE;
uniform float time;
uniform sampler2D screenText;
uniform sampler2D randomText;
uniform sampler2D noiseText;

// The classic GLSL random function
float rnd(float x)
{
    return fract(sin(dot(vec2(x+47.49,38.2467/(x+2.3)), vec2(12.9898, 78.233)))* (43758.5453));
}

float newFlake(vec2 pos, vec2 id)
{
    vec2 idPos = id / SIZE;
    //idPos = mod(idPos, 1.0);

    vec4 rand = texture(randomText, idPos);

    if(rand.x > 0.1) return 0.0;

    vec2 idPos1 = (id + vec2(rand.x)) / 20.0f;
    vec4 rand1 = texture(randomText, idPos1);
    
    float r = rand.x * 200;
    float rad = rand.y * rand.z;
    rad = mod(rad, 1.0 / (SIZE*2.0));
    r = mod(r, 1.0 / SIZE);

    vec2 flakePos = idPos;// + r;
    flakePos.y += r;
    flakePos.x += sin(time*rand.z*3 + rand.x)/(SIZE);

    //pos = mod(pos, 1.0);
    //flakePos = mod(flakePos, 1.0);
    float dist = length(pos - flakePos);

    dist = clamp(rad - dist, 0, rad);
    dist = smoothstep(0, 1.5, dist/rad);

    return dist;
}

void main()
{
    vec4 outColor = texture(screenText, TexCoord);
    
    vec2 uv = TexCoord;
    uv += vec2(-time/6, time/3);
    //uv = mod(uv, 1.0);

    vec2 id = uv * SIZE;
    id = floor(id);

    //id = mod(id, SIZE);

    //outColor = normalize(vec4(id.x / SIZE, id.y / SIZE, 1, 1));
    outColor = mix(vec4(1.0), outColor, clamp(1.3 - TexCoord.y, 0, 1.0));
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
            outColor = mix(outColor, vec4(1), fl);
        }
    }


    //vec4 r = texture(noiseText, TexCoord);
    //float d = length(TexCoord - vec2(0.5));

    //outColor = clamp(outColor, 0.0, 1.0);

    //float delta = smoothstep(0.7, 0, d);
    //outColor = mix(vec4(0.0), outColor, 0.7-d);


    //outColor -= mod(r.r, 0.01*d);

    FragColor = outColor;
} 