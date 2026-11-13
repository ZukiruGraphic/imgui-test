#include <SDL.h>
#include <GLES3/gl3.h>
int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);
  SDL_Window* w = SDL_CreateWindow("RED", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext gl = SDL_GL_CreateContext(w);
  SDL_GL_MakeCurrent(w, gl);
  bool r = true;
  while (r) { SDL_Event e; while (SDL_PollEvent(&e)) if (e.type==SDL_QUIT) r=false; glClearColor(1.0f,0.0f,0.0f,1.0f); glClear(GL_COLOR_BUFFER_BIT); SDL_GL_SwapWindow(w); }
  return 0;
}
