#version 430
layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 outColor;
uniform float t;

//Night Effect Shader
uniform layout(binding = 0) sampler2D s_Image;
uniform layout(binding = 1) sampler2D TexNoise;
uniform layout(binding = 2) sampler2D TexMask;

uniform float lumi = 0.3; 
uniform float ampcolor = 5.0;
uniform float e = 1.0; 

void main ()
{
  vec4 finalColor;
  if (inUV.x < e) 
  {
    vec2 uv;           
    
    uv.x = 0.3*sin(t*45.0);                                 
    uv.y = 0.3*cos(t*45.0);                                 
    
    float xy = texture2D(TexMask, inUV.st).r;
    vec3 pq = texture2D(TexNoise, (inUV.st*2.55) + uv).rgb;
    vec3 zt = texture2D(s_Image, inUV.st + (pq.xy*0.008)).rgb;
  
    float l = dot(vec3(0.30, 0.59, 0.11), zt);
    if (l < lumi)
      zt *= ampcolor; 
  
    vec3 vc = vec3(0.15, 1.0, 0.15);
    finalColor.rgb = (zt + (pq*0.3)) * vc * xy;
   }
   else
   {
    finalColor = texture2D(s_Image, inUV);
   }
  outColor.rgb = finalColor.rgb;
  outColor.a = 1.0;
}			