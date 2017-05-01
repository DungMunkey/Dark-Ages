#include "CLoadSave.h"

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
    renderBox(80, 52, 480, 296);

    //Draw selection
    r.x=86; r.y=64 + selection * 40; r.w=468; r.h=32;
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

    font->render(94, 58, "Cancel");
    for(i=0; i < 6; i++){
      if(i == 0) sprintf(str, "QuickSave, %d-%d-%d", saves[i].year, saves[i].month + 1, saves[i].day);
      else sprintf(str, "Save %d, %d-%d-%d", i, saves[i].year, saves[i].month + 1, saves[i].day);
      font->render(94, 98 + i * 40, str);
      sprintf(str, "%s Lv=%d Gold=%d", saves[i].name, saves[i].level, saves[i].gold);
      font->render(94, 114 + i * 40, str);
    }

  } else {

    renderBox(80, 72, 480, 256);

    //Draw selection
    r.x=86; r.y=84 + selection * 40; r.w=468; r.h=32;
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    SDL_RenderFillRect(display->renderer, &r);
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

    font->render(94, 78, "Cancel");
    for(i=1; i < 6; i++){
      sprintf(str, "Save %d, %d-%d-%d", i, saves[i].year, saves[i].month, saves[i].day);
      font->render(94, 78 + i * 40, str);
      sprintf(str, "%s Lv=%d Gold=%d", saves[i].name, saves[i].level, saves[i].gold);
      font->render(94, 94 + i * 40, str);
    }
  }
}
