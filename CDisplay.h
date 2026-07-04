#ifndef _CDISPLAY_H
#define _CDISPLAY_H

#include "Structs.h"
#include <SDL.h>
#include <stdio.h>
#include <string>
#include <vector>

typedef struct sDAVidMode{
  int w;
  int h;
  std::string name;
}sDAVidMode;

class CDisplay {
public:
  CDisplay();
  ~CDisplay();

  //SDL_Surface*  screenSurface;
  SDL_Renderer* renderer;
  SDL_Window*   window;
  
  bool init(sConf& conf);
  std::vector<sDAVidMode> screenModes;
  int           screenHeight;
  int           screenWidth;
  size_t        currentScreenMode;
  SDL_Color     txtColors[20];

private:
  

  

};

#endif