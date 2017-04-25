#include "COptions.h"

COptions::COptions(CDisplay* d, CMusic* m, CFont* f, CGfxCollection* g, sConf* c){
  display=d;
  conf=c;
  font=f;
  music=m;
  gfx=g;
  selection=0;
  active=false;
  tmpVol=conf->vol;
  tmpFull=conf->fullScreen;
  tmpScreen=display->currentScreenMode;
}

COptions::~COptions(){
  display=NULL;
  music=NULL;
  conf=NULL;
  font=NULL;
  gfx=NULL;
}

bool COptions::logic(optAction a){
  switch(a){
  case optPress:
    if(selection == 0) return false;
    if(active) {
      active=false;
      if(selection==1){
        conf->vol=tmpVol;
      } else if(selection == 2){
        SDL_SetWindowSize(display->window, display->screenModes[tmpScreen].w, display->screenModes[tmpScreen].h);
        display->currentScreenMode=tmpScreen;
        conf->w=display->screenModes[tmpScreen].w;
        conf->h=display->screenModes[tmpScreen].h;
      } else if(selection==3){
        if(tmpFull){
          conf->fullScreen=true;
          SDL_SetWindowFullscreen(display->window, SDL_WINDOW_FULLSCREEN);
        } else {
          conf->fullScreen=false;
          SDL_SetWindowFullscreen(display->window, 0);
        }
      }
    } else {
      active=true;
      if(selection==1){
        tmpVol=conf->vol;
      } else if(selection==2){
        tmpScreen=display->currentScreenMode;
      } else if(selection==3){
        tmpFull=conf->fullScreen;
      }
    }
    break;
  case optLeft:
    if(!active) break;
    if(selection == 1 && tmpVol > 0) {
      tmpVol--;
      music->setVolume(tmpVol);
    } else if(selection == 2 && tmpScreen > 0){
      tmpScreen--;
    } else if(selection == 3){
      tmpFull=false;
    }
    break;
  case optRight:
    if(!active) break;
    if(selection == 1 && tmpVol <10) {
      tmpVol++;
      music->setVolume(tmpVol);
    } else if(selection == 2 && tmpScreen<display->screenModes.size() - 1){
      tmpScreen++;
    } else if(selection == 3){
      tmpFull=true;
    }
    break;
  case optUp:
    if(active) break;
    if(selection >0) selection--;
    break;
  case optDown:
    if(active) break;
    if(selection <3) selection++;
    break;
  default:
    break;
  }
  return true;
}

void COptions::render(){
  SDL_Rect r;
 
  //SDL_SetRenderTarget(display->renderer, canvas);
  SDL_RenderClear(display->renderer);

  renderBox(20, 20, 600, 360);

  if(active) {
    SDL_SetRenderDrawColor(display->renderer, 0, 128, 0, 255);
    r.w=570;
  } else {
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    r.w=260;
  }
  r.h=26;
  switch(selection){
  case 0:   r.x=36; r.y=40;    break;
  case 1:   r.x=36; r.y=80;    break;
  case 2:   r.x=36; r.y=120;   break;
  case 3:   r.x=36; r.y=160;   break;
  default:  break;
  }
  SDL_RenderFillRect(display->renderer, &r);

  //draw music indicators
  if(selection == 1 && active){
    r.w=16; r.h=16;
    if(tmpVol > 0) {
      r.x = 400; r.y = 84;
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    }
    if(tmpVol<10){
      r.x = 590; r.y = 84;
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  //draw resolution indicators
  if(selection == 2 && active){
    r.w=16; r.h=16;
    if(tmpScreen > 0) {
      r.x = 400; r.y = 124;
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    }
    if(tmpScreen<display->screenModes.size() - 1){
      r.x = 590; r.y = 124;
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  //draw fullscreen indicators
  if(selection == 3 && active){
    r.w=16; r.h=16;
    if(tmpFull) {
      r.x = 400; r.y = 164;
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    } else {
      r.x = 590; r.y = 164;
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  font->render(40, 40, "Return to Game");
  font->render(40, 80, "Music");
  r.w = 186; r.h = 24; r.x = 410; r.y = 80;
  SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.x++; r.y++; r.w-=2; r.h-=2;
  SDL_RenderDrawRect(display->renderer, &r);
  for(int i=1; i <= tmpVol; i++){
    r.w = 16; r.h = 16; r.x = 414 + (i - 1) * 18; r.y = 84;
    SDL_RenderFillRect(display->renderer, &r);
  }
  font->render(40, 120, "Screen Res:");
  font->render(430, 120, display->screenModes[tmpScreen].name);

  font->render(40, 160, "Fullscreen:");
  if(tmpFull) font->render(450, 160, "Yes");
  else font->render(460, 160, "No");

  font->render(40, 200, "Keys:");
  font->render(60, 230, "Cursors = Movement");
  font->render(60, 260, "SPACE/ENTER = Talk/Action");
  font->render(60, 290, "C = Cast Spell");
  font->render(60, 320, "Z = Player Stats");
  font->render(60, 350, "ESC = Game Menu");


  SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255);

  //SDL_SetRenderTarget(display->renderer, NULL);
  //SDL_RenderCopy(display->renderer, canvas, NULL, NULL);
  SDL_RenderPresent(display->renderer);

}

void COptions::run(){

  SDL_Event e;
  bool stop = false;

  while(true){

    while(SDL_PollEvent(&e) != 0) {
      if(e.type == SDL_KEYDOWN)  {
        //Select surfaces based on key press
        switch(e.key.keysym.sym)  {
        case SDLK_UP:  logic(optUp);  break;
        case SDLK_DOWN: logic(optDown); break;
        case SDLK_LEFT: logic(optLeft); break;
        case SDLK_RIGHT: logic(optRight); break;
        case SDLK_ESCAPE: 
          if(!active) stop = true; 
          break;
        case SDLK_RETURN:
        case SDLK_SPACE:
          if(!logic(optPress)) stop=true; 
          break;
        default: break;
        }
      }
    }

    if(stop) break;
    render();
  }

}
