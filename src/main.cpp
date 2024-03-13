#include<stdio.h>
#include<SDL.h>

int main(int argc,char*argv[]){
  fprintf(stderr,"Ahoj svete\n");

  auto window = SDL_CreateWindow("izg06",0,0,1024,768,0);

  bool running = true;
  while(running){ // main loop
    SDL_Event event;
    while(SDL_PollEvent(&event)){ // event loop
      if(event.type == SDL_QUIT)running = false;
        
    }

  }

  SDL_DestroyWindow(window);

  return 0;
}
