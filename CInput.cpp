#include "CInput.h"
#include <iostream>

using namespace std;

CInput::CInput(){
  int i;
  for(i=0;i<128;i++) keyState[i]=false;
  for(i=0;i<3;i++) buttonState[i]=false;
  lastButton=0;

  //cout << "Joysticks: " << SDL_NumJoysticks() << endl;
  /*
  controller = NULL;
  for(i = 0; i < SDL_NumJoysticks(); i++) {
    controller = SDL_GameControllerOpen(i);
    if(controller) {
      break;
    } else {
      fprintf(stderr, "Could not open gamecontroller %i: %s\n", i, SDL_GetError());
    }
  }
  */
}

CInput::~CInput(){
  //SDL_GameControllerClose(controller);
}

bool CInput::getButtonState(int k){
  return buttonState[k];
}

bool CInput::getKeyState(int k){
  return keyState[k];
}

bool CInput::isPressed(int k){
  if(keyState[k]) return true;
  else return false;
}

bool CInput::isReleased(int k){
  if(keyState[k]) return false;
  else return true;
}

int CInput::mouseAction(){
  //printf("mouseAction %d\n",lastButton);
  int i=lastButton;
  if(lastButton>0) lastButton=0;
  return i;
}

void CInput::pollEvents(){
  //printf("Polling\n");
  while( SDL_PollEvent( &e ) != 0 )	{  
    if( e.type == SDL_KEYDOWN ) setKey(e.key.keysym.sym,true);
    else if( e.type == SDL_KEYUP ) setKey(e.key.keysym.sym,false);
    else if( e.type == SDL_MOUSEBUTTONDOWN ) setButton(e.button.button,true);
    else if( e.type == SDL_MOUSEBUTTONUP ) setButton(e.button.button,false);
  }
}

void CInput::setButton(Uint8 k, bool b){
  switch(k){
    case SDL_BUTTON_LEFT:   
      buttonState[0]=b; 
      if(!b) {
        printf("Button 1 release\n");
        lastButton=1; 
      }
      break;
    case SDL_BUTTON_RIGHT:  
      buttonState[1]=b; 
      if(!b) {
        printf("Button 2 release\n");
        lastButton=2; 
      }
      break;
    case SDL_BUTTON_MIDDLE: 
      buttonState[2]=b; 
      if(!b) {
        printf("Button 3 release\n");
        lastButton=3; 
      }
      break;
    default: 
      break;
  }
}

void CInput::setKey(SDL_Keycode k, bool b){
  switch(k){
    case SDLK_q:      keyState[KEY_Q]=b; break;
    case SDLK_UP:     keyState[KEY_UP]=b; break;
    case SDLK_DOWN:   keyState[KEY_DOWN]=b; break;
    case SDLK_LEFT:   keyState[KEY_LEFT]=b; break;
    case SDLK_RIGHT:  keyState[KEY_RIGHT]=b; break;
    default: break;
  }
}
