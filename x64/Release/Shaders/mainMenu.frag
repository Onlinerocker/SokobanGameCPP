#version 330 core
#define TILES 10.0
#define PI 3.14159265359

in vec2 TexCoord;
out vec4 fragColor;

uniform float time;

//2D random from https://www.shadertoy.com/view/WstGDj
float random (vec2 uv) {
    return fract(sin(dot(uv, vec2(135., 263.))) * 103.214532);
}

vec4 drawSnow(vec2 curid, vec2 uv, vec4 fragColor, float r, float c)
{
    float maxoff = 2.0 / TILES; //calculate the max offset a particle can have (two tiles)

    //loop through neighboring tiles
    for(int x=-2; x<=1; x++)
    {
        for(int y=-2; y<=0; y++)
        {
            float rad = (1.0 / (TILES * 5.0)) * r; //set default radius
            vec2 id = curid + vec2(x, y); //get the id of the tile we're visiting
            vec2 pos = id / TILES; //calculate position
            float xmod = mod(random(pos), maxoff);
            pos.x += xmod; //add a random x-offset
            pos.y += mod(random(pos+vec2(4,3)), maxoff); //add a random y-offset
            rad *= mod(random(pos), 1.0); //vary the radius by multiplying by a random val
            pos.x += 0.5*(maxoff-xmod)*sin(time*r + random(pos)*100.0); //dynamic sin wave x-offset
            
            float len = length(uv - pos); //calculate distance from tile's particle

            //if we're inside the particle, draw it
            float v = smoothstep(0.0, 1.5, (rad - len) / rad*0.75);
            fragColor = mix(fragColor, vec4(c), v);      
        }
    }
    
    return fragColor;
}


float hash11(float p)
{
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

vec3 createRay(vec2 ray, vec2 uv, vec3 col, float delay, float size, float len)
{
    vec3 sunCol = vec3(1.0, 0.9, 0.8);
    float ang = dot(normalize(uv), normalize(ray));
    ang = abs(1.0 - ang);
    ang = ang / size;
    ang = clamp(ang, 0.0, 1.0);
    
    float v = smoothstep(0.0, 1.0, (1.0 - ang)*(1.0 - ang));
    v *= sin(time/2.0 + delay)/2.0 + 0.5;
    
    float l = length(uv) * len;
    l = clamp(l, 0.0, 1.0);

    vec3 o = mix(col, mix(sunCol, col, l), smoothstep(1.0, 4.0, time) * v);
    return o;
}

void main()
{
	vec2 uv = TexCoord * 2.0 - 1.0;
	uv.x *= 16.0/9.0;
	uv.y -= 0.7;
	vec3 sunCol = vec3(1.0, 0.9, 0.8);
    vec3 col = mix(vec3(0.0, 0.60, 0.85), vec3(1), smoothstep(0.0, 1.0, -1.0-uv.y));

    float t = time/15.0;
    mat2 rotTime = mat2( cos(t), sin(t), -sin(t), cos(t) );
    for(float x=0.0; x < 2.0*PI; x += PI/8.0)
    {
        mat2 rot = mat2( cos(x), sin(x), -sin(x), cos(x) );
        vec2 ray = rotTime * rot * vec2(1.0, 0.0); 
        col = createRay(ray, uv, col, hash11(x*15.0)*10000.0, hash11(x*234.0)*0.04, 2.0+hash11(54.0*x)*2.0);
    }
    
    float l = length(uv)*4.5;
    l -= 0.2;
    l = clamp(l, 0.0, 1.0);
    col = mix(sunCol*1.2, col, smoothstep(0.0, 1.0, l));

    // Output to screen
    fragColor = vec4(col,1.0);

    vec2 uvNorm = TexCoord; //normalized UV coordinate [0, 1]
    vec2 uvog = TexCoord; //UV coordinate (will remain static)
	uvog.x *= 16.0/9.0;    
	uv = uvog; //UV coordinate (we'll modify this one)
    
    //draw the closest snow layer
    uv += 0.2*vec2(-time, time); //move the UV coords based on time
    vec2 curid = floor(uv * TILES); //calculate the ID associated with the current UV
    curid += vec2(0.5); //center the ID
    
    //if(curid.y > 10.0)
    {
    fragColor += drawSnow(curid, uv, vec4(0), 1.0, 0.9); //draw closest snow layer
    
    //draw the middle snow layer, calculate new UV and ID
    uv = uvog + 0.1*vec2(-time - 100.0, time + 100.0);
    curid = floor(uv * TILES);
    curid += vec2(0.5);
    //fragColor += drawSnow(curid, uv, vec4(0), 0.75, 0.45); 
    
    //draw the far snow layer, calculate new UV and ID
    uv = uvog + 0.05*vec2(-time - 150.0, time + 150.0);
    curid = floor(uv * TILES);
    curid += vec2(0.5);
    //fragColor += drawSnow(curid, uv, vec4(0), 0.5, 0.225);
    
    fragColor = fragColor;
    }
}