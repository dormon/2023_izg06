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
  uniform vec3 cameraPos;

  out vec3 vPosition;

  out vec3 vPhong;

  void main(){
    vNormal = normal;
    vPosition = vec3(model*vec4(position,1));

    vec3 lightPos = vec3(10,20,30);

    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vPosition);
    vec3 V = normalize(cameraPos - vPosition);

    float sF = pow(clamp(dot(reflect(-L,N),V),0,1),30.f);
    //float sF = pow(1-abs(dot(N,L)-dot(N,V)),100.f);

    float dF = clamp(dot(L,N),0,1);
    vec3 materialColor = vec3(1,1,1);
    vec3 lightColor = vec3(1,1,1);
    vec3 ambientLight = vec3(0,0,0);



    vec3 lambert = dF * materialColor * lightColor + ambientLight * materialColor;

    vec3 phong = lambert + sF*lightColor*vec3(1);
  
    vPhong = phong;

    gl_Position = proj*view*model*vec4(position,1);
  }
  ).";

  auto fsSrc = R".(
  #version 330

  in vec3 vNormal;
  out vec4 fragColor;

  uniform vec3 cameraPos;
  uniform int mode = 0;

  in vec3 vPosition;
  in vec3 vPhong;

  void main(){
    vec3 lightPos = vec3(10,20,30);

    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vPosition);
    vec3 V = normalize(cameraPos - vPosition);

    float sF = pow(clamp(dot(reflect(-L,N),V),0,1),30.f);
    //float sF = pow(1-abs(dot(N,L)-dot(N,V)),100.f);

    float dF = clamp(dot(L,N),0,1);
    vec3 materialColor = vec3(1,1,1);
    vec3 lightColor = vec3(1,1,1);
    vec3 ambientLight = vec3(0,0,0);



    vec3 lambert = dF * materialColor * lightColor + ambientLight * materialColor;

    vec3 phong = lambert + sF*lightColor*vec3(1);

    if(mode == 0)
      fragColor = vec4(vPhong,1);
    else
      fragColor = vec4(phong,1);
  }
  ).";

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs,1,&vsSrc,nullptr);
  glCompileShader(vs);
  char buf2[1000];
  glGetShaderInfoLog(vs,1000,0,buf2);
  std::cerr << buf2 << std::endl;

  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs,1,&fsSrc,nullptr);
  glCompileShader(fs);
  char buf[1000];
  glGetShaderInfoLog(fs,1000,0,buf);
  std::cerr << buf << std::endl;

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
  float alpha=0;

  glEnable(GL_DEPTH_TEST);

  glDepthFunc(GL_LESS);

  bool running = true;
  float yAngle = 0.f;
  float xAngle = 0.f;
  int mode = 0;
  while(running){ // main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_QUIT)running = false;
      if(event.type == SDL_MOUSEMOTION){
        if(event.motion.state != 0){
          float sensitivity = 0.01f;
          yAngle += event.motion.xrel * sensitivity;
          xAngle += event.motion.yrel * sensitivity;
        }
      }
      if(event.type == SDL_KEYDOWN){
        if(event.key.keysym.sym == SDLK_q)
          mode = !mode;

      }
        
    }

    glm::vec2 xzCircle = glm::vec2(glm::cos(yAngle),glm::sin(yAngle));
    
    auto cameraPos = 2.f*glm::vec3(xzCircle.x*glm::cos(xAngle),glm::sin(xAngle),xzCircle.y*glm::cos(xAngle));
    glm::mat4 view = glm::lookAt(cameraPos,glm::vec3(0),glm::vec3(0,1,0));
    //draw
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    glUseProgram(prg);

    glm::mat4 model = glm::mat4(1);//glm::rotate(glm::mat4(1),alpha,glm::vec3(0,1,0));
    alpha+=0.1;
    glProgramUniformMatrix4fv(prg,glGetUniformLocation(prg,"view"),1,GL_FALSE,(float*)&view);
    glProgramUniformMatrix4fv(prg,glGetUniformLocation(prg,"proj"),1,GL_FALSE,(float*)&proj);
    glProgramUniformMatrix4fv(prg,glGetUniformLocation(prg,"model"),1,GL_FALSE,(float*)&model);
    glProgramUniform3fv(prg,glGetUniformLocation(prg,"cameraPos"),1,(float*)&cameraPos);
    glProgramUniform1i(prg,glGetUniformLocation(prg,"mode"),mode);
    glDrawElements(GL_TRIANGLES,sizeof(bunnyIndices)/sizeof(uint32_t),GL_UNSIGNED_INT,0);
    //
    //glDrawArrays(GL_TRIANGLES,0,6);


    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_DestroyWindow(window);

  return 0;
}
