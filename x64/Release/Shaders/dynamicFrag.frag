#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float Glowing;
in float ID;

uniform float time;
uniform sampler2DArray diffuse;
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
    vec4 outColor = texture(diffuse, vec3(TexCoord.xy, ID - 1.0));
    
    if(outColor.a < 0.1) discard; 
    if(Glowing > 0.1) outColor *= 1.0 + 1.2*(0.5*sin(time*8 - 1.5) + 0.5);

    //float div = clamp(dot(normal, normalize(vec3(5.0, 5.0, 5.0))), 0, 1);
    //div = smoothRound(div, 0.7, 0.05);

    //outColor *= div;

	
	vec3 rgb = outColor.rgb * 0.5;
    FragColor = vec4(rgb, outColor.a);
} 