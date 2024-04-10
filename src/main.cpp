#include<stdio.h>
#include<SDL.h>
#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<bunny.hpp>

using namespace ge::gl;

int main(int argc,char*argv[]){
  int width = 1024;
  int height = 768;
  auto window = SDL_CreateWindow("izg06",0,0,width,height,SDL_WINDOW_OPENGL);

  auto context = SDL_GL_CreateContext(window);

  ge::gl::init(SDL_GL_GetProcAddress);


  auto vsSrc = R".(
  #version 330

  layout(location=0)in vec3 position;
  layout(location=1)in vec3 normal  ;

  out vec3 vNormal;

  uniform mat4 model = mat4(1);
  uniform mat4 view = mat4(1);
  uniform mat4 proj = mat4(1);

  void main(){
    vNormal = normal;
    gl_Position = proj*view*model*vec4(position,1);
  }
  ).";

  auto fsSrc = R".(
  #version 330

  in vec3 vNormal;
  out vec4 fragColor;
  void main(){
    fragColor = vec4(vNormal,1);
  }
  ).";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs,1,&vsSrc,nullptr);
  glCompileShader(vs);

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs,1,&fsSrc,nullptr);
  glCompileShader(fs);

  GLuint prg = glCreateProgram();
  glAttachShader(prg,vs);
  glAttachShader(prg,fs);
  glLinkProgram(prg);

  GLuint vbo;
  glCreateBuffers(1,&vbo);
  glNamedBufferData(vbo,sizeof(bunnyVertices),bunnyVertices,GL_DYNAMIC_COPY);

  GLuint ebo;
  glCreateBuffers(1,&ebo);
  glNamedBufferData(ebo,sizeof(bunnyIndices),bunnyIndices,GL_DYNAMIC_COPY);

  GLuint vao;
  glCreateVertexArrays(1,&vao);


  glEnableVertexArrayAttrib(vao,0);
  glVertexArrayAttribBinding(vao, 0, 0);
  glEnableVertexArrayAttrib(vao, 0);
  glVertexArrayAttribFormat(vao, 0,3,GL_FLOAT,GL_FALSE,0);
  glVertexArrayVertexBuffer(vao, 0, vbo,sizeof(float)*0,sizeof(BunnyVertex));

  glEnableVertexArrayAttrib(vao,1);
  glVertexArrayAttribBinding(vao, 1, 1);
  glEnableVertexArrayAttrib(vao, 1);
  glVertexArrayAttribFormat(vao, 1,3,GL_FLOAT,GL_FALSE,0);
  glVertexArrayVertexBuffer(vao, 1, vbo,sizeof(float)*3,sizeof(BunnyVertex));

  glVertexArrayElementBuffer(vao,ebo);
   
  glm::mat4 proj = glm::perspective(glm::pi<float>()/2.f,(float)width/(float)height,0.1f,1000.f);
  glm::mat4 view = glm::lookAt(glm::vec3(2),glm::vec3(0),glm::vec3(0,1,0));
  float alpha=0;

  glEnable(GL_DEPTH_TEST);

  bool running = true;
  while(running){ // main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_QUIT)running = false;
        
    }

    //draw
    glClearColor(0,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    glUseProgram(prg);

    glm::mat4 model = glm::rotate(glm::mat4(1),alpha,glm::vec3(0,1,0));
    alpha+=0.1;
    glProgramUniformMatrix4fv(prg,glGetUniformLocation(prg,"view"),1,GL_FALSE,(float*)&view);
    glProgramUniformMatrix4fv(prg,glGetUniformLocation(prg,"proj"),1,GL_FALSE,(float*)&proj);
    glProgramUniformMatrix4fv(prg,glGetUniformLocation(prg,"model"),1,GL_FALSE,(float*)&model);
    glDrawElements(GL_TRIANGLES,sizeof(bunnyIndices)/sizeof(uint32_t),GL_UNSIGNED_INT,0);


    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);

  return 0;
}
