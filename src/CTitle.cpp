#include "CInput.h"
#include "CTitle.h"
#include "Version.h"

using namespace std;

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
  SDL_FRect r;

  display->clearScreen();

  //The title image can be any size a mod supplies (display-scaling-plan.md section 4.5): drawn at its
  //own aspect ratio, the largest whole-number scale that fits the window, the same routine every other
  //full-screen image in the game uses. The menu text below is positioned in the UI layer's own fixed
  //640x400 reference space, entirely independent of the image's real size, so it stays in the same place
  //on screen no matter what the mod's title image looks like.
  CWindow::renderFullScreenImage(display, gfx->title);

  display->beginUIPass();

  font->setFontSize(display->S(16));
  font->render(display->S(10), display->S(380), DA_COPYRIGHT);
  font->render(display->S(10), display->S(370), "version " DA_VERSION);
  font->setFontSize(display->S(32));

  //Menu options
  font->render(display->S(260), display->S(240), "New Game");
  font->render(display->S(260), display->S(270), "Load Game");
  font->render(display->S(260), display->S(300), "Options");
  font->render(display->S(260), display->S(330), "Exit");

  //draw indicator
  r.w = (float)(display->S(16));
  r.h = (float)(display->S(16));
  r.x = (float)(display->S(240));
  r.y = (float)(display->S(244) + selection * display->S(30));
  SDL_RenderTexture(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);

  display->endUIPass();

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

    while(DA_PollEvent(&e)) {
      if(e.type == SDL_EVENT_KEY_DOWN)  {
        switch(e.key.key)  {
        case SDLK_UP: actionCursorUp(); break;
        case SDLK_DOWN: actionCursorDown(); break;
        case SDLK_RETURN:
        case SDLK_SPACE:
          return actionEnter();
        default: break;
        }
      } else if(e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
        switch(e.gbutton.button){
        case SDL_GAMEPAD_BUTTON_SOUTH: return actionEnter();
        case SDL_GAMEPAD_BUTTON_DPAD_UP: actionCursorUp(); break;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN: actionCursorDown(); break;
        default:break;
        }
      }
    }

    render();

  }

  return 0; //shouldn't ever happen

}
