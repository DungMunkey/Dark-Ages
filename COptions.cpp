#include "COptions.h"

using namespace std;

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
  tmpVSync=conf->vSync;

  mods=CMods::listMods();
  tmpMod=0;
  for(size_t i=0; i < mods.size(); i++){
    if(mods[i]==conf->modName){
      tmpMod=(int)i;
      break;
    }
  }
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
      } else if(selection == 4){
        if(tmpVSync){
          conf->vSync=true;
          SDL_SetHint(SDL_HINT_RENDER_VSYNC,"1");
        } else {
          conf->vSync=false;
          SDL_SetHint(SDL_HINT_RENDER_VSYNC,"0");
        }
      } else if(selection == 5){
        strncpy(conf->modName, mods[tmpMod].c_str(), 31);
        conf->modName[31]=0;
      }
    } else {
      active=true;
      if(selection==1){
        tmpVol=conf->vol;
      } else if(selection==2){
        tmpScreen=display->currentScreenMode;
      } else if(selection==3){
        tmpFull=conf->fullScreen;
      } else if(selection == 4){
        tmpVSync=conf->vSync;
      } else if(selection == 5){
        tmpMod=0;
        for(size_t i=0; i < mods.size(); i++){
          if(mods[i]==conf->modName){
            tmpMod=(int)i;
            break;
          }
        }
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
    } else if(selection == 4){
      tmpVSync=false;
    } else if(selection == 5 && tmpMod > 0){
      tmpMod--;
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
    } else if(selection == 4){
      tmpVSync=true;
    } else if(selection == 5 && tmpMod < (int)mods.size() - 1){
      tmpMod++;
    }
    break;
  case optUp:
    if(active) break;
    if(selection >0) selection--;
    break;
  case optDown:
    if(active) break;
    if(selection <5) selection++;
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

  renderBox(display->S(20), display->S(20), display->S(600), display->S(360));

  if(active) {
    SDL_SetRenderDrawColor(display->renderer, 0, 128, 0, 255);
    r.w=display->S(570);
  } else {
    SDL_SetRenderDrawColor(display->renderer, 0, 0, 128, 255);
    r.w=display->S(260);
  }
  r.h=display->S(26);
  switch(selection){
  case 0:   r.x=display->S(36); r.y=display->S(40);    break;
  case 1:   r.x=display->S(36); r.y=display->S(65);    break;
  case 2:   r.x=display->S(36); r.y=display->S(90);    break;
  case 3:   r.x=display->S(36); r.y=display->S(115);   break;
  case 4:   r.x=display->S(36); r.y=display->S(140);   break;
  case 5:   r.x=display->S(36); r.y=display->S(165);   break;
  default:  break;
  }
  SDL_RenderFillRect(display->renderer, &r);

  //draw music indicators
  if(selection == 1 && active){
    r.w=display->S(16); r.h=display->S(16);
    if(tmpVol > 0) {
      r.x = display->S(400); r.y = display->S(69);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    }
    if(tmpVol<10){
      r.x = display->S(590); r.y = display->S(69);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  //draw resolution indicators
  if(selection == 2 && active){
    r.w=display->S(16); r.h=display->S(16);
    if(tmpScreen > 0) {
      r.x = display->S(400); r.y = display->S(94);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    }
    if(tmpScreen<display->screenModes.size() - 1){
      r.x = display->S(590); r.y = display->S(94);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  //draw fullscreen indicators
  if(selection == 3 && active){
    r.w=display->S(16); r.h=display->S(16);
    if(tmpFull) {
      r.x = display->S(400); r.y = display->S(119);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    } else {
      r.x = display->S(590); r.y = display->S(119);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  //draw vSync indicators
  if(selection == 4 && active){
    r.w=display->S(16); r.h=display->S(16);
    if(tmpVSync) {
      r.x = display->S(400); r.y = display->S(144);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    } else {
      r.x = display->S(590); r.y = display->S(144);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  //draw mod indicators
  if(selection == 5 && active){
    r.w=display->S(16); r.h=display->S(16);
    if(tmpMod > 0) {
      r.x = display->S(400); r.y = display->S(169);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(3), &r);
    }
    if(tmpMod < (int)mods.size() - 1){
      r.x = display->S(590); r.y = display->S(169);
      SDL_RenderCopy(display->renderer, gfx->extra->texture, gfx->extra->getTile(2), &r);
    }
  }

  font->render(display->S(40), display->S(40), "Return to Game");
  font->render(display->S(40), display->S(65), "Music");
  r.w = display->S(186); r.h = display->S(24); r.x = display->S(410); r.y = display->S(65);
  SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255);
  SDL_RenderDrawRect(display->renderer, &r);
  r.x++; r.y++; r.w-=2; r.h-=2;
  SDL_RenderDrawRect(display->renderer, &r);
  for(int i=1; i <= tmpVol; i++){
    r.w = display->S(16); r.h = display->S(16); r.x = display->S(414) + (i - 1) * display->S(18); r.y = display->S(69);
    SDL_RenderFillRect(display->renderer, &r);
  }
  font->render(display->S(40), display->S(90), "Screen Res:");
  font->render(display->S(430), display->S(90), display->screenModes[tmpScreen].name);

  font->render(display->S(40), display->S(115), "Fullscreen:");
  if(tmpFull) font->render(display->S(450), display->S(115), "Yes");
  else font->render(display->S(460), display->S(115), "No");

  font->render(display->S(40), display->S(140), "VSync:");
  if(tmpVSync) font->render(display->S(450), display->S(140), "Yes");
  else font->render(display->S(460), display->S(140), "No");

  font->render(display->S(40), display->S(165), "Mod:");
  font->render(display->S(430), display->S(165), mods[tmpMod]);

  font->setFontSize(display->S(16));
  font->render(display->S(40), display->S(193), "Mod changes take effect after restarting the game.");
  font->setFontSize(display->S(32));

  font->render(display->S(40), display->S(224), "Keys:");
  font->render(display->S(60), display->S(248), "Cursors = Movement");
  font->render(display->S(60), display->S(272), "SPACE/ENTER = Talk/Action");
  font->render(display->S(60), display->S(296), "C = Cast Spell");
  font->render(display->S(60), display->S(320), "Z = Player Stats");
  font->render(display->S(60), display->S(344), "ESC = Game Menu");


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
      } else if(e.type == SDL_CONTROLLERBUTTONDOWN) {
        switch(e.cbutton.button){
        case SDL_CONTROLLER_BUTTON_A:
          if(!logic(optPress)) stop=true;
          break;
        case SDL_CONTROLLER_BUTTON_B:
        case SDL_CONTROLLER_BUTTON_Y:
          if(!active) stop = true;
          break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP: logic(optUp); break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN: logic(optDown); break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT: logic(optLeft); break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: logic(optRight); break;
        default:break;
        }
      }
    }

    if(stop) break;
    render();
  }

}
