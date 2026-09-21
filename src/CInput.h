#ifndef _CINPUT_H
#define _CINPUT_H

#include <SDL.h>
#include <stdio.h>

enum {
  KEY_A,
  KEY_B,
  KEY_Q,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT
};

class CInput{
public:
  CInput();
  ~CInput();
  bool getButtonState (int k);
  bool getKeyState    (int k);
  int  mouseAction    ();
  bool isPressed      (int k);
  bool isReleased     (int k);
  void pollEvents     ();

  SDL_GameController *controller;

private:
  SDL_Event e;
  bool keyState[128];
  bool buttonState[3];
  int  lastButton;

  void setButton(Uint8 k, bool b);
  void setKey(SDL_Keycode k, bool b);
};

#endif