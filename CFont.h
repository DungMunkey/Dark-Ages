#ifndef _CFONT_H
#define _CFONT_H

#include <string>

#ifdef GCC
#include <SDL2/SDL_ttf.h>
#else
#include <SDL_ttf.h>
#endif

#include "CDisplay.h"

class CFont {
public:
  CFont();
  ~CFont();

  int fontSize;

  int getFontHeight();
  int getStringWidth(char c);
  int getStringWidth(char* str);
  int getStringWidth(std::string str);
  int getStringWidthN(int num);
  int getStringWidthN(const char* str);
  int getStringWidthN(std::string str);
  bool loadFont(const char* fname);
  void setDisplay(CDisplay* d);
  void setFontSize(int sz);
  bool setText(char c, SDL_Texture*& dest);
  void render(int x, int y, char* str, int color = 0, bool rotate = false);
  void render(int x, int y, std::string s, int color = 0, bool rotate = false);
  void renderInt(int x, int y, int num, int color = 0, bool rotate = false);
  int  renderWrap(int x, int y, std::string s, int wrap, int lineHeight, int color = 0); //width of box in pixels. Word wrap past that point. Returns number of lines rendered.

private:
  CDisplay*   display;
  TTF_Font*   font;

  SDL_Texture* texture[65][128]; //[size][character]; one glyph per size/char - color is applied at render time via SDL_SetTextureColorMod

  SDL_Rect rect[65][128];

  int height;
  int width;

};

#endif
