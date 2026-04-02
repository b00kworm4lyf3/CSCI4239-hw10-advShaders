//  lava Fragment Shader
#version 400

//  Colors
const vec3 AmbientMaterial = vec3(0.5,0.5,0.5);
const vec3 DiffuseMaterial = vec3(0.75,0.75,0.75);
//  Light direction
uniform vec3 LightDir;

uniform float time;

//textures
uniform sampler2D tex;
uniform sampler2D noise1;
uniform sampler2D noise2;
uniform sampler2D terr; //ramp for land
uniform sampler2D lava; //ramp for lava

//  Normal
in  vec3  gFacetNormal;
//  Distance to edge of patch and triange
in  vec3  gTriDistance;
in  vec3  gPatchDistance;
in vec2 gUv;
//  Output color
out vec4  FragColor;

void main()
{
   //  Diffuse only lighting
   vec3 N = normalize(gFacetNormal);
   vec3 L = normalize(LightDir);
   vec3 light = AmbientMaterial + max(dot(N,L),0.0) * DiffuseMaterial;

   //  Pixel color
   //terrain colour based on voronoi intensity
   vec4 vorSamp = texture(tex, gUv);
   float terrIntensity = (vorSamp.r+texture(noise1, gUv*0.3).r)*0.5;
   vec4 terrCol = texture(terr, vec2(terrIntensity*0.9, 0.0));
   float blend = smoothstep(0.4, 0.7, vorSamp.r);

   //lava colour based on avg noise intensity
   vec4 samp1 = texture(noise1, gUv+vec2(0.02*time, 0.01*time));
   vec4 samp2 = texture(noise2, gUv+vec2(-0.02*time, 0.1*time));
   float lavaIntensity = (samp1.r+samp2.r)*0.5;
   vec4 lavaCol = texture(lava, vec2(lavaIntensity, 0.0));

   //mix between terrain and lava
   FragColor = mix(terrCol*vec4(light, 0.0), lavaCol*lavaIntensity, blend);
}
