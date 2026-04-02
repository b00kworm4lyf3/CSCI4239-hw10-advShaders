/*
 *  Homework 10: Tessellation Shader
 *
 *  Key bindings:
 *  +/-        Increase/decrease inner and outer
 *  i/I        Increase/decrease inner
 *  o/O        Increase/decrease outer
 *  r          Reset inner/outer levels
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int th=25;         //  Azimuth of view angle
int ph=30;         //  Elevation of view angle
int zh=0;         //  Light angle
int shader=0;     //  Shader
int N;            //  Number of faces
int Inner=13;      //  Tesselation inner level
int Outer=12;      //  Tesselation outer level
float asp=1;      //  Screen aspect ratio
float dim=2;      //  World dimension
float elv=-10;    //  Light elevation
unsigned int vao; //  plane VAO
int tex,nml, lava1, lava1Norm, lava2;   //textures

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   float project[16];
   mat4identity(project);
   mat4perspective(project, 57, asp, 0.1, 6*dim);
   //mat4ortho(project , -dim*asp, +dim*asp, -dim, +dim, -dim, +dim);
   //  Modelview matrix
   float modelview[16];
   mat4identity(modelview);
   mat4translate(modelview, 0, 0, -3.0*dim);
   mat4rotate(modelview , ph,1,0,0);
   mat4rotate(modelview , th,0,1,0);
   //  Normal matrix
   float normal[9];
   mat3normalMatrix(modelview , normal);

   //  Set shader
   glUseProgram(shader);

   //  Lighting parameters
   int id = glGetUniformLocation(shader,"LightDir");
   glUniform3f(id,Cos(zh),0.1*elv,Sin(zh));

   //  Set transformation matrixes
   id = glGetUniformLocation(shader,"Projection");
   glUniformMatrix4fv(id,1,0,project);
   id = glGetUniformLocation(shader,"Modelview");
   glUniformMatrix4fv(id,1,0,modelview);
   id = glGetUniformLocation(shader,"NormalMatrix");
   glUniformMatrix3fv(id,1,0,normal);

   //  Controls for tesselation level
   id = glGetUniformLocation(shader,"Inner");
   glUniform1f(id,Inner);
   id = glGetUniformLocation(shader,"Outer");
   glUniform1f(id,Outer);

   //time and textures for colour and disp
   float t = (float)glfwGetTime();
   glUniform1f(glGetUniformLocation(shader,"time"), t);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,tex);
   glUniform1i(glGetUniformLocation(shader,"tex"),0);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,nml);
   glUniform1i(glGetUniformLocation(shader,"nml"),1);
   glUniform1i(glGetUniformLocation(shader,"terr"),2);
   glUniform1i(glGetUniformLocation(shader,"lava"),3);
   glActiveTexture(GL_TEXTURE4);
   glBindTexture(GL_TEXTURE_2D,lava1);
   glUniform1i(glGetUniformLocation(shader,"noise1"),4);
   glActiveTexture(GL_TEXTURE5);
   glBindTexture(GL_TEXTURE_2D,lava1Norm);
   glUniform1i(glGetUniformLocation(shader,"noise1Norm"),5);
   glActiveTexture(GL_TEXTURE6);
   glBindTexture(GL_TEXTURE_2D,lava2);
   glUniform1i(glGetUniformLocation(shader,"noise2"),6);

   // Render the scene
   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   glClearColor(0.2,0.2,0.2,1.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glPatchParameteri(GL_PATCH_VERTICES,3);

   //  Bind VAO and render
   glBindVertexArray(vao);
   glDrawElements(GL_PATCHES,N,GL_UNSIGNED_INT,0);

   //  Unset shader
   glUseProgram(0);

#ifndef APPLE_GL4
   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("FPS=%d Angle=%d,%d Inner=%d Outer=%d",
      FramesPerSecond(),th,ph,Inner,Outer);
#endif
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Move light
   else if (key == GLFW_KEY_LEFT_BRACKET && !shift)
      zh += 5;
   else if (key == GLFW_KEY_RIGHT_BRACKET && !shift)
      zh -= 5;
   //  Move light
   else if (key == GLFW_KEY_LEFT_BRACKET && shift)
      elv++;
   else if (key == GLFW_KEY_RIGHT_BRACKET && shift)
      elv--;
   //  Reset levels
   else if (key == GLFW_KEY_R)
   {
      Inner = 13;
      Outer = 12;
   }
   //  Both levels
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
   {
      Inner++;
      Outer++;
   }
   else if ((key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) && N>1)
   {
      Inner--;
      Outer--;
   }
   //  Inner level
   else if (key == GLFW_KEY_I)
      Inner += shift ? -1 : +1;
   //  Outer level
   else if (key == GLFW_KEY_O)
      Outer += shift ? -1 : +1;
   //  Right arrow key - increase angle by 5 degrees
   else if (key == GLFW_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLFW_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLFW_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLFW_KEY_DOWN)
      ph -= 5;

   //  PageUp key - increase dim
   else if (key==GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key==GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;

   //  Limit tessalation level
   if (Inner<1) Inner = 1;
   if (Outer<1) Outer = 1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? width/(double)height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
}

//
//  Create Shader Program
//
int CreateShaderProgTess(void)
{
   //  Create program
   int prog = glCreateProgram();
   //  Compile shaders
   CreateShader(prog,GL_VERTEX_SHADER         ,"lava.vert");
   CreateShader(prog,GL_TESS_CONTROL_SHADER   ,"lava.tcs");
   CreateShader(prog,GL_TESS_EVALUATION_SHADER,"lava.tes");
   CreateShader(prog,GL_GEOMETRY_SHADER       ,"lava.geom");
   CreateShader(prog,GL_FRAGMENT_SHADER       ,"lava.frag");
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//
//  plane data xD
//   will just calculate texture coords from verts
//
static void CreatePlane()
{
   unsigned int verts,faces;
   const int Faces[] =
      {0, 1, 2,
       2, 3, 0
      };
   const float Verts[] =
      {
       -2.000, 0.000, -2.000,
       2.000, 0.000, -2.000,
       2.000, 0.000, 2.000,
       -2.000, 0.000,2.000
      };
   N = sizeof(Faces)/sizeof(int);

   //  Position attribute
   glUseProgram(shader);
   int loc = glGetAttribLocation(shader,"Position");

   // Create the VAO:
   glGenVertexArrays(1,&vao);
   glBindVertexArray(vao);

   // Create the VBO for facet indices
   glGenBuffers(1,&faces);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,faces);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(Faces),Faces,GL_STATIC_DRAW);

   // Create the VBO for coordinates
   glGenBuffers(1,&verts);
   glBindBuffer(GL_ARRAY_BUFFER,verts);
   glBufferData(GL_ARRAY_BUFFER,sizeof(Verts),Verts,GL_STATIC_DRAW);
   // Enable Position as vertex array
   glEnableVertexAttribArray(loc);
   glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE,3*sizeof(float),0);

   glUseProgram(0);
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Sage Ebert - Homework 10: Tessalation Shader",1,600,600,&reshape,&key);

   //  Shader program
   shader = CreateShaderProgTess();
   CreatePlane();

   //load textures
   tex = LoadTexBMP("terrain.bmp");
   nml = LoadTexBMP("terrainNorm.bmp");
   lava1 = LoadTexBMP("lava1.bmp");
   lava1Norm = LoadTexBMP("lava1Norm.bmp");
   lava2 = LoadTexBMP("lava2.bmp");

   //build colour ramps
   unsigned char terrRamp[] = {
      10,  5,  3, 255,   // darkest - near black with slight warmth
      25, 12,  8, 255,
      45, 20, 12, 255,
      65, 30, 15, 255,
      90, 40, 18, 255,
      120, 55, 22, 255,
      160, 75, 25, 255,
      200, 100, 30, 255   // brightest - dim orange glow at lava edge
   };

   unsigned int terrTex;

   glActiveTexture(GL_TEXTURE2);
   glGenTextures(1,&terrTex);
   glBindTexture(GL_TEXTURE_2D,terrTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, terrRamp);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   unsigned char lavaRamp[] = {
      130, 20,  5, 255,   // darkest - deep red
      180, 40,  5, 255,
      210, 60, 10, 255,
      230, 90, 10, 255,
      240, 130, 15, 255,
      250, 170, 30, 255,
      255, 210, 60, 255,
      255, 245, 130, 255  // brightest - hot yellow-white
   };

   unsigned int lavaTex;

   glActiveTexture(GL_TEXTURE3);
   glGenTextures(1,&lavaTex);
   glBindTexture(GL_TEXTURE_2D,lavaTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, lavaRamp);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
