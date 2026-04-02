//  lava Geometry Shader

#version 400

//  Normal matrix
uniform mat3 NormalMatrix;
//  Triangles in and out
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
//  Coordinates and weights in and out
in  vec3 tePosition[3];
in  vec3 tePatchDistance[3];
in  vec2 teUv[3];
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;
out vec2 gUv;

void main()
{
   //  Compute normal as cross product
   //would probably look better sampled from noise too
   //but decided to keep the faceted look for this hw
   vec3 A = tePosition[2] - tePosition[0];
   vec3 B = tePosition[1] - tePosition[0];
   vec3 N = normalize(cross(A, B));
   //  Transform normal
   gFacetNormal = NormalMatrix * N;

   //  First vertex
   gUv = teUv[0];
   gPatchDistance = tePatchDistance[0];
   gTriDistance = vec3(1, 0, 0);
   gl_Position = gl_in[0].gl_Position;
   EmitVertex();

   //  Second vertex
   gUv = teUv[1];
   gPatchDistance = tePatchDistance[1];
   gTriDistance = vec3(0, 1, 0);
   gl_Position = gl_in[1].gl_Position;
   EmitVertex();

   //  Third vertex
   gUv = teUv[2];
   gPatchDistance = tePatchDistance[2];
   gTriDistance = vec3(0, 0, 1);
   gl_Position = gl_in[2].gl_Position;
   EmitVertex();

   //  Done with triangle
   EndPrimitive();
}
