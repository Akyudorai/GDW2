#version 430 
layout(location = 0) in vec2 UV; 
layout(location = 0) out vec4 outColor; 

//Night Vision Shader
uniform float t;
uniform layout(binding = 6) sampler2D S_Image; 
uniform layout(binding = 1) sampler2D noiseTex; 
uniform layout(binding = 2) sampler2D maskTex; 
uniform float lumi = 0.5; 
uniform float coloramp = 5.0; 
uniform float effect = 1.0; 

void main()
{
	vec4 colorfinal; 
	if (UV.x < effect) 
	{
		vec2 uv; uv.x = 0.5* sin(t*50.0); 
		uv.y = 0.4*cos(t*50.0); 
			float m = texture2D(maskTex, UV.st).r; 
		
		vec3 n = texture2D(noiseTex, (UV.st*3.5) + uv).rgb; 
		
		vec3 c = texture2D(S_Image, UV.st + (n.xy*0.005)).rgb;

       float lum = dot(vec3(0.30, 0.59, 0.11), c); 
	   if (lum < lumi)
       c *= coloramp;
	
		vec3 visionColor = vec3(0.1, 0.95, 0.2); 
		colorfinal.rgb = (c + (n*0.2)) * visionColor * m;
	}
		else
			{
				colorfinal = texture2D(S_Image, UV);
			}
			
			outColor.rgb = colorfinal.rgb;
			outColor.a = 1.0;
}