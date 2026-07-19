#include "CLoadSave.h"

using namespace std;

CLoadSave::CLoadSave(CDisplay* d, CFont* f, da1save* s){
  display=d;
  font=f;
  saves=s;
  selection=0;
  showLoad=true;
}
CLoadSave::~CLoadSave(){
  display=NULL;
  font=NULL;
  saves=NULL;
}

void CLoadSave::render(){
  SDL_Rect r;
  int i;
  char str[64];

  //Draw Menu
  if(showLoad){
    renderBox(display->S(80), display->S(52), display->S(480), display->S(296));

    //Draw selection
    r.x=display->S(86); r.y=display->S(64) + selection * display->S(40); r.w=display->S(468); r.h=display->S(32);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

    font->render(display->S(94), display->S(58), "Cancel");
    for(i=0; i < 6; i++){
      if(i == 0) sprintf(str, "QuickSave, %d-%d-%d", saves[i].year, saves[i].month + 1, saves[i].day);
      else sprintf(str, "Save %d, %d-%d-%d", i, saves[i].year, saves[i].month + 1, saves[i].day);
      font->render(display->S(94), display->S(98) + i * display->S(40), str);
      sprintf(str, "%s Lv=%d Gold=%d", saves[i].name, saves[i].level, saves[i].gold);
      font->render(display->S(94), display->S(114) + i * display->S(40), str);
    }

  } else {

    renderBox(display->S(80), display->S(72), display->S(480), display->S(256));

    //Draw selection
    r.x=display->S(86); r.y=display->S(84) + selection * display->S(40); r.w=display->S(468); r.h=display->S(32);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

    font->render(display->S(94), display->S(78), "Cancel");
    for(i=1; i < 6; i++){
      sprintf(str, "Save %d, %d-%d-%d", i, saves[i].year, saves[i].month, saves[i].day);
      font->render(display->S(94), display->S(78) + i * display->S(40), str);
      sprintf(str, "%s Lv=%d Gold=%d", saves[i].name, saves[i].level, saves[i].gold);
      font->render(display->S(94), display->S(94) + i * display->S(40), str);
    }
  }
}
