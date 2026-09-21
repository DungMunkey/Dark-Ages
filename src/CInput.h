#ifndef _CINPUT_H
#define _CINPUT_H

#include <SDL3/SDL.h>
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

  SDL_Gamepad *gamepad; //the gamepad being listened to, or NULL when none is connected

  //Opens or closes gamepads as they are plugged in or removed. SDL3 does not open gamepads by itself, and a gamepad
  //that isn't open sends no button events. Every event loop in the game reads events through DA_PollEvent() (below),
  //which calls this, so a controller plugged in mid-game just starts working.
  static void handleDeviceEvent(const SDL_Event& e);

private:
  SDL_Event e;
  bool keyState[128];
  bool buttonState[3];
  int  lastButton;

  void setButton(Uint8 k, bool b);
  void setKey(SDL_Keycode k, bool b);
  void openAnyGamepad(); //opens the first connected gamepad that will open, if none is open yet
  void closeGamepad();
};

//SDL_PollEvent() plus gamepad plug/unplug handling; the event is still returned to the caller.
bool DA_PollEvent(SDL_Event* e);

#endif
