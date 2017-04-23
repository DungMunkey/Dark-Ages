#include "CWindow.h"

CWindow::CWindow(){
  display=NULL;
  font=NULL;
  gfx=NULL;
}

CWindow::CWindow(CDisplay* d, CFont* f, CGfxCollection* g){
  display=d;
  font=f;
  gfx=g;
}

CWindow::~CWindow(){
  display=NULL;
  font=NULL;
  gfx=NULL;
}

void CWindow::render(){
}

void CWindow::renderBox(int x, int y, int w, int h){
  SDL_Rect r;

  //Draw Box
  r.w = w; r.h = h; r.x = x; r.y = y;
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 200);
  SDL_RenderFillRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 96, 96, 96, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_SetRenderDrawColor(display->renderer, 128, 128, 128, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_SetRenderDrawColor(display->renderer, 96, 96, 96, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.w -= 2; r.h -= 2; r.x++; r.y++;
  SDL_RenderDrawRect(display->renderer, &r);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

}
