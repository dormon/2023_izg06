#include<stdio.h>
#include<SDL.h>
#include<geGL/geGL.h>
#include<geGL/StaticCalls.h>

using namespace ge::gl;

int main(int argc,char*argv[]){
  auto window = SDL_CreateWindow("izg06",0,0,1024,768,SDL_WINDOW_OPENGL);

  auto context = SDL_GL_CreateContext(window);

  ge::gl::init(SDL_GL_GetProcAddress);


  auto vsSrc = R".(
  #version 330

  void main(){
    if(gl_VertexID==0)gl_Position = vec4(0,0,0,1);
    if(gl_VertexID==1)gl_Position = vec4(1,0,0,1);
    if(gl_VertexID==2)gl_Position = vec4(0,1,0,1);
  }
  ).";

  auto fsSrc = R".(
  #version 330

  out vec4 fragColor;
  void main(){
    fragColor = vec4(1);
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

  GLuint vao;
  glCreateVertexArrays(1,&vao);

    

  bool running = true;
  while(running){ // main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_QUIT)running = false;
        
    }

    //draw
    glClearColor(0,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vao);
    glUseProgram(prg);
    glDrawArrays(GL_TRIANGLES,0,3);
    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);

  return 0;
}
