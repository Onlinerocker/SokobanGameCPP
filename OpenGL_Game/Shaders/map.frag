#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float ID;
in vec2 Pos;
in vec3 Color;

uniform float time;
uniform float timeTrue;
uniform sampler2DArray diffuse;

#define PI 3.14159265359

float hash(vec2 p)
{
    // Two typical hashes...
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
    
    // This one is better, but it still stretches out quite quickly...
    // But it's really quite bad on my Mac(!)
    //return fract(sin(dot(p, vec2(1.0,113.0)))*43758.5453123);

}

void main()
{    
    float id = ID;
    float m = 1.0;
	float alphaModi = 1.0;
    vec2 uv = TexCoord.xy;
    vec4 spark = vec4(0);
    float ti = time*2*PI;    

	if(id == 255)
	{
		id = 66;
		if(TexCoord.y > 0.8) discard;

		alphaModi = smoothstep(0.8, 0.0, TexCoord.y);
	}
	
	if(id == 252)
	{
		id = 67;
		if(TexCoord.x > 0.8) discard;

		alphaModi = smoothstep(0.8, 0.0, TexCoord.x);
	}

	//these are water tiles, loop them magically
    if(id == 136 || id == 137)
    {
    
        //vec2 pdelta = vec2(timeTrue * 0.5 * 17.0f);
        //vec2 p = floor(Pos - pdelta);


        vec2 u = uv*17.0f;
        u = floor(u);
        u /= 17.0f;

        m = sin(ti*0.5)*0.25 + 1.25;

        float r = hash(vec2(u.x, u.y*3.0));
        float a = hash(u);

        if(a > 0.95)
        {
            ti += hash(u.yx + id + u + floor(Pos)*50.0f) * 32.0;
            ti*=2.0f;
            float wink = sin(ti - PI/2) * 0.5 + 0.5;
            float silence = floor(sin(ti * 0.5) + 1);
            spark = vec4(wink * silence);
        }

        uv.y -= time*0.5;
		uv.x += time*0.5;        
		uv = mod(uv, 2.0);

        if(id == 136 && uv.x > 1.0 && uv.y < 1.0)
        {
            id++;
        }else if(id == 136 && uv.y > 1.0 && uv.x < 1.0)
        {
            id++;    
        }else if(id == 137 && uv.x > 1.0 && uv.y < 1.0)
        {
            id--;
        }else if(id == 137 && uv.y > 1.0 && uv.x < 1.0)
        {
            id--;     
        }
    }

    if(id == 66 || id == 72)
    {
        uv.y -= time*2.0;
        uv = mod(uv, 1.0);
        m += sin(ti) * 0.5 + 1.0;
    }

	if(id == 67)
	{	
		uv.x -= time*2.0;
        uv = mod(uv, 1.0);
        m += sin(ti) * 0.5 + 1.0;
	}

    uv = fract(uv);
    

	vec4 outColor;
    if(id == 254)
	{
		uv.x -= time*2.0;
        uv = mod(uv, 1.0);
        m += sin(ti) * 0.5 + 1.0;
		outColor = texture(diffuse, vec3(uv.xy, 67 - 1.0)) * m;

		uv = TexCoord;
        uv.y -= time*2.0;
        uv = mod(uv, 1.0);
        m += sin(ti) * 0.5 + 1.0;
		if(outColor.a < 0.1 && TexCoord.y < 0.7) outColor = texture(diffuse, vec3(uv.xy, 66 - 1.0)) * m;
	}
	else if(id == 253)
	{
        uv.y -= time*2.0;
        uv = mod(uv, 1.0);
        m += sin(ti) * 0.5 + 1.0;
		outColor = texture(diffuse, vec3(uv.xy, 66 - 1.0)) * m;

		uv = TexCoord;
		uv.x -= time*2.0;
        uv = mod(uv, 1.0);
        m += sin(ti) * 0.5 + 1.0;
		if(outColor.a < 0.1 && TexCoord.x < 0.7) outColor = texture(diffuse, vec3(uv.xy, 67 - 1.0)) * m;
	}
	else
	{
		outColor = texture(diffuse, vec3(uv.xy, id - 1.0)) * m;
	}
	
	outColor += spark;

	outColor.a *= alphaModi;
    if(outColor.a < 0.1) discard;

    FragColor = vec4(outColor.rgb*0.8*Color, outColor.a);
} 