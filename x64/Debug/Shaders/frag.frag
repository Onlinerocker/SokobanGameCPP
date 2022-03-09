#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float time;
uniform sampler2D diffuse;
uniform sampler2D specular;


//Smoothly rounds to the nearest float value. Range defines
//the range in which the value should be interpolated.
float smoothRound(float v, float roundTo, float range)
{
    
    float val = v/roundTo;
    float frac = fract(val);
    
    if(abs(frac - 0.5) <= range) //If we're close to the rounded "border", do an interpolate between the values
        						 //on each side of the border
    {
        float rd = round(val);
        if(frac < 0.5)
            val = mix((rd)*roundTo, (rd+1.0)*roundTo, smoothstep(0.5-range, 0.5, frac));
		if(frac >= 0.5)
            val = rd*roundTo;//notice: this is equivalent to... mix((rd+1.0)*roundTo, (rd)*roundTo, smoothstep(0.5-range, 0.5, frac));
    }
    else //Rounds to nearest float value.
    {        
     	val = round(val);   
        val *= roundTo;
    } 
    
    return val;
}

void main()
{
    vec4 outColor = texture(diffuse, TexCoord);

    if(outColor.a < 0.1) discard;

    //float div = clamp(dot(normal, normalize(vec3(5.0, 5.0, 5.0))), 0, 1);
    //div = smoothRound(div, 0.7, 0.05);

    //outColor *= div;


    FragColor = outColor * 0.8;
} 