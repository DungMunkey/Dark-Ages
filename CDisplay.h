#ifndef _CDISPLAY_H
#define _CDISPLAY_H

#include <SDL.h>
#include <stdio.h>
#include <string>

using namespace std;

class CDisplay {
public:
  CDisplay();
  ~CDisplay();

  //SDL_Surface*  screenSurface;
  SDL_Renderer* renderer;
  SDL_Window*   window;
  
  bool init();
  int           screenHeight;
  int           screenWidth;
  SDL_Color     txtColors[20];

private:
  

  

};

#endif