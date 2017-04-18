#ifndef _CFONT_H
#define _CFONT_H

#include <string>

#ifdef GCC
#include <SDL2/SDL_ttf.h>
#else
#include <SDL_ttf.h>
#endif

#include "CDisplay.h"

using namespace std;

class CFont {
public:
  CFont();
  ~CFont();

  int fontSize;

  int getFontHeight();
  int getStringWidth(char c);
  int getStringWidth(char* str);
  int getStringWidth(string str);
  bool loadFont(char* fname);
  void setDisplay(CDisplay* d);
  void setFontSize(int sz);
  bool setText(char c, SDL_Texture*& dest, int color = 0);
  void render(int x, int y, char* str, int color = 0, bool rotate = false);
  void render(int x, int y, string s, int color = 0, bool rotate = false);

private:
  CDisplay*   display;
  TTF_Font*   font;

  SDL_Texture* texture[41][20][128]; //allow for up to 20 colors

  SDL_Rect rect[41][128];

  int height;
  int width;

};

#endif