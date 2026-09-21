#include "CInput.h"
#include <iostream>

using namespace std;

static CInput* activeInput = NULL; //the one CInput, so a plug/unplug event seen by any event loop reaches it

CInput::CInput(){
  int i;
  for(i=0;i<128;i++) keyState[i]=false;
  for(i=0;i<3;i++) buttonState[i]=false;
  lastButton=0;

  gamepad = NULL;
  activeInput = this;
  openAnyGamepad();
}

CInput::~CInput(){
  closeGamepad();
  if(activeInput == this) activeInput = NULL;
}

void CInput::openAnyGamepad(){
  if(gamepad != NULL) return;
  int count = 0;
  SDL_JoystickID* ids = SDL_GetGamepads(&count);
  for(int i = 0; i < count; i++) {
    gamepad = SDL_OpenGamepad(ids[i]);
    if(gamepad) {
      break;
    } else {
      fprintf(stderr, "Could not open gamepad %u: %s\n", (unsigned int)ids[i], SDL_GetError());
    }
  }
  SDL_free(ids);
}

void CInput::closeGamepad(){
  if(gamepad != NULL) SDL_CloseGamepad(gamepad);
  gamepad = NULL;
}

void CInput::handleDeviceEvent(const SDL_Event& e){
  if(activeInput == NULL) return;
  if(e.type == SDL_EVENT_GAMEPAD_ADDED){
    activeInput->openAnyGamepad(); //does nothing if one is already in use
  } else if(e.type == SDL_EVENT_GAMEPAD_REMOVED){
    if(activeInput->gamepad != NULL && SDL_GetGamepadID(activeInput->gamepad) == e.gdevice.which){
      activeInput->closeGamepad();
      activeInput->openAnyGamepad(); //fall back to another connected gamepad, if there is one
    }
  }
}

bool DA_PollEvent(SDL_Event* e){
  bool got = SDL_PollEvent(e);
  if(got) CInput::handleDeviceEvent(*e);
  return got;
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
  while( DA_PollEvent( &e ) )	{  
    if( e.type == SDL_EVENT_KEY_DOWN ) setKey(e.key.key,true);
    else if( e.type == SDL_EVENT_KEY_UP ) setKey(e.key.key,false);
    else if( e.type == SDL_EVENT_MOUSE_BUTTON_DOWN ) setButton(e.button.button,true);
    else if( e.type == SDL_EVENT_MOUSE_BUTTON_UP ) setButton(e.button.button,false);
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
    case SDLK_Q:      keyState[KEY_Q]=b; break;
    case SDLK_UP:     keyState[KEY_UP]=b; break;
    case SDLK_DOWN:   keyState[KEY_DOWN]=b; break;
    case SDLK_LEFT:   keyState[KEY_LEFT]=b; break;
    case SDLK_RIGHT:  keyState[KEY_RIGHT]=b; break;
    default: break;
  }
}
