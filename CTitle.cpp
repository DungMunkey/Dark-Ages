#include "CTitle.h"

CTitle::CTitle(CDisplay* d, CFont* f, CGfxCollection* g){
  display=d;
  font=f;
  gfx=g;
  selection=0;
  active=false;
}

CTitle::~CTitle(){
  display=NULL;
  font=NULL;
  gfx=NULL;
}

void CTitle::actionCursorDown(){
  if(selection == 3) selection=0;
  else selection++;
}

void CTitle::actionCursorUp(){
  if(selection == 0) selection=3;
  else selection--;
}

int CTitle::actionEnter(){
  if(selection == 0) return 1;
  else if(selection == 1) return 2;
  else if(selection == 2) return 3;
  else if(selection == 3) return 4;
  return 0; //shouldn't ever happen
}

void CTitle::render(){
  SDL_Rect r;

  SDL_RenderClear(display->renderer);
  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  r.x=0; r.y=0; r.h=400; r.w=640;
  SDL_RenderCopy(display->renderer, gfx->title->texture, gfx->title->getTile(0), &r);

  font->setFontSize(16);
  font->render(10, 380, "Copyright (C) 1997-2017, Dark Knight Software");
  font->render(10, 370, "version 2.0 alpha 2");
  font->setFontSize(32);

  //Menu options
  font->render(260, 240, "New Game");
  font->render(260, 270, "Load Game");
  font->render(260, 300, "Options");
  font->render(260, 330, "Exit");

  //draw indicator
  r.w=16;
  r.h=16;
  r.x = 240;
  r.y = 244 + selection * 30;
  SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);

  SDL_RenderPresent(display->renderer);

}

int CTitle::run(){

  //Return codes
  //1 = new game
  //2 = load game
  //3 = options
  //4 = quit

  SDL_Event e;

  while(true){

    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_KEYDOWN)  {
        switch(e.key.keysym.sym)  {
        case SDLK_UP: actionCursorUp(); break;
        case SDLK_DOWN: actionCursorDown(); break;
        case SDLK_RETURN:
        case SDLK_SPACE:
          return actionEnter();
        default: break;
        }
      } else if(e.type == SDL_CONTROLLERBUTTONDOWN) {
        switch(e.cbutton.button){
        case SDL_CONTROLLER_BUTTON_A: return actionEnter();
        case SDL_CONTROLLER_BUTTON_DPAD_UP: actionCursorUp(); break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: actionCursorDown(); break;
        default:break;
        }
      }
    }

    render();

  }

  return 0; //shouldn't ever happen

}
